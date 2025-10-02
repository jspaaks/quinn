#include <inttypes.h>
#include <mpi.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "identifiers/identifiers.h"

int main (int argc, char * argv[]) {
    int irank;
    int nranks;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    const size_t n = 1000000;
    const size_t ndigits = 6;
    char identifier[7] = {};
    uint32_t count_lcl = 0;
    for (size_t i = irank; i < n; i += nranks) {
        sprintf(&identifier[0], "%06zu", i);
        bool b = isvalid(&identifier[0], ndigits);
        if (b) {
            count_lcl++;
        }
    }
    uint32_t count_gbl = 0;
    MPI_Reduce(&count_lcl, &count_gbl, 1, MPI_UINT32_T, MPI_SUM, 0, MPI_COMM_WORLD);
    if (irank == 0) {
        fprintf(stdout, "Found %" PRIu32 " valid identifiers\n", count_gbl);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
