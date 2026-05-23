/*
 * poisson.c — Poisson spike train encoder
 *
 * Generates binary spike trains from normalized intensities via a
 * homogeneous Poisson process. For each time step and each neuron,
 * a spike is emitted with probability p = I * max_rate * dt.
 *
 * Uses a simple LCG PRNG for speed and reproducibility.
 * Not cryptographically secure — fine for SNN simulation.
 */

#include "base.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

static u32 rng_state = 0x12345678;

void poisson_seed(u32 seed) {
    rng_state = seed ? seed : 0x12345678;
}

static inline u32 xorshift32(void) {
    u32 x = rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng_state = x;
    return x;
}

static inline float frand(void) {
    return (float)xorshift32() / (float)UINT32_MAX;
}

b32 trial(f32 chance_istrue) {
    f32 rng = (float)rand();
    if (rng/((float)RAND_MAX) < chance_istrue) {
        return 1;
    } else{
        return 0;
    }
}

u32 poisson_step(const f32 *intensities, u32 num_neurons, f32 max_rate, f32 dt, u32 *out_fired) {
    u32 count = 0;
    for (u32 i = 0; i < num_neurons; i++) {
        f32 p = intensities[i] * max_rate * dt;
        if (frand() < p) {
            out_fired[count++] = i;
        }
    }
    return count;
}

int main(void) {
    u32 seed = 42;
    srand(seed);
    u32 num_steps = 100;
    u8 *out = malloc(sizeof(u8) * num_steps);
    poisson_spike_train(out, 10, 0.5, 0.001, num_steps);
    printf("[%d", out[0]);
    for (u32 i = 1; i < num_steps - 1; i++) {
        printf(", %d", out[i]);
    }
    printf(", %d]", out[99]);
    free(out);
}
