#include <stdio.h>
#include <stdlib.h>
#include "circuit/circuit.h"

int main (int argc, char * argv[]) {
    if (argc == 1) {
        fprintf(stdout, "serial version:\n");
        for (size_t i = 0; i < 0xffff; i++) {
            circuit_check(0, i);
        }
        return EXIT_SUCCESS;
    }

    if (argc == 2) {
        uint16_t input16 = (uint16_t) atoi(argv[1]);
        if (input16 == 0) goto err;
        circuit_check(0, input16);
        return EXIT_SUCCESS;
    }

err:
    fprintf(stderr, "Expected either no arguments or 1 integer input argument, aborting\n");
    return EXIT_FAILURE;
}
