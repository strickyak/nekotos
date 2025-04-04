#include "kernel/private.h"

// config.c


struct config gConfig = {
    .ram_limit = RAM_LIMIT,
#if NET_TYPE_cocoio
    .net_type = "cocoio",
#endif
#if NET_TYPE_bonobo
    .net_type = "bonobo",
#endif
};
