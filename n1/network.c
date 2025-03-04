#include "n1/private.h"

byte logbuf[30];

word strlen(const char* s) {
    const char* p = s;
    while (*p) ++p;
    return p-s;
}

void WizSend(byte* addr, word size) {
    tx_ptr_t t = WizReserveToSend(size);
    t = WizBytesToSend(t, addr, size);
    WizFinalizeSend(size);
}

// HACK
void Network_Log(const char* s) {
    word n = strlen(s);
    logbuf[0] = CMD_LOG;
    Poke2(logbuf+1, n);
    Poke2(logbuf+3, 0);
    MemCopy(logbuf+5, (byte*)s, n);

    WizSend(logbuf, n+5);
#if 0
    logbuf[0] = CMD_ECHO;
    logbuf[5]++;

    WizSend(logbuf, n+5);
#endif
}

MORE_DATA byte recv_head[5];
MORE_DATA byte recv_buf[64];

bool need_recv_payload;

void Fatal2(word arg, const char* why);

bool need_to_start_task;
word task_to_start;

void ExecuteReceivedCommand() {
    byte* h = recv_head;
    byte* b = recv_buf;

    word n = Peek2(h+1);
    word p = Peek2(h+3);

    if (h[0] == CMD_DATA) {
#if 0
            // This was for early debugging.

            // If we receive CMD_DATA,
            // we send it back as CMD_LOG.
            errnum e2 = WizRecvChunkTry(b, n);
            if (e2==NOTYET) return;
            if (e2) Fatal("ERR E",e2);

            MemCopy(logbuf, h, 5);
            MemCopy(logbuf+5, b, n);
            logbuf[0] = CMD_LOG;
            tx_ptr_t t3 = WizReserveToSend(n+5);
            t3 = WizBytesToSend(t3, logbuf, n+5);
            WizFinalizeSend(n+5);
#endif
    } else if (h[0] == NEKOT_POKE) { // 66
            errnum e2 = WizRecvChunkTry((byte*)p, n);
            if (e2==NOTYET) return;
            if (e2) Fatal("ERR E",e2);
    } else if (h[0] == NEKOT_CALL) { // 67
            func fn = (func)p;
            fn();
    } else if (h[0] == NEKOT_LAUNCH) { // 68
            task_to_start = p;
            need_to_start_task = true;
    } else {
        Fatal("xxx h[0]", h[0]);
    }

    need_recv_payload = false;
}

void CheckReceived() {
    if (!need_recv_payload) {
        byte err = WizRecvChunkTry(recv_head, 5);
        if (err==NOTYET) return;
        if (err) Fatal("RECV", err);
        need_recv_payload = true;
    }

    ExecuteReceivedCommand();
    if (need_to_start_task) {
        StartTask(task_to_start);
        need_to_start_task = false;
    }
}

void HelloMCP() {
    struct quint q = {CMD_HELLO_NEKOT, 0, 0};
    WizSend((byte*)&q, 5);
}

void Network_Init() {
    Wiznet_Init();
}
