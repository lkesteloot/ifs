
#include <stdlib.h>
#include "util.h"

// Thread-local state for our random number generator.
thread_local unsigned short g_xsubi[3];

void init_rand(int seed) {
    g_xsubi[0] = seed;
    g_xsubi[1] = seed;
    g_xsubi[2] = seed;
}

double my_randd() {
    return erand48(g_xsubi);
}

long my_randl() {
    return nrand48(g_xsubi);
}
