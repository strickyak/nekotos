#if NET_TYPE_bonobo

#include "nekot1/private.h"

#define bCONTROL 0xFF68
#define bSTATUS 0xFF69
#define bDATA 0xFF6A

#define bOKAY   0
#define bNOTYET 1

// Low Level Hardware Interface

#define XXX if(false)

void Delay(gword n) {
    for (gword i = 0 ;i < n; i++) {
        asm volatile("mul" : : : "d");
        asm volatile("mul" : : : "d");
        asm volatile("mul" : : : "d");
        asm volatile("mul" : : : "d");
        asm volatile("mul" : : : "d");
    }
}

gbool bProbeHardware() {
    Printf("Probe ");
    gPoke1(bCONTROL, 252); // Firmware Probe

    for (gword i = 0; i < 16; i++) {
        gbyte status = gPeek1(bSTATUS);
        Printf("->%d.", status);

        if (status == 'b') { // Firmware Reply
            Printf(" probe OK\n");
            return gTRUE;
        }
        /*
        if (status) {   
            PutChar('?');
            return gFALSE;
        }
        */
    }
    PutChar('!');
    return gFALSE;
}

void bSendControl(gbyte c) {
    //PutChar('<');
    //PutDec(c);
    //PutChar('>');
    gPoke1(bCONTROL, c);
    //Delay(50);

    gbyte status;
#if 1
    for (gbyte i = 0; i < 4; i++) {
        Delay(5);
        status = gPeek1(bCONTROL);
        //Printf("^%d", status);
        if (status == 1) break;
    }
#else
    do {
        status = gPeek1(bSTATUS);
        //Printf("^%d", status);
    } while (status != 0);
#endif
}

void bSendData(const gbyte *p, gword n) {
    //PutChar('S');
    //PutDec(n);
    //PutChar(';');
    for (gword i = 0; i < n; i++) {
        Delay(5);
        gPoke1(bDATA, p[i]);
    }
}

void bReceiveData(gbyte *p, gword n) {
  if (n != 2 ) {
    //PutChar('R');
    //PutDec(n);
    //PutChar(';');
  }
    for (gword i = 0; i < n; i++) {
        Delay(5);
        p[i] = gPeek1(bDATA);
  if (n != 2 ) {
        if (i<8) {
            //PutDec(p[i]);
            //PutChar(',');
        }
  }
    }
  if (n != 2 ) {
    //PutChar(';');
  }
}

// Mid Level Operations

gword bQueryAvailableFromMcp() {
  gword x;
  do {
    //PutStr("(Q");
    bSendControl(250);
    bReceiveData((gbyte*)&x, sizeof x);
    //PutDec(x);
    //PutChar(')');

    if (x > 4) {
        //Printf(" Q%d ", x);
    }
    if (x == 7967) {
        // gFatal("31&31", x);
        //Delay(1000);
    }
  } while ( x == 7967); 
    return x;
}

void bReadFromMcp(gbyte* p, gword n) {
    //PutStr("(R");
    gAssert(1 <= n);
    gAssert(n <= 100);
    bSendControl(n);
    bReceiveData(p, n);
    //PutChar(')');
}

void bWriteToMcp(const gbyte* p, gword n) {
    //PutStr("(W");
    gAssert(1 <= n);
    gAssert(n <= 100);

    //PutChar('@');
    //PutDec(n);
    for (gword i =0; i < n; i++) {
        //PutChar('+');
        //PutDec(p[i]);
    }
    //PutChar(',');

    bSendControl(n + 100);
    bSendData(p, n);
    bSendControl(251);  // Push
    //PutChar(')');
}

// High Level Interface

errnum BonoboRecvChunkTry( gbyte* buf, gword n) {
    gbyte cc_value = gIrqSaveAndDisable();
    gAssert(1 <= n);
    gAssert(n <= 100);

    gword available = bQueryAvailableFromMcp();
    if (n > available) {
        return bNOTYET;
    }
    bReadFromMcp(buf, n);
    gIrqRestore(cc_value);
    return bOKAY;
}

void BonoboSend(const gbyte* addr, gword n) {
    gbyte cc_value = gIrqSaveAndDisable();
    gAssert(1 <= n);
    gAssert(n <= 100);
    bWriteToMcp(addr, n);
    gIrqRestore(cc_value);
}

void Bonobo_Init() {
    gbool ok = bProbeHardware();
    if (!ok) gFatal("bonobo not found", 0);
    PutChar('B');
}

#endif // NET_TYPE_bonobo
