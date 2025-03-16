#include "nekot1/private.h"

void SendPacket(gbyte cmd, gword p, const gbyte* pay, gbyte size) {
    gbyte cc_value = gIrqSaveAndDisable();
    gbyte qbuf[5];

    qbuf[0] = cmd;
    gPoke2(qbuf+1, size);
    gPoke2(qbuf+3, p);

    WizSend(qbuf, 5);
    WizSend(pay, size);

    gIrqRestore(cc_value);
}

void gSendCast(const struct gamecast* pay, gbyte size) {
    gAssert(size >= 0);
    gAssert(size <= 60);
    size += 2;  // for two header gbytes.
    SendPacket(NEKOT_GAMECAST, 0, (const gbyte*)pay, size);
}

void xSendControlPacket(gword p, const gbyte* pay, gword size) {
    SendPacket(NEKOT_CONTROL, p, pay, size);
}

void gNetworkLog(const char* s) {
    SendPacket(CMD_LOG, 0, (const gbyte*)s, strlen(s));
}

gbool need_recv_payload;
gbool need_to_start_task;
gword task_to_start;

struct gamecast *recvcast_root;

void ExecuteReceivedCommand(const gbyte* quint) {
    gbyte cmd = quint[0];
    gword n = gPeek2(quint+1);
    gword p = gPeek2(quint+3);

    if (cmd == CMD_DATA) {
        // If we ever send CMD_ECHO, expect CMD_DATA.
    } else if (cmd == NEKOT_MEMCPY) { // 65
        gbyte six[6];
        gAssert(n==6);
        errnum e2 = WizRecvChunkTry((gbyte*)six, n);
        if (e2==NOTYET) return;  // do not let need_recv_payload get falsified.
        if (e2) gFatal("E-M",e2);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

        void* dst = (void*)gPeek2(six);
        void* src = (void*)gPeek2(six+2);
        gword siz = gPeek2(six+4);
        gMemcpy((char*)dst, (char*)src, siz);

#pragma GCC diagnostic pop

    } else if (cmd == NEKOT_POKE) { // 66
        errnum e3 = WizRecvChunkTry((gbyte*)p, n);
        if (e3==NOTYET) return;  // do not let need_recv_payload get falsified.
        if (e3) gFatal("E-P",e3);

    } else if (cmd == NEKOT_CALL) { // 67
        gAssert(n==0);
        gfunc fn = (gfunc)p;
        fn();

    } else if (cmd == NEKOT_LAUNCH) { // 68
        gAssert(n==0);
        task_to_start = p;
        need_to_start_task = gTRUE;

    } else if (cmd == NEKOT_GAMECAST) { // 71
        gbyte cc_value = gIrqSaveAndDisable();

        struct gamecast* chunk = (struct gamecast*) gAlloc64();
        if (!chunk) {
            gFatal("RECV CAST NOMEM", 0);
        }
        gAssert(2 <= n);
        gAssert(n <= 62);

        errnum e4 = WizRecvChunkTry(chunk->payload, n);
        if (e4==NOTYET) {
            gIrqRestore(cc_value);
            return;  // do not let need_recv_payload get falsified.
        }
        if (e4) gFatal("E-C",e4);

        // Need to append the chunk to the end of the chain!
        chunk->next = gNULL;
        if (recvcast_root) {
            struct gamecast* ptr;
            // Find the end of the chain, which has no ->next.
            for (ptr = recvcast_root; ptr->next; ptr = ptr->next) {}
            // The new chunk is now the ->next.
            ptr->next = chunk;
        } else {
            // No chunks in the list yet, so we become the first.
            recvcast_root = chunk;
        }

        chunk->next = recvcast_root;
        recvcast_root = chunk;

        gIrqRestore(cc_value);
    } else {
        gFatal("XRC", cmd);
    }

    need_recv_payload = gFALSE;
}

void CheckReceived() {
    gbyte cc_value = gIrqSaveAndDisable();
    gbyte quint[5];

    if (!need_recv_payload) {
        gbyte err = WizRecvChunkTry(quint, 5);
        if (err==NOTYET) goto RESTORE;
        if (err) gFatal("RECV", err);
        need_recv_payload = gTRUE;
    }

#if NETWORK_CLICK
    gPoke1(0xFF22, Vdg.shadow_pia1portb | 0x02);  // 1-bit click
#endif
    ExecuteReceivedCommand(quint);
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
    struct gamecast *ptr = gNULL;
    gbyte cc_value = gIrqSaveAndDisable();
    if (recvcast_root) {
        ptr = recvcast_root;
        recvcast_root = ptr->next;
    }
    gIrqRestore(cc_value);
    ptr->next = gNULL;
    return ptr;
}

#define DOUBLE_BYTE(W)  (gbyte)((gword)(W) >> 8), (gbyte)(gword)(W)

void HelloMCP() {
    gbyte rev = 1;
    gbyte hello[] = {
        'n', 'e', 'k', 'o', 't', '1', '.', '0',
        DOUBLE_BYTE(Cons),
        DOUBLE_BYTE(gMAX_PLAYERS),
        DOUBLE_BYTE(&gScore),
        DOUBLE_BYTE(&gWall),
    };
    SendPacket(CMD_HELLO_NEKOT, rev, hello, sizeof hello);
}

void Network_Init() {
    Wiznet_Init();
}
