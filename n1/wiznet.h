#ifndef _N1_WIZNET_H_
#define _N1_WIZNET_H_

typedef word tx_ptr_t;
typedef byte errnum;

struct wiznet {
    struct wiz_port *wiz_port;
} Wiznet;

void Wiznet_Init(void);

tx_ptr_t WizReserveToSend( word n);
tx_ptr_t WizBytesToSend( tx_ptr_t tx_ptr,
                        const byte* data, word n);
void WizFinalizeSend( word n);

errnum WizRecvGetBytesWaiting(word* bytes_waiting_out);
errnum WizRecvChunkTry( byte* buf, word n);

#define RING_SIZE 2048
#define RING_MASK (RING_SIZE - 1)
#define WIZ (Wiznet.wiz_port)
#define OKAY ((errnum)0)
#define NOTYET ((errnum)1)

#endif // _N1_WIZNET_H_
