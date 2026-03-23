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
                "    Determine the maximum gap between adjacent primes in the interval [2..ZMAX].\n", argv[0]);
        return EXIT_FAILURE;
    }
    const size_t imax = atoll(argv[1]);
    size_t maxgap = 0;
    Pair prev = {
        .v = 2,
        .isprime = true
    };
    Pair this = {};

    for (size_t i = 3; i < imax; i += 2) {
        this = (Pair){
            .v = i,
            .isprime = isprime(i)
        };
        if (!this.isprime) continue;
        size_t gap = this.v - prev.v;
        if (gap > maxgap) {
            maxgap = gap;
        }
        prev = this;
    }
    fprintf(stdout, "Max gap between adjacent primes in range [2..%zu] was %zu.\n", imax, maxgap);
    return EXIT_SUCCESS;
}
