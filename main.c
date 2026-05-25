#include "raylib.h"
#include "base.h"
#include "mnist.h"
#include "poisson.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define N_INPUT 784
#define N_HIDDEN 784

typedef struct {
    f32 *weights;
    f32 *state;
    i32 *last_in;
    i32 *last_hid;
    u32  tick;
} Network;

#define A_PLUS       0.01f
#define A_MINUS      0.01f
#define TAU_PLUS     10.0f
#define TAU_MINUS    10.0f
#define V_THRESHOLD  1.0f
#define V_LEAK       0.9f
#define NEVER        ((i32)-1000000)

Network *network_create(void) {
    Network *net = malloc(sizeof(Network));

    net->weights  = malloc(N_INPUT * N_HIDDEN * sizeof(f32));
    net->state    = malloc(N_HIDDEN * sizeof(f32));
    net->last_in  = malloc(N_INPUT  * sizeof(i32));
    net->last_hid = malloc(N_HIDDEN * sizeof(i32));
    net->tick     = 0;

    memset(net->state, 0, N_HIDDEN * sizeof(f32));
    for (u32 i = 0; i < N_INPUT;  i++) net->last_in[i]  = NEVER;
    for (u32 h = 0; h < N_HIDDEN; h++) net->last_hid[h] = NEVER;
    for (u32 i = 0; i < N_INPUT * N_HIDDEN; i++) {
        net->weights[i] = (f32)rand() / (f32)RAND_MAX;
    }

    return net;
}

void network_destroy(Network *net) {
    free(net->weights);
    free(net->state);
    free(net->last_in);
    free(net->last_hid);
    free(net);
}

void network_step(Network *net, const u32 *fired_in, u32 in_count) {
    i32 T = (i32)net->tick;

    for (u32 h = 0; h < N_HIDDEN; h++) net->state[h] *= V_LEAK;

    for (u32 k = 0; k < in_count; k++) {
        u32  j   = fired_in[k];
        f32 *row = net->weights + j * N_HIDDEN;

        for (u32 h = 0; h < N_HIDDEN; h++) {
            i32 dt = T - net->last_hid[h];
            row[h] -= A_MINUS * expf(-(f32)dt / TAU_MINUS);
            if (row[h] < 0.0f) row[h] = 0.0f;
        }

        for (u32 h = 0; h < N_HIDDEN; h++) net->state[h] += row[h];

        net->last_in[j] = T;
    }

    for (u32 h = 0; h < N_HIDDEN; h++) {
        if (net->state[h] < V_THRESHOLD) continue;

        net->state[h] = 0.0f;

        for (u32 j = 0; j < N_INPUT; j++) {
            i32 dt = T - net->last_in[j];
            f32 *w = &net->weights[j * N_HIDDEN + h];
            *w += A_PLUS * expf(-(f32)dt / TAU_PLUS);
            if (*w > 1.0f) *w = 1.0f;
        }

        net->last_hid[h] = T;
    }

    net->tick++;
}

#define CELL_SIZE  20
#define GRID_W     28
#define GRID_H     28
#define WIN_W      (GRID_W * CELL_SIZE)
#define WIN_H      (GRID_H * CELL_SIZE)
#define SEP_W      4
#define LABEL_H    24 
#define STATUS_H   24 

#define HIDDEN_X   (WIN_W + SEP_W)
#define GRID_Y     LABEL_H
#define STATUS_Y   (LABEL_H + WIN_H)
#define TOTAL_W    (WIN_W * 2 + SEP_W)
#define TOTAL_H    (LABEL_H + WIN_H + STATUS_H)

#define MAX_RATE   10.0f
#define DT         0.01f
#define DECAY      0.8f
#define FPS        24

int main(void) {
    srand(42);
    poisson_seed(42);

    MNIST mnist;
    if (!mnist_load(&mnist, "train-images.idx3-ubyte", "train-labels.idx1-ubyte")) {
        fprintf(stderr, "failed to load MNIST\n");
        return 1;
    }

    Network *net = network_create();
    printf("Network ready. Weights: %d  State: %d\n", N_INPUT * N_HIDDEN, N_HIDDEN);

    InitWindow(TOTAL_W, TOTAL_H, "Neocortex");
    SetTargetFPS(FPS);

    f32 brightness[N_INPUT];
    memset(brightness, 0, sizeof(brightness));
    u32 fired[N_INPUT];
    u32 image_idx = 0;

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_SPACE)) {
            image_idx = (image_idx + 1) % mnist.count;
            memset(brightness, 0, sizeof(brightness));
        }
        if (IsKeyPressed(KEY_LEFT)) {
            image_idx = image_idx > 0 ? image_idx - 1 : mnist.count - 1;
            memset(brightness, 0, sizeof(brightness));
        }

        const f32 *intensities = mnist.images + (u64)image_idx * N_INPUT;
        u32 count = poisson_step(intensities, N_INPUT, MAX_RATE, DT, fired);

        for (u32 k = 0; k < count; k++) brightness[fired[k]] = 1.0f;
        for (u32 i = 0; i < N_INPUT; i++) brightness[i] *= DECAY;

        // -------------- STDP Rule Logic -------------- //

        network_step(net, fired, count);

        // -------------- Render -------------- //

        BeginDrawing();
        ClearBackground(BLACK);

        DrawRectangle(0,        0, WIN_W, LABEL_H, (Color){ 40, 40, 40, 255 });
        DrawRectangle(HIDDEN_X, 0, WIN_W, LABEL_H, (Color){ 40, 40, 40, 255 });
        DrawText("Input Layer",  6,          5, 13, RAYWHITE);
        DrawText("Hidden Layer", HIDDEN_X+6, 5, 13, RAYWHITE);

        DrawRectangle(WIN_W, 0, SEP_W, LABEL_H + WIN_H, (Color){ 10, 10, 10, 255 });

        for (u32 i = 0; i < N_INPUT; i++) {
            u32 row = i / GRID_W;
            u32 col = i % GRID_W;
            u8  v   = (u8)(brightness[i] * 255.0f);
            DrawRectangle(col * CELL_SIZE, GRID_Y + row * CELL_SIZE,
                          CELL_SIZE, CELL_SIZE, (Color){ v, v, v, 255 });
        }

        for (u32 h = 0; h < N_HIDDEN; h++) {
            u32 row = h / GRID_W;
            u32 col = h % GRID_W;
            f32 s   = net->state[h];
            if (s < 0.0f) s = 0.0f;
            if (s > 1.0f) s = 1.0f;
            u8 v = (u8)(s * 255.0f);
            DrawRectangle(HIDDEN_X + col * CELL_SIZE, GRID_Y + row * CELL_SIZE,
                          CELL_SIZE, CELL_SIZE, (Color){ v, v, v, 255 });
        }

        char status[64];
        snprintf(status, sizeof(status), "img %u   label: %u   spikes: %u",
                 image_idx, mnist.labels[image_idx], count);
        DrawRectangle(0, STATUS_Y, TOTAL_W, STATUS_H, (Color){ 20, 20, 20, 255 });
        DrawText(status, 6, STATUS_Y + 5, 13, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    network_destroy(net);
    mnist_release(&mnist);
    return 0;

}