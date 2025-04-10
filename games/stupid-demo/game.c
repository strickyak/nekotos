#include "nekotos/kernel/public.h"
#include "nekotos/lib/pmode1.h"
#include "nekotos/lib/keyscan_arrows_0to7.h"

#define BG 0
#define ME 2
#define THEM 3
#define FG 1

// Special Declarations
gSCREEN(Screen, 12);  // Screen for PMODE1

struct tank {
  int x, y;          // x, y position: fixed binary point, 8b.8b ;  modulo 96, 96
  int xvel, yvel;    // x, y velocity: fixed binary point, 8b.8b ;  change in position per Decisecond.
};

#define MAX_TANKS 8
struct state {
    struct tank tank[MAX_TANKS];
} State;

#define XWRAP (96 << 8)
#define YWRAP (96 << 8)

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
  for (gbyte i = 0; i < gScore.number_of_players; i++) {
      struct tank* p = &State.tank[i];

      p->x += p->xvel;
      p->y += p->yvel;

      // Wrap to stay on torus.
      while (p->x < 0) {
        p->x += XWRAP;
      }
      while (p->y < 0) {
        p->y += YWRAP;
      }
      while (p->x >= XWRAP) {
        p->x -= XWRAP;
      }
      while (p->y >= YWRAP) {
        p->y -= YWRAP;
      }
  }
}

void DrawScores() {
  gScore.total_updated = gFALSE;

  int y = 2;
  for (gbyte i = 0; i < gScore.number_of_players; i++) {
    // PMode1ClearBoxAlignX4(Screen, /*nw_x=*/100, /*nw_y=*/y, /*wid=*/32, /*hei=*/4, /*color=*/BG);

    PMode1ClearDecimal3x5(Screen, PMode1DrawSpot, 100, y, /*color=*/BG);
    gbyte color = (i == gScore.player) ? ME : THEM;
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

void XorTanks() {
  for (gbyte i = 0; i < gScore.number_of_players; i++) {
      struct tank* p = &State.tank[i];
      signed char x = (signed char)(p->x >> 8); // Shed fractional 8 bits.
      signed char y = (signed char)(p->y >> 8); // Shed fractional 8 bits.

      gbyte color = (i == gScore.player) ? ME : THEM;
      drawMod96(x, y, color);
      drawMod96(x-1, y, color);
      drawMod96(x+1, y, color);
      drawMod96(x, y-1, color);
      drawMod96(x, y+1, color);
  }
}

struct payload {
    struct tank tank;
};

void ProcessPacket(struct gamecast* packet) {
  gbyte sender = packet->sender;
  struct payload* pay = (struct payload*) packet->payload;

  // Only use the update if it is from another player.
  if (sender != gScore.player) {
    State.tank[sender] = pay->tank;
  }
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
  mytank = &State.tank[gScore.player];
  mytank->x = 20<<8;
  mytank->y = 40<<8;
  mytank->xvel = 0;
  mytank->yvel=  0;

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

  XorTanks();
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
            if (keys & ArrowsAnd0To7_LEFT) { mytank->xvel -= DELTA_VEL;
                            //PMode1ClearDecimal3x5(Screen, PMode1DrawSpot, 97, 85, BG);
                            //PMode1DrawDecimal3x5(Screen, PMode1DrawSpot, 97, 85, ME, mytank->xvel);
            }
            if (keys & ArrowsAnd0To7_RIGHT) { mytank->xvel += DELTA_VEL;
                            //PMode1ClearDecimal3x5(Screen, PMode1DrawSpot, 97, 85, BG);
                            //PMode1DrawDecimal3x5(Screen, PMode1DrawSpot, 97, 85, ME, mytank->xvel);
            }
            if (keys & ArrowsAnd0To7_UP) { mytank->yvel -= DELTA_VEL;
                            //PMode1ClearDecimal3x5(Screen, PMode1DrawSpot, 102, 91, BG);
                            //PMode1DrawDecimal3x5(Screen, PMode1DrawSpot, 102, 91, ME, mytank->yvel);
            }
            if (keys & ArrowsAnd0To7_DOWN) { mytank->yvel += DELTA_VEL;
                            //PMode1ClearDecimal3x5(Screen, PMode1DrawSpot, 102, 91, BG);
                            //PMode1DrawDecimal3x5(Screen, PMode1DrawSpot, 102, 91, ME, mytank->yvel);
            }
        }

        XorTanks(); // undo old tank
        AdvanceTanks();
        XorTanks();  // new position

        if (gScore.total_updated) DrawScores();
  }
}
