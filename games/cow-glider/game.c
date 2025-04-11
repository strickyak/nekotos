#include "nekotos/kernel/public.h"
#include "nekotos/lib/pmode1.h"
#include "nekotos/lib/keyscan_arrows_0to7.h"
#include "nekotos/lib/format.h"

#define BG 0
#define ME 2
#define THEM 3
#define FG 1

// Special Declarations
gSCREEN(Screen, 12);  // Screen for PMODE1

#define MAX_TANKS 8
#define XWRAP (96 << 8)
#define YWRAP (96 << 8)

char logbuf[66] gZEROED;

struct tank {
  int x, y;          // x, y position: fixed binary point, 8b.8b ;  modulo 96, 96
  int xvel, yvel;    // x, y velocity: fixed binary point, 8b.8b ;  change in position per Decisecond.
};

struct state {
    struct tank tank[MAX_TANKS];
} State gZEROED, OldState gZEROED;

struct payload {
    gbyte type;
    struct tank tank;
};

enum PayloadType {
    PT_START,
    PT_STATE,
    PT_COWS,
    PT_OVER,
    PT_ABORT,
};

gbyte NumberOfPlayers;
gbyte Player;

void Spin(gbyte offset) {
  gword southwest = 0x3FE0;
  gword p = southwest + offset;
  gPoke1(p, 1+gPeek1(p));
}

void Hang() {
  while (gALWAYS) {
    Spin(0);
  }
}

void AdvanceTanks() {
  for (gbyte i = 0; i < NumberOfPlayers; i++) {
      struct tank* t = &State.tank[i];

      // Advance positions based on velocity.
      t->x += t->xvel;
      t->y += t->yvel;

      // Wrap to stay on torus.
      while (t->x < 0) {
        t->x += XWRAP;
      }
      while (t->y < 0) {
        t->y += YWRAP;
      }
      while (t->x >= XWRAP) {
        t->x -= XWRAP;
      }
      while (t->y >= YWRAP) {
        t->y -= YWRAP;
      }
  }
}

void DrawScores() {
  gScore.total_updated = gFALSE;

  int y = 2;
  for (gbyte i = 0; i < NumberOfPlayers; i++) {
    PMode1ClearDecimal3x5(Screen, PMode1DrawSpot, 100, y, /*color=*/BG);
    gbyte color = (i == Player) ? ME : THEM;
    PMode1DrawDecimal3x5(Screen, PMode1DrawSpot, 100, y, /*color=*/color, /*value=*/gScore.total_scores[i]);
    y += 7;
  }
}

void drawMod96(signed char x, signed char y, gbyte color) {
    if (x < 0) x += 96;
    if (y < 0) x += 96;
    if (x >= 96) x -= 96;
    if (y >= 96) x -= 96;

    PMode1DrawSpotXor(Screen, (gbyte)x, (gbyte)y, color);
}

void XorOldTanks() {
  for (gbyte i = 0; i < NumberOfPlayers; i++) {
      struct tank* p = &OldState.tank[i];
      signed char x = (signed char)(p->x >> 8); // Shed fractional 8 bits.
      signed char y = (signed char)(p->y >> 8); // Shed fractional 8 bits.

      gbyte color = (i == Player) ? ME : THEM;
      drawMod96(x, y, color);
      drawMod96(x-1, y, color);
      drawMod96(x+1, y, color);
      drawMod96(x, y-1, color);
      drawMod96(x, y+1, color);
  }
}

void ProcessPacket(struct gamecast* packet) {
#if 0
  gword p = (gword)packet;
  gbyte sender = gPeek1(p);
  gbyte flags = gPeek1(p+1);
  gbyte type = gPeek1(p+2);
  gword x = gPeek2(p+3);
  gword y = gPeek2(p+5);

  Sprintf(logbuf, "nando %x %x: %x/%x/%x %x,%x", packet, p, sender, flags, type, x, y);
  gNetworkLog(logbuf);

  if (type==1 && sender != Player) {
    memcpy( &State.tank[sender], p+3, sizeof (struct tank) );
    Sprintf( logbuf, "memcpy %x %x %x", &State.tank[sender], p+3, sizeof (struct tank) );
    gNetworkLog(logbuf);
  }

#else
  struct payload* pay = (struct payload*) packet->payload;

/*
  Sprintf(logbuf, "GCPP pack=%x pay=%x", packet, pay);
  gNetworkLog(logbuf);
  Sprintf(logbuf, "GCPP S=%x F=%x T=%x", packet->sender, packet->flags, pay->type);
  gNetworkLog(logbuf);
  Sprintf(logbuf, "GCPP pp %x  %x %x  %x %x  %x %x  %x %x",
    packet->payload[0],
    packet->payload[1],
    packet->payload[2],
    packet->payload[3],
    packet->payload[4],
    packet->payload[5],
    packet->payload[6],
    packet->payload[7],
    packet->payload[8]);
  gNetworkLog(logbuf);
  Sprintf(logbuf, "GCPP sp %x  %x %x  %x %x",
    pay->type,
    pay->tank.x,
    pay->tank.y,
    pay->tank.xvel,
    pay->tank.yvel);
  gNetworkLog(logbuf);
  Sprintf(logbuf, "GCPP p0=%x type=%x x=%x", &packet->payload[0], &pay->type, &pay->tank.x);
  gNetworkLog(logbuf);
*/
  switch (pay->type) {
  case PT_STATE:
      // Only copy the update if it is from another player.
      if (packet->sender != Player) {
        memcpy( &State.tank[packet->sender] , &pay->tank, sizeof pay->tank);
        //MMMM State.tank[packet->sender] = pay->tank;
      }
      break;
  }
#endif
}

