/*
 * main.c — Poisson spike train visualizer
 *
 * Loads an MNIST image and displays its Poisson-encoded spike activity
 * as a 28x28 pixel grid at 24 fps. Fired pixels flash white and decay
 * back to black over time, giving a phosphor-glow effect.
 *
 * Controls:
 *   RIGHT / SPACE  — next image
 *   LEFT           — previous image
 *   ESC            — quit
 *
 * Compile (raylib must be in the same directory):
 *   gcc main.c mnist.c poisson.c -L. -lraylib -lopengl32 -lgdi32 -lwinmm -o viz.exe
 */

#include "raylib.h"
#include "base.h"
#include "mnist.h"
#include "poisson.h"
#include <string.h>
#include <stdio.h>

#define CELL_SIZE 20
#define GRID_W 28
#define GRID_H 28
#define NUM_PIXELS (GRID_H * GRID_W)
#define WIN_W (GRID_W * CELL_SIZE)
#define WIN_H (GRID_H * CELL_SIZE)
#define STATUS_H 24

#define MAX_RATE 10.0f
#define DT 0.01f
#define DECAY 0.8f
#define TARGET_FPS 24

// int main(void) {
//     MNIST mnist;
//     if (!mnist_load(&mnist, "train-images.idx3-ubyte", "train-labels.idx1-ubyte")) {
//         fprintf(stderr, "Error: failed to load the MNIST data \n");
//         return 1;
//     }

//     poisson_seed(42);

//     InitWindow(WIN_W, WIN_H + STATUS_H, "Neocortex Spike Visualizer");
//     SetTargetFPS(TARGET_FPS);

//     f32 brightness[NUM_PIXELS];
//     memset(brightness, 0, sizeof(brightness));

//     u32 fired[NUM_PIXELS];
//     u32 image_idx = 0;

//     while (!WindowShouldClose()) {
//         if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_SPACE)) {
//             image_idx = (image_idx + 1) % mnist.count;
//             memset(brightness, 0, sizeof(brightness));
//         }
//         if (IsKeyPressed(KEY_LEFT)) {
//             image_idx = image_idx > 0 ? image_idx - 1: mnist.count - 1;
//             memset(brightness, 0, sizeof(brightness));
//         }

//         const f32 *intensities = mnist.images + (u64)image_idx * NUM_PIXELS;
//         u32 count = poisson_step(intensities, NUM_PIXELS, MAX_RATE, DT, fired);

//         for (u32 k = 0; k < count; k++) {
//             brightness[fired[k]] = 1.0f;
//         }
//         for (u32 i = 0; i < NUM_PIXELS; i++) {
//             brightness[i] *= DECAY;
//         }

//         BeginDrawing();
//         ClearBackground(BLACK);

//         for (u32 i = 0; i < NUM_PIXELS; i++) {
//             u32 row = i / GRID_W;
//             u32 col = i % GRID_W;
//             u8 v = (u8)(brightness[i] * 255.0f);
//             DrawRectangle((int)(col * CELL_SIZE), (int)(row * CELL_SIZE), CELL_SIZE, CELL_SIZE, (Color){v, v, v, 255});
//         }

//         char status[64];
//         snprintf(status, sizeof(status), "img %u   label: %u   spikes/step: %u",
//                  image_idx, mnist.labels[image_idx], count);
//         DrawRectangle(0, WIN_H, WIN_W, STATUS_H, (Color){ 20, 20, 20, 255 });
//         DrawText(status, 6, WIN_H + 5, 13, RAYWHITE);

//         EndDrawing();
//     }
//     CloseWindow();
//     mnist_release(&mnist);
//     return 0;
// }
