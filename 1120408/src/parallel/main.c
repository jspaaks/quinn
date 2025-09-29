#include <inttypes.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include "primes/primes.h"

typedef struct pair Pair;

struct pair {
    size_t v;
    bool isprime;
};

bool iseven(size_t n) {
    return n % 2 == 0;
}

int main (int argc, char * argv[]) {

    int irank;
    int nranks;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    uint32_t tallylcl = 0;
    uint32_t tallygbl = 0;

    const uint32_t imaxgbl = atoi(argv[1]);
    const uint32_t d = imaxgbl + 1 - 3;
    const uint32_t nodds = (d + 1) / 2;
    const uint32_t w = (nodds + nranks - 1) / nranks;
    const uint32_t iminlcl = 3 + irank * w * 2;
    const uint32_t imaxlcl = MIN(3 + (irank + 1) * w * 2, imaxgbl + 1);

    fprintf(stdout, "(%d) range [%" PRIu32 ",%" PRIu32 ")\n", irank, iminlcl, imaxlcl);

    MPI_Barrier(MPI_COMM_WORLD);

    Pair prev = {
        .v = iminlcl,
        .isprime = isprime(iminlcl)
    };

    Pair this = {};

    for (size_t i = iminlcl + 2; i < imaxlcl + 2; i += 2) {
        this = (Pair){
            .v = i,
            .isprime = isprime(i)
        };
        if (prev.isprime && this.isprime) {
            tallylcl++;
        }
        prev = this;
    }
    fprintf(stdout, "(%d) found %" PRIu32 " occurrences of adjacent primes in range [%" PRIu32 ",%" PRIu32 ")\n", irank, tallylcl, iminlcl, imaxlcl);

    MPI_Reduce(&tallylcl, &tallygbl, 1, MPI_UINT32_T, MPI_SUM, 0, MPI_COMM_WORLD);
    if (irank == 0) {
        fprintf(stdout, "(%d) there are %" PRIu32 " adjacent primes in range [2,%" PRIu32 "]\n", irank, tallygbl + 1, imaxgbl);
    }
    MPI_Finalize();
    return EXIT_SUCCESS;
}
