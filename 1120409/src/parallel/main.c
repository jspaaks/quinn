#include <mpi.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include "primes/primes.h"

#define VERBOSE

int main (int argc, char * argv[]) {
    if (argc != 1) {
        fprintf(stderr,
                "Usage: %s\n"
                "\n"
                "    Determine the maximum gap between adjacent primes in the\n"
                "    interval [2..N]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int irank;
    int nranks;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    const uint32_t ncols = 4;
    const uint32_t imax = 23;
    const uint32_t blocksz = nranks * ncols;
    const uint32_t nblocks = (imax + blocksz - 1) / blocksz;
    if (imax < 5) {
        fprintf(stdout, "imax has to be at least 5, aborting\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

#ifdef VERBOSE
    if (irank == 0) {
        fprintf(stdout, "imax   : %" PRIu32 "\n", imax);
        fprintf(stdout, "ncols  : %" PRIu32 "\n", ncols);
        fprintf(stdout, "nranks : %d\n", nranks);
        fprintf(stdout, "blocksz: %" PRIu32 "\n", blocksz);
        fprintf(stdout, "nblocks: %" PRIu32 "\n", nblocks);
    }
#endif

    uint32_t maxgap_lcl = 0;
    uint32_t maxgap_gbl = 0;

    for (uint32_t iblock = 0; iblock < nblocks; iblock++) {
        uint32_t nprimes = 0;
        uint32_t ithis = 5 + iblock * blocksz + irank * ncols;
        if (ithis > imax) break;

        uint32_t n = MIN(5 + iblock * blocksz + (irank + 1) * ncols, imax + 1);

        // find first prime preceding i
        uint32_t iprev = ithis - 2;
        while (iprev >= 3 && !isprime(iprev)) {
            iprev -= 2;
        }
#ifdef VERBOSE
        fprintf(stdout, "(%2d)  [%2" PRIu32 ",%2" PRIu32 ") -- first preceding prime: %" PRIu32 "\n", irank, ithis, n, iprev);
#endif

        // find adjacent primes
        while (ithis < n) {
            if (isprime(ithis)) {
                maxgap_lcl = MAX(ithis - iprev, maxgap_lcl);
                iprev = ithis;
                nprimes++;
            }
            ithis += 2;
        }
#ifdef VERBOSE
        fprintf(stdout, "(%2d)          -- nprimes: %" PRIu32 ", maxgap_lcl: %" PRIu32 "\n", irank, nprimes, maxgap_lcl);
        fflush(stdout);
#endif
    }

    MPI_Reduce(&maxgap_lcl, &maxgap_gbl, 1, MPI_UINT32_T, MPI_MAX, 0, MPI_COMM_WORLD);
    if (irank == 0) {
        fprintf(stdout, "The maximum gap between two adjancent primes in interval [2..%" PRIu32 "] is: %" PRIu32 "\n", imax, maxgap_gbl);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
