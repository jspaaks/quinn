#include <stdio.h>
#include <stdlib.h>
#include "primes/primes.h"

typedef struct pair Pair;

struct pair {
    size_t v;
    bool isprime;
};

int main (int argc, char * argv[]) {
    if (argc != 2) {
        fprintf(stderr,
                "Usage: %s ZMAX\n"
                "\n"
                "    Count the number of adjacent primes in the interval [2..ZMAX].\n", argv[0]);
        return EXIT_FAILURE;
    }
    size_t tally = 0;

    Pair prev = {
        .v = 2,
        .isprime = true
    };

    Pair this = {};

    const size_t imax = atoll(argv[1]);

    for (size_t i = 3; i < imax; i += 2) {
        this = (Pair){
            .v = i,
            .isprime = isprime(i)
        };
        if (prev.isprime && this.isprime) {
            // fprintf(stdout, "%zu and %zu are adjacent primes\n", prev.v, this.v);
            tally++;
        }
        prev = this;
    }
    fprintf(stdout, "Found %zu occurrences of adjacent primes in range [2..%zu].\n", tally, imax);
    return EXIT_SUCCESS;
}
