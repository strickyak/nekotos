#include "nekot1/private.h"

gbyte logbuf[30];

word strlen(const char* s) {
    const char* p = s;
    while (*p) ++p;
    return p-s;
}

void WizSend(gbyte* addr, word size) {
    gbyte cc_value = gIrqSaveAndDisable();

    tx_ptr_t t = WizReserveToSend(size);
    t = WizBytesToSend(t, addr, size);
    WizFinalizeSend(size);

    gIrqRestore(cc_value);
}

void gSendClientPacket(word p, char* pay, word size) {
    gbyte cc_value = gIrqSaveAndDisable();

    logbuf[0] = NEKOT_CLIENT;
    Poke2(logbuf+1, size);
    Poke2(logbuf+3, p);

    WizSend(logbuf, 5);
    WizSend(pay, size);

    gIrqRestore(cc_value);
}

void gNetworkLog(const char* s) {
    gbyte cc_value = gIrqSaveAndDisable();
    // Still uses CMD_LOG=200.  TODO convert to CLIENT=70.
    word n = strlen(s);
    logbuf[0] = CMD_LOG;
    Poke2(logbuf+1, n);
    Poke2(logbuf+3, 0);
    MemCopy(logbuf+5, (gbyte*)s, n);

    WizSend(logbuf, n+5);
    gIrqRestore(cc_value);
}

MORE_DATA gbyte recv_head[5];
MORE_DATA gbyte recv_buf[64];

gbool need_recv_payload;
gbool need_to_start_task;
word task_to_start;

void ExecuteReceivedCommand() {
    gbyte* h = recv_head;
    gbyte* b = recv_buf;

    word n = Peek2(h+1);
    word p = Peek2(h+3);

    if (h[0] == CMD_DATA) {
        // If we ever send CMD_ECHO, expect CMD_DATA.
    } else if (h[0] == NEKOT_MEMCPY) { // 65
        errnum e2 = WizRecvChunkTry((gbyte*)b, n);
        if (e2==NOTYET) return;
        if (e2) Fatal("E-M",e2);

        MemCopy((gbyte*)Peek2(b), (gbyte*)Peek2(b+2), Peek2(b+4));
    } else if (h[0] == NEKOT_POKE) { // 66
        errnum e2 = WizRecvChunkTry((gbyte*)p, n);
        if (e2==NOTYET) return;
        if (e2) Fatal("E-P",e2);
    } else if (h[0] == NEKOT_CALL) { // 67
        func fn = (func)p;
        fn();
    } else if (h[0] == NEKOT_LAUNCH) { // 68
        task_to_start = p;
        need_to_start_task = true;
    } else {
        Fatal("XRC", h[0]);
    }

    need_recv_payload = false;
}

void CheckReceived() {
    gbyte cc_value = gIrqSaveAndDisable();

    if (!need_recv_payload) {
        gbyte err = WizRecvChunkTry(recv_head, 5);
        if (err==NOTYET) goto RESTORE;
        if (err) Fatal("RECV", err);
        need_recv_payload = true;
    }

#if NETWORK_CLICK
    Poke1(0xFF22, Vdg.shadow_pia1portb | 0x02);  // 1-bit click
#endif
    ExecuteReceivedCommand();
#if NETWORK_CLICK
    Poke1(0xFF22, Vdg.shadow_pia1portb | 0x00);  // 1-bit click
#endif

    if (need_to_start_task) {
// Console_Printf("NEED(%d).", task_to_start);
        need_to_start_task = false;
        StartTask(task_to_start);
        // Note StartTask never returns.
        // It will launch the task and allow IRQs.
    }

RESTORE:
    gIrqRestore(cc_value);
}

void HelloMCP() {
    struct quint q = {CMD_HELLO_NEKOT, 0, 0};
    WizSend((gbyte*)&q, 5);
}

void Network_Init() {
    Wiznet_Init();
}
