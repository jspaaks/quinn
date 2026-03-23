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

int main (int argc, char * argv[]) {

    int irank;
    int nranks;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    uint32_t tallylcl = 0;
    uint32_t tallygbl = 0;
    const uint32_t imax = atoi(argv[1]);

    Pair this = {};
    Pair next = {};

    for (size_t i = 3 + irank * 2; i <= imax; i += nranks * 2) {
        this = (Pair){
            .v = i,
            .isprime = isprime(i)
        };
        next = (Pair){
            .v = i + 2,
            .isprime = isprime(i + 2)
        };
        if (this.isprime && next.isprime) {
            tallylcl++;
        }
    }

    fprintf(stdout, "(%d) found %" PRIu32 " occurrences of adjacent primes\n", irank, tallylcl);

    MPI_Reduce(&tallylcl, &tallygbl, 1, MPI_UINT32_T, MPI_SUM, 0, MPI_COMM_WORLD);
    if (irank == 0) {
        fprintf(stdout, "(%d) there are %" PRIu32 " adjacent primes in range [2,%" PRIu32 "]\n", irank, tallygbl + 1, imax);
    }
    MPI_Finalize();
    return EXIT_SUCCESS;
}
