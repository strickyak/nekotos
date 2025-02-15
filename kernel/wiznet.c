/////////////////////////////////////////

// How to talk to the four hardware ports.
struct wiz_port {
  volatile byte command;
  volatile uint addr;
  volatile byte data;
};

// Axiom uses Socket 1 (of 0 thru 3).
uint const B = 0x500u;   // Socket Base
uint const T = 0x4800u;  // Transmit ring
uint const R = 0x6800u;  // Receive ring

//////////////////////////////////////

byte WizGet1(word reg) {
  WIZ->addr = reg;
  return WIZ->data;
}
word WizGet2(word reg) {
  WIZ->addr = reg;
  byte z_hi = WIZ->data;
  byte z_lo = WIZ->data;
  return ((word)(z_hi) << 8) + (word)z_lo;
}
void WizGetN(word reg, void* buffer, word size) {
  volatile struct wiz_port* wiz = WIZ;
  byte* to = (byte*)buffer;
  wiz->addr = reg;
  for (word i = size; i; i--) {
    *to++ = wiz->data;
  }
}
void WizPut1(word reg, byte value) {
  WIZ->addr = reg;
  WIZ->data = value;
}
void WizPut2(word reg, word value) {
  WIZ->addr = reg;
  WIZ->data = (byte)(value >> 8);
  WIZ->data = (byte)(value);
}
void WizPutN(word reg, const void* data, word size) {
  volatile struct wiz_port* wiz = WIZ;
  const byte* from = (const byte*)data;
  wiz->addr = reg;
  for (word i = size; i; i--) {
    wiz->data = *from++;
  }
}

word WizTicks() { return WizGet2(0x0082 /*TCNTR Tick Counter*/); }
byte WizTocks() { return WizGet1(0x0082 /*TCNTR Tick Counter*/); }

bool ValidateWizPort(struct wiz_port* p) {
  byte status = p->command;
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

void WizIssueCommand(byte cmd) {
  WizPut1(B + SK_CR, cmd);
  while (WizGet1(B + SK_CR)) {
  }
}

void WizWaitStatus(byte want) {
  byte status;
  byte stuck = 200;
  do {
    status = WizGet1(B + SK_SR);
    if (!--stuck) Fatal("W", status);
  } while (status != want);
}


////////////////////////////////////////
////////////////////////////////////////

// returns tx_ptr
tx_ptr_t WizReserveToSend( size_t n) {
  // PrintH("ResTS %x;", n);
  // Wait until free space is available.
  word free_size;
  do {
    free_size = WizGet2(B + SK_TX_FSR0);
    // PrintH("Res free %x;", free_size);
  } while (free_size < n);

  return WizGet2(B + SK_TX_WR0) & RING_MASK;
}

tx_ptr_t WizDataToSend( tx_ptr_t tx_ptr,
                       const char* data, size_t n) {
  word begin = tx_ptr;   // begin: Beneath RING_SIZE.
  word end = begin + n;  // end:  Sum may not be beneath RING_SIZE.

  if (end >= RING_SIZE) {
    word first_n = RING_SIZE - begin;
    word second_n = n - first_n;
    WizPutN(T + begin, data, first_n);
    WizPutN(T, data + first_n, second_n);
  } else {
    WizPutN(T + begin, data, n);
  }
  return (n + tx_ptr) & RING_MASK;
}
tx_ptr_t WizBytesToSend( tx_ptr_t tx_ptr,
                        const byte* data, size_t n) {
  return WizDataToSend(tx_ptr, (char*)data, n);
}

void WizFinalizeSend( size_t n) {
  word tx_wr = WizGet2(B + SK_TX_WR0);
  tx_wr += n;
  WizPut2(B + SK_TX_WR0, tx_wr);
  WizIssueCommand(SK_CR_SEND);
}

errnum WizCheck() {
  byte ir = WizGet1(B + SK_IR);  // Socket Interrupt Register.
  if (ir & SK_IR_TOUT) {         // Timeout?
    return SK_IR_TOUT;
  }
  if (ir & SK_IR_DISC) {  // Disconnect?
    return SK_IR_DISC;
  }
  return OKAY;
}
errnum WizSendChunk( char* data, size_t n) {
  // PrintH("Ax WizSendChunk %x@%x : %x %x %x %x %x", n, data, data[0], data[1],
  // data[2], data[3], data[4]);
  errnum e = WizCheck();
  if (e) return e;
  tx_ptr_t tx_ptr = WizReserveToSend(n);
  WizDataToSend(tx_ptr, data, n);
  WizFinalizeSend(n);
  // PrintH("Ax WSC.");
  return OKAY;
}

//////////////////////////////////////////////////////////////////

errnum WizRecvGetBytesWaiting(word* bytes_waiting_out) {
  errnum e = WizCheck();
  if (e) return e;

  *bytes_waiting_out = WizGet2(B + SK_RX_RSR0);  // Unread Received Size.
  return OKAY;
}

errnum WizRecvChunkTry( char* buf, size_t n) {
  word bytes_waiting = 0;
  errnum e = WizRecvGetBytesWaiting(& bytes_waiting);
  if (e) return e;
  if (bytes_waiting < n) return NOTYET;

  word rd = WizGet2(B + SK_RX_RD0);
  word begin = rd & RING_MASK;  // begin: Beneath RING_SIZE.
  word end = begin + n;         // end: Sum may not be beneath RING_SIZE.

  if (end >= RING_SIZE) {
    word first_n = RING_SIZE - begin;
    word second_n = n - first_n;
    WizGetN(R + begin, buf, first_n);
    WizGetN(R, buf + first_n, second_n);
  } else {
    WizGetN(R + begin, buf, n);
  }

  WizPut2(B + SK_RX_RD0, rd + n);
  WizIssueCommand(SK_CR_RECV);  // Inform socket of changed SK_RX_RD.
  return OKAY;
}

errnum WizRecvChunk( char* buf, size_t n) {
  // PrintH("WizRecvChunk %x...", n);
  errnum e;
  do {
    e = WizRecvChunkTry(buf, n);
  } while (e == NOTYET);
  // PrintH("WRC %x: %x %x %x %x %x.", n, buf[0], buf[1], buf[2], buf[3],
  // buf[4]);
  return e;
}
errnum WizRecvChunkBytes( byte* buf, size_t n) {
  return WizRecvChunk((char*)buf, n);
}

////////////////////////////////////////
////////////////////////////////////////

////////////////////////////////////////
////////////////////////////////////////

void Wiznet_Handler() {
}
void Network_Handler() {
    Wiznet_Handler();
}

void Wiznet_Init() {
    volatile byte* p = Cons;
    FindWizPort();
    if ((uint)Wiznet.wiz_port == 0xFF68) {
         p[4] = '6';
    } else if ((uint)Wiznet.wiz_port == 0xFF78) {
         p[4] = '7';
    } else if ((uint)Wiznet.wiz_port == 0) {
         p[4] = '0';
    } else {
         p[4] = '?';
    }
}
void Network_Init() {
    Wiznet_Init();
}
