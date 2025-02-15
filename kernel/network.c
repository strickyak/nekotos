#define CMD_LOG 200
#define CMD_DATA 204
#define CMD_ECHO 217

byte logbuf[30];

size_t strlen(const char* s) {
    const char* p = s;
    while (*p) ++p;
    return p-s;
}

// HACK
void Network_Log(const char* s) {
    size_t n = strlen(s);
    logbuf[0] = CMD_LOG;
    Poke2(logbuf+1, n);
    Poke2(logbuf+3, 0);
    MemCopy(logbuf+5, (byte*)s, n);

    tx_ptr_t t = WizReserveToSend(n+5);
    t = WizBytesToSend(t, logbuf, n+5);
    WizFinalizeSend(n+5);

    logbuf[0] = CMD_ECHO;
    logbuf[5]++;
    tx_ptr_t t2 = WizReserveToSend(n+5);
    t2 = WizBytesToSend(t2, logbuf, n+5);
    WizFinalizeSend(n+5);
}

byte recv_head[5];
byte recv_buf[30];

void CheckRecv() {
#if 0
    byte* h = recv_head;
    byte* b = recv_buf;

    errnum e = WizRecvChunkTry(h, 5);
    if (e==NOTYET) return;

    switch (h[0]) {
    case CMD_DATA:
        {
            word n = Peek2(h+1);
            errnum e2 = WizRecvChunkTry(b, n);
            if (e==NOTYET) Fatal("PANIC NY ",0);
            if (e) Fatal("ERR E ",0);

            MemCopy(logbuf, h, 5);
            MemCopy(logbuf+5, b, n);
            logbuf[0] = CMD_LOG;
            tx_ptr_t t3 = WizReserveToSend(n+5);
            t3 = WizBytesToSend(t3, logbuf, n+5);
            WizFinalizeSend(n+5);
        }
        break;
    }
#endif
}
