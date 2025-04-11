#include "kernel/private.h"

// SendPacket sends a Quint Header and a payload
// of 0 to 64 bytes.
void SendPacket(gbyte cmd, gword p, const gbyte* pay, gbyte size) {
  gPoke1(0x5FF0, '$');
  if (0) PutChar('W');
  size = (size > 64) ? 64 : size;
  gPoke1(0x5FF1, 'A');
  if (0) PutChar('A');
  gbyte qbuf[5];
  qbuf[0] = cmd;
  gPoke2(qbuf + 1, size);
  gPoke2(qbuf + 3, p);

  gPoke1(0x5FF1, 'B');
  if (0) PutChar('B');
  gbyte cc_value = gIrqSaveAndDisable();
  gPoke1(0x5FF1, 'C');
  if (0) PutChar('C');
  NET_Send(qbuf, 5);
  if (0) PutChar('D');
  NET_Send(pay, size);
  if (0) PutChar('E');
  gIrqRestore(cc_value);
  if (0) PutChar('F');
  gPoke1(0x5FF1, 'D');
}

// gSendCast is used by games to send gamecasts to other cocos
// in the same game shard.  The size is the payload size.
// It does not count the first two bytes of the struct,
// sender & flags, which are set by the system.
// The receiver is not told the payload size that was sent.
// So use (say) the first byte of the payload for the payload size,
// if you want the receiver to know it.
void gSendCast(const struct gamecast* pay, gbyte size) {
  gAssert(size >= 0);
  gAssert(size <= 60);
  size += 2;  // for two header gbytes.
  SendPacket(NEKOT_GAMECAST, 0, (const gbyte*)pay, size);
}

// Games call this indirectly to send control packets
// to the MCP.
void xSendControlPacket(gword p, const gbyte* pay, gword size) {
  if (0) PutChar('Z');
  SendPacket(NEKOT_CONTROL, p, pay, size);
}

// Games can log a message with the network.
// Dont spam it too badly!
void gNetworkLog(const char* s) {
  SendPacket(CMD_LOG, 0, (const gbyte*)s, strlen(s));
}

gbool need_recv_payload;
gbool need_to_start_task;
gword task_to_start;

struct gamecast* recvcast_root;

void ExecuteReceivedCommand(const gbyte* quint) {
  gbyte cmd = quint[0];
  gword n = gPeek2(quint + 1);
  gword p = gPeek2(quint + 3);

  if (cmd == CMD_DATA) {
    // If we ever send CMD_ECHO, expect CMD_DATA.
  } else if (cmd == NEKOT_MEMCPY) {  // 65
    gbyte six[6];
    gAssert(n == 6);
    errnum e2 = NET_RecvChunkTry((gbyte*)six, n);
    if (e2 == NOTYET) return;  // do not let need_recv_payload get falsified.
    if (e2) gFatal("E-M", e2);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

    void* dst = (void*)gPeek2(six);
    void* src = (void*)gPeek2(six + 2);
    gword siz = gPeek2(six + 4);
    memcpy((char*)dst, (char*)src, siz);

#pragma GCC diagnostic pop

  } else if (cmd == NEKOT_POKE) {  // 66
    errnum e3 = NET_RecvChunkTry((gbyte*)p, n);
    if (e3 == NOTYET) return;  // do not let need_recv_payload get falsified.
    if (e3) gFatal("E-P", e3);

  } else if (cmd == NEKOT_CALL) {  // 67
    gAssert(n == 0);
    gfunc fn = (gfunc)p;
    fn();

  } else if (cmd == NEKOT_LAUNCH) {  // 68
    gAssert(n == 0);
    task_to_start = p;
    need_to_start_task = gTRUE;

  } else if (cmd == NEKOT_GAMECAST) {  // 71
    gbyte cc_value = gIrqSaveAndDisable();

    struct gamecast* chunk;
    if (gKern.in_game) {
      chunk = (struct gamecast*)gAlloc64();
    } else {
      // Chunks don't exist when not in a game.
      chunk = (struct gamecast*)
          CHUNKS_BEGIN;  // Borrow a buffer in the chunk space.
    }

    if (!chunk) {
      gFatal("RECV CAST NOMEM", 0);
    }
    gAssert(2 <= n);   // two mandatory header bytes, and
    gAssert(n <= 62);  // up to 60 payload bytes.

    errnum e4 = NET_RecvChunkTry(chunk, n);
    if (e4 == NOTYET) {
      gFree64(chunk);
      gIrqRestore(cc_value);
      return;  // do not let need_recv_payload get falsified.
    }
    if (e4) gFatal("E-C", e4);

    if (gKern.in_game) {
      // Need to append the chunk to the end of the chain!
      chunk->next = gNULL;
      if (recvcast_root) {
        struct gamecast* ptr;
        // Find the end of the chain, which has no ->next.
        for (ptr = recvcast_root; ptr->next; ptr = ptr->next) {
        }
        // The new chunk is now the ->next.
        ptr->next = chunk;
      } else {
        // No chunks in the list yet, so we become the first.
        recvcast_root = chunk;
      }
    }

    gIrqRestore(cc_value);
  } else {
    gFatal("XRC", cmd);
  }

  need_recv_payload = gFALSE;
}

gbyte RecvQuint[5];
void CheckReceived() {
  gbyte cc_value = gIrqSaveAndDisable();

  if (!need_recv_payload) {
    gbyte err = NET_RecvChunkTry(RecvQuint, 5);
    if (err == NOTYET) goto RESTORE;
    if (err) gFatal("RECV", err);
    need_recv_payload = gTRUE;
  }

#if NETWORK_CLICK
  gPoke1(0xFF22, Vdg.shadow_pia1portb | 0x02);  // 1-bit click
#endif
  ExecuteReceivedCommand(RecvQuint);
#if NETWORK_CLICK
  gPoke1(0xFF22, Vdg.shadow_pia1portb | 0x00);  // 1-bit click
#endif

  if (need_to_start_task) {
    need_to_start_task = gFALSE;
    StartTask(task_to_start);
    // Note StartTask never returns.
    // It will restart the stack, allow IRQs, and launch the task.
  }

RESTORE:
  gIrqRestore(cc_value);
}

struct gamecast* gReceiveCast64() {
  struct gamecast* ptr = gNULL;

  gbyte cc_value = gIrqSaveAndDisable();
  if (recvcast_root) {
    ptr = recvcast_root;
    recvcast_root = ptr->next;
  }
  gIrqRestore(cc_value);

  ptr->next = gNULL;
  return ptr;
}

#define DOUBLE_BYTE(W) (gbyte)((gword)(W) >> 8), (gbyte)(gword)(W)

void HelloMCP() {
  gbyte hello[] = {
      'n',
      'e',
      'k',
      'o',
      't',
      'o',
      's',
      '\0',
      DOUBLE_BYTE(Cons),
      DOUBLE_BYTE(gMAX_PLAYERS),
      DOUBLE_BYTE(&gScore),
      DOUBLE_BYTE(&gWall),
  };

  ColdPrint("G11");
  SendPacket(N_GREETING, 1, hello, sizeof hello);
  ColdPrint("G12");
  SendPacket(N_GREETING, 1, hello, sizeof hello);
  ColdPrint("G2");
  SendPacket(N_GREETING, 2, (const gbyte*)0x0118, 8);  // Hash of NekotOS
  ColdPrint("G0");
}

void Network_Init() {
  NET_Init();
  if (0) PutChar('N');
}
