#define CMD_LOG 200

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
    MemCopy(logbuf+5, s, n);

    tx_ptr_t t = WizReserveToSend(n+5);
    t = WizBytesToSend(t, logbuf, n+5);
    WizFinalizeSend(n+5);
}
