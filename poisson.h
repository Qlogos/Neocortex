/*
 * poisson.h — Poisson spike train encoder
 *
 * Converts normalized input intensities (0.0–1.0) into binary spike trains
 * using a homogeneous Poisson process. Each input maps to a neuron whose
 * per-step firing probability is proportional to its intensity.
 *
 * For neuron i with intensity I[i] in [0,1]:
 *   p_spike(t) = I[i] * max_rate * dt
 *
 * A spike occurs at step t if a uniform random sample falls below p_spike.
 */

#ifndef POISSON_H
#define POISSON_H

#include "base.h"

typedef struct {
    u8  *spikes;       /* spikes[t * num_neurons + i], value 0 or 1 */
    u32  num_neurons;
    u32  num_steps;
    f32  max_rate;     /* Hz, maximum firing rate for intensity = 1.0 */
    f32  dt;           /* seconds, simulation time step */
} PoissonEncoder;

/*
 * Generate spike trains for all neurons over num_steps time steps.
 * Returns true on success. Seed controls reproducibility (0 = time-based).
 */
b32 poisson_encode(PoissonEncoder *enc, const f32 *intensities, u32 num_neurons,
                   u32 num_steps, f32 max_rate, f32 dt, u32 seed);

/* Free allocated memory and zero the struct. */
void poisson_release(PoissonEncoder *enc);

/* Query: did neuron i fire at time step t? */
b32 poisson_spike(const PoissonEncoder *enc, u32 t, u32 i);

/* Get total spike count for neuron i across all time steps. */
u32 poisson_count(const PoissonEncoder *enc, u32 i);

#endif /* POISSON_H */
