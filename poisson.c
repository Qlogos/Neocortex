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

b32 trial(f32 chance_istrue) {
    f32 rng = (float)rand();
    if (rng/((float)RAND_MAX) < chance_istrue) {
        return 1;
    } else{
        return 0;
    }
}

void poisson_spike_train(u8 *out, f32 max_rate, f32 intensity, f32 dt, u32 num_steps) {
    f32 per_step = dt * max_rate * intensity;
    for (u32 t = 0; t < num_steps; t++) {
         out[t] = trial(per_step);
    }
}

int main(void) {
    u32 seed = 42;
    srand(seed);
    u32 num_steps = 100;
    u8 *out = malloc(sizeof(u8) * num_steps);
    poisson_spike_train(out, 100, 0.5, 0.001, num_steps);
    printf("[%d", out[0]);
    for (u32 i = 1; i < num_steps - 1; i++) {
        printf(", %d", out[i]);
    }
    printf(", %d]", out[99]);
    free(out);
}
