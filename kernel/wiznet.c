#if NET_TYPE_cocoio

#include "kernel/private.h"

#include "kernel/w5100s_defs.h"

// How to talk to the four hardware ports.
struct wiz_port {
  volatile gbyte command;
  volatile gword addr;
  volatile gbyte data;
};

// Axiom uses Socket 1 (of 0 thru 3).
#define B 0x0500u   // Socket Base
#define T 0x4800u  // Transmit ring
#define R 0x6800u  // Receive ring

//////////////////////////////////////

gbyte WizGet1(gword reg) {
  WIZ->addr = reg;
  return WIZ->data;
}
gword WizGet2(gword reg) {
  WIZ->addr = reg;
  gbyte z_hi = WIZ->data;
  gbyte z_lo = WIZ->data;
  return ((gword)(z_hi) << 8) + (gword)z_lo;
}
void WizGetN(gword reg, void* buffer, gword size) {
  volatile struct wiz_port* wiz = WIZ;
  gbyte* to = (gbyte*)buffer;
  wiz->addr = reg;
  for (gword i = size; i; i--) {
    *to++ = wiz->data;
  }
}
void WizPut1(gword reg, gbyte value) {
  WIZ->addr = reg;
  WIZ->data = value;
}
void WizPut2(gword reg, gword value) {
  WIZ->addr = reg;
  WIZ->data = (gbyte)(value >> 8);
  WIZ->data = (gbyte)(value);
}
void WizPutN(gword reg, const void* data, gword size) {
  volatile struct wiz_port* wiz = WIZ;
  const gbyte* from = (const gbyte*)data;
  wiz->addr = reg;
  for (gword i = size; i; i--) {
    wiz->data = *from++;
  }
}

gword WizTicks() { return WizGet2(0x0082 /*TCNTR Tick Counter*/); }
gbyte WizTocks() { return WizGet1(0x0082 /*TCNTR Tick Counter*/); }

gbool ValidateWizPort(struct wiz_port* p) {
  gbyte status = p->command;
  return (status == 3);
}

void FindWizPort() {
    struct wiz_port* p = (void*)0xFF68;
    if (ValidateWizPort(p)) {
        Wiznet.wiz_port = p;
        return;
    }
    p = (void*)0xFF78;
    if (ValidateWizPort(p)) {
        Wiznet.wiz_port = p;
        return;
    }
    Wiznet.wiz_port = 0;
}

//////////////////////////////////////////

void WizIssueCommand(gbyte cmd) {
  WizPut1(B + SK_CR, cmd);
  while (WizGet1(B + SK_CR)) {
  }
}

void WizWaitStatus(gbyte want) {
  gbyte status;
  gbyte stuck = 200;
  do {
    status = WizGet1(B + SK_SR);
    if (!--stuck) gFatal("W", status);
  } while (status != want);
}


////////////////////////////////////////
////////////////////////////////////////

// returns tx_ptr
tx_ptr_t WizReserveToSend( gword n) {
  // PrintH("ResTS %d;", n);
  // Wait until free space is available.
  gword free_size;
  do {
    free_size = WizGet2(B + SK_TX_FSR0);
    // PrintH("Res free %d;", free_size);
  } while (free_size < n);

  return WizGet2(B + SK_TX_WR0) & RING_MASK;
}

tx_ptr_t WizDataToSend( tx_ptr_t tx_ptr,
                       const char* data, gword n) {
  gword begin = tx_ptr;   // begin: Beneath RING_SIZE.
  gword end = begin + n;  // end:  Sum may not be beneath RING_SIZE.

  if (end >= RING_SIZE) {
    gword first_n = RING_SIZE - begin;
    gword second_n = n - first_n;
    WizPutN(T + begin, data, first_n);
    WizPutN(T, data + first_n, second_n);
  } else {
    WizPutN(T + begin, data, n);
  }
  return (n + tx_ptr) & RING_MASK;
}
tx_ptr_t WizBytesToSend( tx_ptr_t tx_ptr,
                        const gbyte* data, gword n) {
  return WizDataToSend(tx_ptr, (char*)data, n);
}

