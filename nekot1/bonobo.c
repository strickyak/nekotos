#include "nekot1/private.h"

#define bCONTROL 0xFF68
#define bSTATUS 0xFF69
#define bDATA 0xFF6A

#define bOKAY   0
#define bNOTYET 1

// Low Level Hardware Interface

gbool bProbeHardware() {
    gPoke1(bCONTROL, 252);

    for (gword i = 0; i < 60000u; i++) {
        gbyte status = gPeek1(bSTATUS);
        if (status == 'b') return gTRUE;
        if (status) return gFALSE;
    }
    return 0;
}

void bSendControl(gbyte c) {
    gPoke1(bCONTROL, c);
    while (!gPeek1(bSTATUS)) {}
}

void bSendData(const gbyte *p, gword n) {
    for (gword i = 0; i < n; i++) {
        gPoke1(bDATA, gPeek1(p++));
    }
}

void bReceiveData(gbyte *p, gword n) {
    for (gword i = 0; i < n; i++) {
        gPoke1(p++, gPeek1(bDATA));
    }
}

// Mid Level Operations

gword bQueryAvailableFromMcp() {
    gword x;
    bSendControl(250);
    bReceiveData((gbyte*)&x, sizeof x);
    return x;
}

void bReadFromMcp(gbyte* p, gword n) {
    gAssert(1 <= n);
    gAssert(n <= 100);
    bSendControl(n);
    bReceiveData(p, n);
}

void bWriteToMcp(const gbyte* p, gword n) {
    gAssert(1 <= n);
    gAssert(n <= 100);
    bSendControl(n + 100);
    bSendData(p, n);
    bSendControl(251);  // Push
}

// High Level Interface

errnum BonoboRecvChunkTry( gbyte* buf, gword n) {
    gAssert(1 <= n);
    gAssert(n <= 100);

    gword available = bQueryAvailableFromMcp();
    if (n > available) {
        return bNOTYET;
    }
    bReadFromMcp(buf, n);
    return bOKAY;
}

void BonoboSend(const gbyte* addr, gword n) {
    gAssert(1 <= n);
    gAssert(n <= 100);
    bWriteToMcp(addr, n);
}


void Bonobo_Init() {
    gbool ok = bProbeHardware();
    if (!ok) gFatal("bonobo not found", 0);
}
