#ifndef UTIL_H
#define UTIL_H

#include <cstdint>

// Gamma-encoded color component.
typedef uint8_t gamma_color;

// Linear-encoded color component.
typedef uint16_t linear_color;

void init_rand(int seed);

// [0, 1)
double my_randd();

// [0, 2**31)
long my_randl();

#endif // UTIL_H
