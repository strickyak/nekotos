#define CMD_LOG 200
#define CMD_DATA 204
#define CMD_ECHO 217

byte logbuf[30];

size_t strlen(const char* s) {
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
    size_t n = strlen(s);
    logbuf[0] = CMD_LOG;
    Poke2(logbuf+1, n);
    Poke2(logbuf+3, 0);
    MemCopy(logbuf+5, (byte*)s, n);

    WizSend(logbuf, n+5);

    logbuf[0] = CMD_ECHO;
    logbuf[5]++;

    WizSend(logbuf, n+5);
}

MORE_DATA byte recv_head[5];
MORE_DATA byte recv_buf[64];

bool need_recv_payload;

void Fatal2(word arg, const char* why);


void ExecuteReceivedCommand() {
    byte* h = recv_head;
    byte* b = recv_buf;

    word n = Peek2(h+1);
    word p = Peek2(h+3);
//Console_Printf(" [%d.%d.%d] ", h[0], n, p);

    if (h[0] == CMD_DATA) {
            // If we receive CMD_DATA,
            // we send it back as CMD_LOG.
            errnum e2 = WizRecvChunkTry(b, n);
            if (e2==NOTYET) return;
            if (e2) Fatal("ERR E ",e2);

            MemCopy(logbuf, h, 5);
            MemCopy(logbuf+5, b, n);
            logbuf[0] = CMD_LOG;
            tx_ptr_t t3 = WizReserveToSend(n+5);
            t3 = WizBytesToSend(t3, logbuf, n+5);
            WizFinalizeSend(n+5);
    } else if (h[0] == NEKOT_POKE) { // 66
            errnum e2 = WizRecvChunkTry((byte*)p, n);
            if (e2==NOTYET) return;
            if (e2) Fatal("ERR E ",e2);
    } else if (h[0] == NEKOT_CALL) { // 67
            func fn = (func)p;
            fn();
    } else if (h[0] == NEKOT_LAUNCH) { // 68
            // Remember, if p==0 then
            // it starts the no-game task.
            StartTask(p);
    } else {
        Fatal("xxx h[0]", h[0]);
    }

    need_recv_payload = false;
}

void CheckReceived() {
#if 1 
    word bytes_waiting_out = 0;
    errnum e = WizRecvGetBytesWaiting(&bytes_waiting_out);
#endif

#if 1
    byte* h = recv_head;
    //fut// byte* b = recv_buf;

    if (!need_recv_payload) {
        byte e9 = WizRecvChunkTry(h, 5);
// Console_Printf("hmm(e%u) ", e9);
        if (e9==NOTYET) return;
        if (e9) Fatal("RECV", e);
        need_recv_payload = true;
    }

    ExecuteReceivedCommand();
#endif
}

void HelloMCP() {
#define CMD_HELLO_NEKOT 64
    struct quint q = {CMD_HELLO_NEKOT, 0, 0};
    WizSend((byte*)&q, 5);
}


void Network_Init() {
    Wiznet_Init();
}
