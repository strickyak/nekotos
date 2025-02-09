struct wiznet {
    struct wiz_port *wiz_port;
};

void Wiznet_Handler(void);
void Network_Handler(void);

void Wiznet_Init(void);
void Network_Init(void);
