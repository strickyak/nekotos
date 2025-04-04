#ifndef _KERNEL_PRIVATE_H_
#define _KERNEL_PRIVATE_H_

// For the game, gCONST means const.
// For the kernel, gCONST means mutable.
#define gCONST

#include "kernel/public.h"
#include "kernel/prelude.h"
#include "kernel/spin.h"
#include "kernel/wall.h"
#include "kernel/real.h"
#include "kernel/breakkey.h"
#include "kernel/kern.h"
#include "kernel/vdg.h"
#include "kernel/bonobo.h"
#include "kernel/wiznet.h"
#include "kernel/network.h"
#include "kernel/kern.h"
#include "kernel/console.h"
#include "kernel/keyboard.h"
#include "kernel/irq.h"
#include "kernel/alloc64.h"
#include "kernel/score.h"

#endif // _KERNEL_PRIVATE_H_
