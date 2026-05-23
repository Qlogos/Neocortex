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
#include <stdint.h>

void poisson_seed(uint32_t seed);

uint32_t poisson_step(const float *intensities, uint32_t num_neurons,
                      float max_rate, float dt, uint32_t *out_fired);

#endif