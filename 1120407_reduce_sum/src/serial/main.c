#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


int main (int argc, char * argv[]) {
    if (argc != 2) goto err;

    uint64_t p = (uint64_t) atoi(argv[1]);
    if (p == 0) goto err;

    uint64_t actual = 0;
    for (uint64_t i = 0; i < p; i++) {
        actual += i + 1;
    }

    uint64_t expected = p * (p + 1) / 2;
    if (actual != expected) {
        fprintf(stdout, "Calculated value does not match expected value\n");
        return EXIT_FAILURE;
    } else {
        fprintf(stdout, "The sum was accurately calculated as %" PRIu64 "\n", actual);
    }

    return EXIT_SUCCESS;

err:
    fprintf(stdout, "Expected exactly one integer command line argument, aborting.\n");
    return EXIT_FAILURE;
}