void CheckIncomingPackets(gbyte n) {
    for (gbyte i = 0; i < n; i++) {
        struct gamecast* packet = gReceiveCast64();
        if (!packet) {
            break;
        }
        ProcessPacket(packet);
        gFree64(packet);
    }
}

struct tank* mytank;

void setup() {
  NumberOfPlayers = gScore.number_of_players;
  Player = gScore.player;
  mytank = &State.tank[Player];

  gNetworkLog("setup nando");
  Sprintf(logbuf, "IN GLIDER GAME P %d COUNT %d", Player, NumberOfPlayers);
  gNetworkLog(logbuf);

  memset(&State, 0, sizeof State);

  for (gbyte i = 0; i < NumberOfPlayers; i++) {
      struct tank* t = &State.tank[i];
      t->x = 20<<8;
      t->y = (10 * i + 5)<<8;
      t->xvel = 0;
      t->yvel=  0;
  }

  PMode1ClearScreen(Screen, /*color=*/BG);
  gPMode1Screen(Screen, /*colorset=*/0);

  PMode1DrawHorz(Screen, PMode1DrawSpot, /*x=*/0, /*y=*/0, /*color=*/FG, /*len=*/96);
  PMode1DrawHorz(Screen, PMode1DrawSpot, /*x=*/0, /*y=*/1, /*color=*/FG, /*len=*/96);
  PMode1DrawHorz(Screen, PMode1DrawSpot, /*x=*/0, /*y=*/94, /*color=*/FG, /*len=*/96);
  PMode1DrawHorz(Screen, PMode1DrawSpot, /*x=*/0, /*y=*/95, /*color=*/FG, /*len=*/96);

  PMode1DrawVirt(Screen, PMode1DrawSpot, /*x=*/0, /*y=*/0, /*color=*/FG, /*len=*/96);
  PMode1DrawVirt(Screen, PMode1DrawSpot, /*x=*/1, /*y=*/0, /*color=*/FG, /*len=*/96);
  PMode1DrawVirt(Screen, PMode1DrawSpot, /*x=*/94, /*y=*/0, /*color=*/FG, /*len=*/96);
  PMode1DrawVirt(Screen, PMode1DrawSpot, /*x=*/95, /*y=*/0, /*color=*/FG, /*len=*/96);

  OldState = State;
  XorOldTanks();
  DrawScores();
}

gword loops;
gbyte decis = 0;
gbyte counter = 0;
void loop() {
  ++loops;

  if (gReal.decis != decis) {
    ++counter;

    if (!(loops < 20 || gReal.decis == decis+1 || (gReal.decis == 0 && decis == 9))) {
        Spin(4);
    }

    // Every 10th of a second.
    decis = gReal.decis;

    if (decis == 0) {
        // Once per second, send GameCast of State.
        static struct gamecast packet;
        struct payload* pay = (struct payload*)packet.payload;

        packet.flags = 0;
        pay->type = PT_STATE; // Payload Type
        pay->tank = *mytank;   // copy entire struct
        gSendCast(&packet, sizeof *pay);
    } else {
            // On other 9 decis per second, receive packets and scan keyboard.

            CheckIncomingPackets(2);

            gword keys = ScanArrowsAnd0To7();
            if (keys) {

                if (keys & ArrowsAnd0To7_0) { gScore.partial_scores[0]++; gScore.partial_dirty = gTRUE; }
                if (keys & ArrowsAnd0To7_1) { gScore.partial_scores[1]++; gScore.partial_dirty = gTRUE; }
                if (keys & ArrowsAnd0To7_2) { gScore.partial_scores[2]++; gScore.partial_dirty = gTRUE; }
                if (keys & ArrowsAnd0To7_3) { gScore.partial_scores[3]++; gScore.partial_dirty = gTRUE; }
                if (keys & ArrowsAnd0To7_4) { gScore.partial_scores[4]++; gScore.partial_dirty = gTRUE; }
                if (keys & ArrowsAnd0To7_5) { gScore.partial_scores[5]++; gScore.partial_dirty = gTRUE; }
                if (keys & ArrowsAnd0To7_6) { gScore.partial_scores[6]++; gScore.partial_dirty = gTRUE; }
                if (keys & ArrowsAnd0To7_7) { gScore.partial_scores[7]++; gScore.partial_dirty = gTRUE; }

    #define DELTA_VEL 16
                if (keys & ArrowsAnd0To7_LEFT) { mytank->xvel -= DELTA_VEL; }
                if (keys & ArrowsAnd0To7_RIGHT) { mytank->xvel += DELTA_VEL; }
                if (keys & ArrowsAnd0To7_UP) { mytank->yvel -= DELTA_VEL; }
                if (keys & ArrowsAnd0To7_DOWN) { mytank->yvel += DELTA_VEL; }
            }
    }

    XorOldTanks(); // undo old tank
    AdvanceTanks();
    OldState = State;
    XorOldTanks();  // new position

    if (gScore.total_updated) DrawScores();
  }
}
