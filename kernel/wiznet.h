#ifndef _KERNEL_WIZNET_H_
#define _KERNEL_WIZNET_H_

typedef gword tx_ptr_t;
typedef gbyte errnum;

struct wiznet {
    struct wiz_port *wiz_port;
} Wiznet;

void Wiznet_Init(void);

tx_ptr_t WizReserveToSend( gword n);
tx_ptr_t WizBytesToSend( tx_ptr_t tx_ptr,
                        const gbyte* data, gword n);
void WizFinalizeSend( gword n);

void WizSend(const gbyte* addr, gword size);

errnum WizRecvGetBytesWaiting(gword* bytes_waiting_out);
errnum WizRecvChunkTry( gbyte* buf, gword n);

#define RING_SIZE 2048
#define RING_MASK (RING_SIZE - 1)
#define WIZ (Wiznet.wiz_port)
#define OKAY ((errnum)0)
#define NOTYET ((errnum)1)

#endif // _KERNEL_WIZNET_H_
