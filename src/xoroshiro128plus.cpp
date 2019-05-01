/*  Written in 2016 by David Blackman and Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

#include <stdint.h>

/* This is the successor to xorshift128+. It is the fastest full-period
   generator passing BigCrush without systematic failures, but due to the
   relatively short period it is acceptable only for applications with a
   mild amount of parallelism; otherwise, use a xorshift1024* generator.

   Beside passing BigCrush, this generator passes the PractRand test suite
   up to (and included) 16TB, with the exception of binary rank tests,
   which fail due to the lowest bit being an LFSR; all other bits pass all
   tests. We suggest to use a sign test to extract a random Boolean value.

   Note that the generator uses a simulated rotate operation, which most C
   compilers will turn into a single instruction. In Java, you can use
   Long.rotateLeft(). In languages that do not make low-level rotation
   instructions accessible xorshift128+ could be faster.

   The state must be seeded so that it is not everywhere zero. If you have
   a 64-bit seed, we suggest to seed a splitmix64 generator and use its
   output to fill s. */

#include "xoroshiro128.h"

uint64_t xor128s[2];



inline uint64_t rotl(const uint64_t x, int k)
{
	return (x << k) | (x >> (64 - k));
}

inline  uint64_t nextxor128s(void)
{
	const uint64_t s0 = xor128s[0];
	uint64_t s1 = xor128s[1];
	const uint64_t result = s0 + s1;

	s1 ^= s0;
	xor128s[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14); // a, b
	xor128s[1] = rotl(s1, 36); // c

	return result;
}


/* This is the jump function for the generator. It is equivalent
   to 2^64 calls to next(); it can be used to generate 2^64
   non-overlapping subsequences for parallel computations. */

void jumpxor128s(void)
{
	static const uint64_t JUMP[] = { 0xbeac0467eba5facb, 0xd86b048b86aa9922 };

	uint64_t s0 = 0;
	uint64_t s1 = 0;
	for(unsigned int i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
		for(int b = 0; b < 64; b++) {
			if (JUMP[i] & 1ULL << b) {
				s0 ^= xor128s[0];
				s1 ^= xor128s[1];
			}
			nextxor128s();
		}

	xor128s[0] = s0;
	xor128s[1] = s1;
}

/*  Written in 2015 by Sebastiano Vigna (vigna@acm.org)

splitmix64

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */


/* This is a fixed-increment version of Java 8's SplittableRandom generator
   See http://dx.doi.org/10.1145/2714064.2660195 and
   http://docs.oracle.com/javase/8/docs/api/java/util/SplittableRandom.html

   It is a very fast generator passing BigCrush, and it can be useful if
   for some reason you absolutely want 64 bits of state; otherwise, we
   rather suggest to use a xoroshiro128+ (for moderately parallel
   computations) or xorshift1024* (for massively parallel computations)
   generator. */

uint64_t x; /* The state can be seeded with any value. */
uint64_t nextsplitmix64()
{
	uint64_t z = (x += UINT64_C(0x9E3779B97F4A7C15));
	z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
	z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
	return z ^ (z >> 31);
}

void xor128plusinit(int t)
{
    x = t;
    xor128s[0] = nextsplitmix64();
    xor128s[1] = nextsplitmix64();
}
void rnd_init(int rnd_seed)
{
    xor128plusinit(rnd_seed);
    //rand_gen.seed(RAND_SEED);
}

int rnd(int min, int max)
{
    //http://stackoverflow.com/questions/16800963/generate-random-int-with-specified-upper-bound-0-n-without-using-or
    // this function seems to be much faster than the stuff in the std lib
    // profiling with debug version in 3000000 simulation runs:
    // merseyne twister took 43% of time
    // minstd_rand (linear congruent generator) 40%
    // xoroshift128plus with this loop 30%
    int limit = max - min+1;
    int m = limit - 1;
    int v = 0;
    m |= m >> 1;
    m |= m >> 2;
    m |= m >> 4;
    m |= m >> 8;
    m |= m >> 16; // m is smallest ((power of 2) - 1) > limit (fill with 1s starting with the most significant 1)

    do
    {
            v = m & nextxor128s();  // random number
    }
    while (v >= limit);

    return min + v;


    /*uniform_int_distribution<uint32_t> rand_dis( min,  max);
    return rand_dis(rand_gen);*/
}