void WizFinalizeSend( gword n) {
  gword tx_wr = WizGet2(B + SK_TX_WR0);
  tx_wr += n;
  WizPut2(B + SK_TX_WR0, tx_wr);
  WizIssueCommand(SK_CR_SEND);
}

errnum WizCheck() {
  gbyte ir = WizGet1(B + SK_IR);  // Socket Interrupt Register.
  if (ir & SK_IR_TOUT) {         // Timeout?
    return SK_IR_TOUT;
  }
  if (ir & SK_IR_DISC) {  // Disconnect?
    return SK_IR_DISC;
  }
  return OKAY;
}
errnum WizSendChunk( char* data, gword n) {
  // PrintH("Ax WizSendChunk %d@%d : %d %d %d %d %d", n, data, data[0], data[1],
  // data[2], data[3], data[4]);
  errnum e = WizCheck();
  if (e) return e;

  tx_ptr_t tx_ptr = WizReserveToSend(n);
  WizDataToSend(tx_ptr, data, n);
  WizFinalizeSend(n);
  // PrintH("Ax WSC.");
  return OKAY;
}

void WizSend(const gbyte* addr, gword size) {
    gbyte cc_value = gIrqSaveAndDisable();

  errnum e = WizCheck();
  if (e) gFatal("WizSend.WizCheck", e);

    tx_ptr_t t = WizReserveToSend(size);
    t = WizBytesToSend(t, addr, size);
    WizFinalizeSend(size);

    gIrqRestore(cc_value);
}

//////////////////////////////////////////////////////////////////

errnum WizRecvGetBytesWaiting(gword* bytes_waiting_out) {
  errnum e = WizCheck();
  if (e) return e;

  *bytes_waiting_out = WizGet2(B + SK_RX_RSR0);  // Unread Received Size.
  return OKAY;
}

errnum WizRecvChunkTry( gbyte* buf, gword n) {
  gword bytes_waiting = 0;
  errnum e = WizRecvGetBytesWaiting(& bytes_waiting);
  if (e) return e;
  if (bytes_waiting < n) return NOTYET;

  gword rd = WizGet2(B + SK_RX_RD0);
  gword begin = rd & RING_MASK;  // begin: Beneath RING_SIZE.
  gword end = begin + n;         // end: Sum may not be beneath RING_SIZE.

  if (end >= RING_SIZE) {
    gword first_n = RING_SIZE - begin;
    gword second_n = n - first_n;
    WizGetN(R + begin, buf, first_n);
    WizGetN(R, buf + first_n, second_n);
  } else {
    WizGetN(R + begin, buf, n);
  }

  WizPut2(B + SK_RX_RD0, rd + n);
  WizIssueCommand(SK_CR_RECV);  // Inform socket of changed SK_RX_RD.
  return OKAY;
}

errnum WizRecvChunk( gbyte* buf, gword n) {
  // PrintH("WizRecvChunk %d...", n);
  errnum e;
  do {
    e = WizRecvChunkTry(buf, n);
  } while (e == NOTYET);
  // PrintH("WRC %d: %d %d %d %d %d.", n, buf[0], buf[1], buf[2], buf[3],
  // buf[4]);
  return e;
}
errnum WizRecvChunkBytes( gbyte* buf, gword n) {
  return WizRecvChunk(buf, n);
}

////////////////////////////////////////
////////////////////////////////////////

void Wiznet_Init() {
    volatile gbyte* p = Cons + WIZNET_BAR_LOCATION;
    p[-1] = 'W';
    FindWizPort();
    if ((gword)Wiznet.wiz_port == 0xFF68u) {
         p[0] = '6';
    } else if ((gword)Wiznet.wiz_port == 0xFF78u) {
         p[0] = '7';
    }
    PutChar('W');
    PutChar(p[0]);
}

#endif // NET_TYPE_cocoio
