typedef word tx_ptr_t;
typedef byte errnum;

struct wiznet {
    struct wiz_port *wiz_port;
};

void Wiznet_Handler(void);
void Network_Handler(void);

void Wiznet_Init(void);
void Network_Init(void);

tx_ptr_t WizReserveToSend( size_t n);
tx_ptr_t WizBytesToSend( tx_ptr_t tx_ptr,
                        const byte* data, size_t n);
void WizFinalizeSend( size_t n);

errnum WizRecvChunkTry( char* buf, size_t n);

#define RING_SIZE 2048
#define RING_MASK (RING_SIZE - 1)
#define WIZ (Wiznet.wiz_port)
#define OKAY ((errnum)0)
#define NOTYET ((errnum)1)
