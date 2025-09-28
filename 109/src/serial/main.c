#include <stdio.h>
#include <stdlib.h>
#include "circuit/circuit.h"

int main (int argc, char * argv[]) {
    if (argc == 1) {
        fprintf(stdout, "serial version:\n");
        uint16_t sum = 0;
        for (size_t i = 0; i < 0xffff; i++) {
            sum += circuit_check(0, i) ? 1 : 0;
        }
        fprintf(stdout, "Found %u ways to satisfy the circuit\n", sum);
        return EXIT_SUCCESS;
    }

    if (argc == 2) {
        uint16_t input16 = (uint16_t) atoi(argv[1]);
        if (input16 == 0) goto err;
        fprintf(stdout,
                "Circuit was%ssatisfied for input %u\n",
                circuit_check(0, input16) ? " " : " not ",
                input16);
        return EXIT_SUCCESS;
    }

err:
    fprintf(stderr, "Expected either no arguments or 1 integer input argument, aborting\n");
    return EXIT_FAILURE;
}
