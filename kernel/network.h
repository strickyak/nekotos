struct quint {
    byte cmd;
    word n;
    word p;
};

void Network_Log(const char* s);

void CheckReceived(void);
void WizSend(byte* addr, word size);

void HelloMCP();
void Network_Init(void);

#define NEKOT_POKE 66
#define NEKOT_CALL 67
#define NEKOT_LAUNCH 68
