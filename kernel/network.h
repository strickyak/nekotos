struct quint {
    byte cmd;
    word n;
    word p;
};

void Network_Log(const char* s);

void CheckRecv(void);

void HelloMCP();
