#include "random.h"
#include <stdlib.h>
#include <time.h>

void random_init() {
    srand(time(NULL));
}

// Returns a random integer in the range [0, max)
// max must be greater than 0
int random_int(int max) {
    return rand() % max;
}

int random_int_range(int min, int max) {
    return rand() % (max - min + 1) + min;
}
