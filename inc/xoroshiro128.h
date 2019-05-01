#ifndef XOROSHIRO128_H
#define XOROSHIRO128_H

#include <stdint.h>

void xor128plusinit(int t);
uint64_t nextsplitmix64();
void jumpxor128s(void);
uint64_t nextxor128s(void);
uint64_t rotl(const uint64_t x, int k);


int rnd(int min, int max);
void rnd_init(int rnd_seed);



#endif
