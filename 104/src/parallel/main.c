#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "circuit/circuit.h"

int main (int argc, char * argv[]) {

    uint16_t nsolutions_global = 0;
    uint16_t nsolutions_local = 0;
    int irank;
    int nranks;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    if (irank == 0) {
        fprintf(stdout, "parallel version:\n");
    }

    for (size_t i = irank; i < 0xffff; i += nranks) {
        nsolutions_local += circuit_check(irank, i) ? 1 : 0;
    }

    MPI_Reduce(&nsolutions_local, &nsolutions_global, 1, MPI_UINT16_T,
               MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Finalize();

    fprintf(stdout, "Process %d is done, identified %u solutions\n", irank, nsolutions_local);
    fflush(stdout);

    if (irank == 0) {
        fprintf(stdout, "There are %u solutions in total\n", nsolutions_global);
    }
    return EXIT_SUCCESS;
}
