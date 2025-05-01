#include "nekotos/kernel/public.h"
#include "nekotos/lib/arcfour.h"
#include "nekotos/lib/format.h"
#include "nekotos/lib/keyscan_arrows_0to7.h"
#include "nekotos/lib/pmode1.h"

// There are only 4 colors.  How to use them?
#define BG 0
#define ME 2    // My ship
#define THEM 3  // Their ships
#define FG 1    // Borders, cows.

// Special Declarations
gSCREEN(Screen, 12);  // Screen for PMODE1

#define MAX_SHIPS 8

#define NPLAYERS gScore.number_of_players
#define PLAYER gScore.player

struct gamecast Packet;

struct ship {
  int x, y;        // x, y position: fixed binary point, 8b.8b ;  modulo 96, 96
  int xvel, yvel;  // x, y velocity: fixed binary point, 8b.8b ;  change in
                   // position per Decisecond.
};
#define DELTA_VEL 16  // Added by each arrow.

struct state {
  struct ship ship[MAX_SHIPS];
} ShipState gZEROED, OldShipState gZEROED;

struct ship* my;  // shortcut to my ship

#define NUM_COWS (MAX_SHIPS * 4)

struct cow {
  gbyte x, y;
} Cows[NUM_COWS];

struct ship_payload {
  gbyte type;
  struct ship ship;
};

struct kill_payload {
  gbyte type;
  gbyte cow_num;
};

enum PayloadType {
  PT_START,
  PT_STATE,
  PT_KILL,
  PT_OVER,
  PT_ABORT,
};

void InitCows() {
  for (gbyte i = 0; i < NUM_COWS; i++) {
    Cows[i].x = 4 + RandomUpTo(88);
    Cows[i].y = 4 + RandomUpTo(88);
    PMode1DrawSpotXor(Screen, Cows[i].x, Cows[i].y, FG);
  }
}

void CheckForKills(gbyte x, gbyte y) {
  for (gbyte i = 0; i < NUM_COWS; i++) {
    if (!Cows[i].x) continue;  // 0 for x means already killed.

    signed char dx = Cows[i].x - x;
    if (dx < 0) dx = (-dx);

    signed char dy = Cows[i].y - y;
    if (dy < 0) dy = (-dy);

    if (dx + dy > 1) continue;

    // Kill that cow.
    // Simplify: allow multiple kills on same cow if all are done
    // simultaneously, before any kill packets some back.
    struct kill_payload* pay = (struct kill_payload*)Packet.payload;

    Packet.flags = 0;
    pay->type = PT_KILL;  // Payload Type
    pay->cow_num = i;     // copy entire struct
    gSendCast(&Packet, sizeof *pay);

    // Claim a point.
    gScore.partial_scores[PLAYER]++;
    gScore.partial_dirty = gTRUE;

    // Use Xor to un-draw the cow.
    PMode1DrawSpotXor(Screen, Cows[i].x, Cows[i].y, FG);
  }
}

