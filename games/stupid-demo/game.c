#include "nekotos/kernel/public.h"
#include "nekotos/lib/pmode1.h"
#include "nekotos/lib/keyscan_arrows_0to7.h"

#define MAX_TANKS 8

// Special Declarations
gSCREEN(Screen, 12);  // Screen for PMODE1

struct tank {
  int x, y;          // x, y position: fixed binary point, 8b.8b ;  modulo 96, 96
  int xvel, yvel;    // x, y velocity: fixed binary point, 8b.8b ;  change in position per Decisecond.
};

struct state {
    struct tank tank[MAX_TANKS];
} State;

#define XWRAP (96 << 8)
#define YWRAP (96 << 8)

void Spin(gbyte offset) {
  gword southwest = 0x3FE0;
  gword p = southwest + offset;
  gPoke2(p, 1+gPeek2(p));
}

void Hang() {
  while (gALWAYS) {
    Spin(0);
  }
}

#define BG 0
#define ME 1
#define THEM 3
#define FG 2

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
    PMode1DrawDecimal3x5(Screen, PMode1DrawSpot, 100, y, /*color=*/FG, /*value=*/gScore.total_scores[i]);
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

void DrawTanks() {
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
      Spin(12);
  }
}

struct payload {
    struct tank tank;
};

void ProcessPacket(struct gamecast* packet) {
    /* note, from nekotos/kernel/public.h:
        struct gamecast {
            gbyte sender;
            gbyte flags;  // must be zero.
            gbyte payload[60];
            struct gamecast *next;
        };
    */
  gbyte sender = packet->sender;
  struct payload* pay = (struct payload*) packet->payload;

  // Only use the update if it is from another player.
  if (sender != gScore.player) {
    State.tank[sender] = pay->tank;
  }
}

void CheckIncomingPackets() {
    // Loop because there may be more than one packet ready.
    while (gALWAYS) {
        struct gamecast* packet = gReceiveCast64();
        if (!packet) {
            break;
        }
        ProcessPacket(packet);
        gFree64(packet);
    }
}

void setup() {
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

  struct tank* p = &State.tank[gScore.player];
  p->x = 20<<8;
  p->y = 40<<8;
  p->xvel = 27;
  p->yvel=  7;

  PMode1DrawDecimal3x5(Screen, PMode1DrawSpot, 20, 65, THEM, &gScore.number_of_players);
  PMode1DrawDecimal3x5(Screen, PMode1DrawSpot, 20, 75, ME, &gScore.player);
  PMode1DrawDecimal3x5(Screen, PMode1DrawSpot, 10, 60, THEM, gScore.number_of_players);
  PMode1DrawDecimal3x5(Screen, PMode1DrawSpot, 10, 70, ME, gScore.player);
  DrawTanks();
  DrawScores();
}

gbool been_in_loop_before;
void loop() {
  Spin(4);
  gbyte decis = 0;

  CheckIncomingPackets();

  if (gReal.decis != decis) {
    Spin(8);
    // Every 10th of a second.
    decis = gReal.decis;

    gword keys = ScanArrowsAnd0To7();
    if (keys) {
        Spin(1);
        PMode1ClearDecimal3x5(Screen, PMode1DrawSpot, 10, 50, BG);
        PMode1DrawDecimal3x5(Screen, PMode1DrawSpot, 10, 50, ME, keys);
        PMode1ClearDecimal3x5Unsigned(Screen, PMode1DrawSpot, 50, 50, BG);
        PMode1DrawDecimal3x5Unsigned(Screen, PMode1DrawSpot, 50, 50, ME, keys);
        gScore.partial_dirty = gTRUE;

        if (keys & ArrowsAnd0To7_0) gScore.partial_scores[0]++;
        if (keys & ArrowsAnd0To7_1) gScore.partial_scores[1]++;
        if (keys & ArrowsAnd0To7_2) gScore.partial_scores[2]++;
        if (keys & ArrowsAnd0To7_3) gScore.partial_scores[3]++;
        if (keys & ArrowsAnd0To7_4) gScore.partial_scores[4]++;
        if (keys & ArrowsAnd0To7_5) gScore.partial_scores[5]++;
        if (keys & ArrowsAnd0To7_6) gScore.partial_scores[6]++;
        if (keys & ArrowsAnd0To7_7) gScore.partial_scores[7]++;
    }

    if (been_in_loop_before) {
        DrawTanks(); // undo old tank
    } else {
        been_in_loop_before = gTRUE;
    }
    AdvanceTanks();
    DrawTanks();  // new position
    if (gScore.total_updated) DrawScores();
  }
}
