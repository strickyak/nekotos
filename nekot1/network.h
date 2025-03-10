#ifndef _NEKOT1_NETWORK_H_
#define _NEKOT1_NETWORK_H_

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
void WizSend(gbyte* addr, gword size);
void xSendClientPacket(gword p, char* pay, gword size);

void HelloMCP();
void Network_Init(void);

// Cmd bytes used by Nekot.
#define NEKOT_MEMCPY 65
#define NEKOT_POKE 66
#define NEKOT_CALL 67
#define NEKOT_LAUNCH 68

// Cmd bytes from coco to MCP.
#define NEKOT_KEYSCAN  69
#define NEKOT_CLIENT  70

// Cmd bytes inherited from Lemma.
#define CMD_HELLO_NEKOT 64
#define CMD_LOG 200
#define CMD_DATA 204
#define CMD_ECHO 217

#endif // _NEKOT1_NETWORK_H_
