/*
 * mnist.h — MNIST IDX binary format loader
 *
 * Loads handwritten digit images and labels from the standard MNIST IDX
 * files into a normalized float array (0.0–1.0) for use in SNNs.
 */

#ifndef MNIST_H
#define MNIST_H

#include "base.h"

/* ── Data ────────────────────────────────────────────────────── */

typedef struct {
    u8  *labels;   /* 1 byte per image, values 0–9 */
    f32 *images;   /* row-major, normalized 0.0–1.0 */
    u32  count;    /* number of images */
    u32  rows;     /* image height (28) */
    u32  cols;     /* image width  (28) */
    u32  size;     /* pixels per image (rows * cols) */
} MNIST;

/* ── API ─────────────────────────────────────────────────────── */

/* Load images and labels. Returns true on success. */
b32 mnist_load(MNIST *mnist, const char *images_path, const char *labels_path);

/* Free all memory and zero the struct. */
void mnist_release(MNIST *mnist);

/* Access pixel (r, c) of image i:  mnist.images[i * mnist.size + r * mnist.cols + c] */
/* Access label of image i:         mnist.labels[i] */

#endif /* MNIST_H */
