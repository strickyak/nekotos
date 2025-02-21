// kern.h

struct kern {
    bool volatile always_true;
    bool volatile in_game;
    bool volatile in_irq;
};

void NoGameMain();
void Network_Handler();
