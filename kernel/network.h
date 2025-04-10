#ifndef _KERNEL_NETWORK_H_
#define _KERNEL_NETWORK_H_

// Make noise on incoming packets.
#define NETWORK_CLICK 1

// struct quint is the 5-gbyte header of
// every packet embedded in the TCP stream
// to and from the MCP.  p varies with the
// command.  n is the number of bytes to
// immediately follow for the payload.
struct quint {
    gbyte cmd;
    gword n;
    gword p;
};

void gNetworkLog(const char* s);

void CheckReceived(void);
void WizSend(const gbyte* addr, gword size);
void xSendControlPacket(gword p, const gbyte* pay, gword size);

void HelloMCP();
void Network_Init(void);

// Cmd bytes used by Nekot.
#define N_GREETING 64
#define NEKOT_MEMCPY 65
#define NEKOT_POKE 66
#define NEKOT_CALL 67
#define NEKOT_LAUNCH 68

// Cmd bytes from coco to MCP.
#define NEKOT_KEYSCAN  69
#define NEKOT_CONTROL  70
#define NEKOT_GAMECAST  71

// Cmd bytes inherited from Lemma.
#define CMD_LOG 200
#define CMD_DATA 204
#define CMD_ECHO 217

#if NET_TYPE_cocoio
  #define  NET_Send         WizSend
  #define  NET_RecvChunkTry WizRecvChunkTry
  #define  NET_Init         Wiznet_Init
#endif

#if NET_TYPE_bonobo
  #define  NET_Send          BonoboSend
  #define  NET_RecvChunkTry  BonoboRecvChunkTry
  #define  NET_Init          Bonobo_Init
#endif

#endif // _KERNEL_NETWORK_H_