#define XWRAP (96 << 8)
#define YWRAP (96 << 8)
void AdvanceShips() {
  for (gbyte i = 0; i < NPLAYERS; i++) {
    struct ship* t = &ShipState.ship[i];

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
  int y = 2;
  for (gbyte i = 0; i < NPLAYERS; i++) {
    PMode1ClearDecimal3x5(Screen, PMode1DrawSpot, 100, y, /*color=*/BG);
    gbyte color = (i == PLAYER) ? ME : THEM;
    PMode1DrawDecimal3x5(Screen, PMode1DrawSpot, 100, y, /*color=*/color,
                         /*value=*/gScore.total_scores[i]);
    y += 7;
  }
}

void drawMod96Xor(signed char x, signed char y, gbyte color) {
  if (x < 0) x += 96;
  if (y < 0) x += 96;
  if (x >= 96) x -= 96;
  if (y >= 96) x -= 96;

  PMode1DrawSpotXor(Screen, (gbyte)x, (gbyte)y, color);
}

void XorOldShips() {
  for (gbyte i = 0; i < NPLAYERS; i++) {
    struct ship* p = &OldShipState.ship[i];
    signed char x = (signed char)(p->x >> 8);  // Shed fractional 8 bits.
    signed char y = (signed char)(p->y >> 8);  // Shed fractional 8 bits.

    gbyte color = (i == PLAYER) ? ME : THEM;
    drawMod96Xor(x, y, color);
    drawMod96Xor(x - 1, y, color);
    drawMod96Xor(x + 1, y, color);
    drawMod96Xor(x, y - 1, color);
    drawMod96Xor(x, y + 1, color);
  }
}

void ProcessPacket(struct gamecast* packet) {
  switch (packet->payload[0]) {
    case PT_STATE: {
      struct ship_payload* p = (struct ship_payload*)packet->payload;
      // Only copy the update if it is from another player.
      if (packet->sender != PLAYER) {
        memcpy(&ShipState.ship[packet->sender], &p->ship, sizeof p->ship);
      }
    } break;
    case PT_KILL: {
      struct kill_payload* p = (struct kill_payload*)packet->payload;
      Cows[p->cow_num].x = 0;  // 0 for x means dead.
    } break;
  }
}

void CheckIncomingPacket() {
  struct gamecast* packet = gReceiveCast64();
  if (packet) {
    ProcessPacket(packet);
    gFree64(packet);
  }
}

void SendCastOurShip() {
  // Once per second, send GameCast of ShipState.
  struct ship_payload* pay = (struct ship_payload*)Packet.payload;

  Packet.flags = 0;
  pay->type = PT_STATE;  // Payload Type
  pay->ship = *my;       // copy entire struct
  gSendCast(&Packet, sizeof *pay);
}

void ScanAndHandleKeys() {
  gword keys = ScanArrowsAnd0To7();
  if (keys) {
    if (keys & ArrowsAnd0To7_LEFT) {
      my->xvel -= DELTA_VEL;
    }
    if (keys & ArrowsAnd0To7_RIGHT) {
      my->xvel += DELTA_VEL;
    }
    if (keys & ArrowsAnd0To7_UP) {
      my->yvel -= DELTA_VEL;
    }
    if (keys & ArrowsAnd0To7_DOWN) {
      my->yvel += DELTA_VEL;
    }
  }
}

void InitializeScreen() {
  PMode1ClearScreen(Screen, /*color=*/BG);
  gPMode1Screen(Screen, /*colorset=*/0);

  PMode1DrawHorz(Screen, PMode1DrawSpot, /*x=*/0, /*y=*/0, /*color=*/FG,
                 /*len=*/96);
  PMode1DrawHorz(Screen, PMode1DrawSpot, /*x=*/0, /*y=*/1, /*color=*/FG,
                 /*len=*/96);
  PMode1DrawHorz(Screen, PMode1DrawSpot, /*x=*/0, /*y=*/94, /*color=*/FG,
                 /*len=*/96);
  PMode1DrawHorz(Screen, PMode1DrawSpot, /*x=*/0, /*y=*/95, /*color=*/FG,
                 /*len=*/96);

  PMode1DrawVirt(Screen, PMode1DrawSpot, /*x=*/0, /*y=*/0, /*color=*/FG,
                 /*len=*/96);
  PMode1DrawVirt(Screen, PMode1DrawSpot, /*x=*/1, /*y=*/0, /*color=*/FG,
                 /*len=*/96);
  PMode1DrawVirt(Screen, PMode1DrawSpot, /*x=*/94, /*y=*/0, /*color=*/FG,
                 /*len=*/96);
  PMode1DrawVirt(Screen, PMode1DrawSpot, /*x=*/95, /*y=*/0, /*color=*/FG,
                 /*len=*/96);
}

void setup() {
  my = &ShipState.ship[PLAYER];  // shortcut to my own ship
  InitializeScreen();

  // TODO: everyone in the shard needs the same seed,
  // but it should be different in each round of the game.
  RandomSeedString("TODO fix me later");
  InitCows();

  // Initial location of ships.
  for (gbyte i = 0; i < NPLAYERS; i++) {
    struct ship* t = &ShipState.ship[i];
    t->x = 20 << 8;
    t->y = (10 * i + 5) << 8;
  }

  OldShipState = ShipState;
  XorOldShips();  // draws ships for the first time
  DrawScores();
}

gbyte decis = 0;

void loop() {
  CheckIncomingPacket();

  if (gMono.decis != decis) {
    // Every 10th of a second.
    decis = gMono.decis;

    if (decis == 0) {
      // Once per second, send our location and velocity.
      SendCastOurShip();
    } else {
      // On other 9 decis per second, scan keyboard.
      ScanAndHandleKeys();
    }

    AdvanceShips();
    XorOldShips();  // undo old ship
    OldShipState = ShipState;
    XorOldShips();  // new position

    CheckForKills(my->x >> 8, my->y >> 8);

    if (gScore.total_updated) {
      gScore.total_updated = gFALSE;
      DrawScores();
    }
  }
}
