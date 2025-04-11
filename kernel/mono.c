#include "kernel/private.h"

struct mono gMono;

#define M gMono

static void IncrementSeconds() {
  SpinMonoSeconds();

  ++M.seconds;

  if (gKern.in_game) SendPartialScores();

  // Now go increment the Wall Time.
  Wall_IncrementSecond();
}

static void IncrementDecis() {
  // SpinMonoDecis();

  if (M.decis < 9) {
    ++M.decis;
  } else {
    M.decis = 0;
    IncrementSeconds();
  }
}

void Mono_IncrementTicks() {
  if (M.ticks < 5) {
    ++M.ticks;
  } else {
    M.ticks = 0;
    IncrementDecis();
  }
}

#undef M
