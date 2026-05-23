/*
 * mnist.c — MNIST IDX binary format loader
 *
 * Parses the IDX format: 4-byte magic, big-endian u32 fields,
 * followed by raw pixel/label bytes. Validates magic numbers
 * and dimensions, normalizes pixels to [0.0, 1.0].
 *
 * Files:
 *   train-images.idx3-ubyte  — 60000 images, 28×28, 16-byte header
 *   train-labels.idx1-ubyte  — 60000 labels, 0–9, 8-byte header
 *
 * Download: https://www.kaggle.com/datasets/hojjatk/mnist-dataset
 */

#include "mnist.h"

/* ── Internal helpers ────────────────────────────────────────── */

static u32 read_be_u32(FILE *f) {
    u8 buf[4];
    if (fread(buf, 1, 4, f) != 4) return 0;
    return ((u32)buf[0] << 24) | ((u32)buf[1] << 16) | ((u32)buf[2] << 8) | (u32)buf[3];
}

/* ── Implementation ──────────────────────────────────────────── */

b32 mnist_load(MNIST *mnist, const char *images_path, const char *labels_path) {
    memset(mnist, 0, sizeof(*mnist));

    FILE *img_f = fopen(images_path, "rb");
    if (!img_f) return false;

    if (read_be_u32(img_f) != 0x00000803) { fclose(img_f); return false; }

    mnist->count = read_be_u32(img_f);
    mnist->rows  = read_be_u32(img_f);
    mnist->cols  = read_be_u32(img_f);
    mnist->size  = mnist->rows * mnist->cols;

    if (!mnist->count || !mnist->rows || !mnist->cols) { fclose(img_f); return false; }

    FILE *lbl_f = fopen(labels_path, "rb");
    if (!lbl_f) { fclose(img_f); return false; }

    if (read_be_u32(lbl_f) != 0x00000801) { fclose(img_f); fclose(lbl_f); return false; }

    u32 lbl_count = read_be_u32(lbl_f);
    if (lbl_count != mnist->count) { fclose(img_f); fclose(lbl_f); return false; }

    mnist->images = malloc(mnist->count * mnist->size * sizeof(f32));
    mnist->labels = malloc(mnist->count * sizeof(u8));

    if (!mnist->images || !mnist->labels) {
        free(mnist->images); free(mnist->labels);
        memset(mnist, 0, sizeof(*mnist));
        fclose(img_f); fclose(lbl_f);
        return false;
    }

    u8 *raw = malloc(mnist->count * mnist->size * sizeof(u8));
    if (!raw) {
        free(mnist->images); free(mnist->labels);
        memset(mnist, 0, sizeof(*mnist));
        fclose(img_f); fclose(lbl_f);
        return false;
    }

    if (fread(raw, 1, mnist->count * mnist->size, img_f) != mnist->count * mnist->size) {
        free(raw); free(mnist->images); free(mnist->labels);
        memset(mnist, 0, sizeof(*mnist));
        fclose(img_f); fclose(lbl_f);
        return false;
    }

    if (fread(mnist->labels, 1, mnist->count, lbl_f) != mnist->count) {
        free(raw); free(mnist->images); free(mnist->labels);
        memset(mnist, 0, sizeof(*mnist));
        fclose(img_f); fclose(lbl_f);
        return false;
    }

    fclose(img_f);
    fclose(lbl_f);

    for (u32 i = 0; i < mnist->count * mnist->size; i++) {
        mnist->images[i] = (f32)raw[i] / 255.0f;
    }

    free(raw);
    return true;
}

void mnist_release(MNIST *mnist) {
    free(mnist->images);
    free(mnist->labels);
    memset(mnist, 0, sizeof(*mnist));
}
