#include "base.h"
#include "mnist.h"

static const float DT = 0.5f;
static const float TAU_V = 100.0f;

int main(void) {
    MNIST mnist;

    if (!mnist_load(&mnist, "train-images.idx3-ubyte", "train-labels.idx1-ubyte")) {
        fprintf(stderr, "Error: failed to load MNIST data\n");
        return 1;
    }

    printf("Loaded %u images (%ux%u), %u labels\n",
           mnist.count, mnist.rows, mnist.cols, mnist.count);

    printf("First image, first 10 pixels: ");
    for (u32 i = 0; i < 10; i++) {
        printf("%.3f ", mnist.images[i]);
    }
    printf("\n");

    printf("First 10 labels: ");
    for (u32 i = 0; i < 10; i++) {
        printf("%u ", mnist.labels[i]);
    }
    printf("\n");

    mnist_release(&mnist);
    return 0;
}
