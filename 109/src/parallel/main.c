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

    // block all processes until they're all ready, then measure duration
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);
    double t0 = MPI_Wtime();
    for (size_t i = irank; i < 0xffff; i += nranks) {
        nsolutions_local += circuit_check(irank, i) ? 1 : 0;
    }
    MPI_Reduce(&nsolutions_local, &nsolutions_global, 1, MPI_UINT16_T,
               MPI_SUM, 0, MPI_COMM_WORLD);
    double t1 = MPI_Wtime();

    MPI_Finalize();

    fprintf(stdout, "Process %d is done, identified %u solutions\n",
            irank, nsolutions_local);
    if (irank == 0) {
        fprintf(stdout, "There are %u solutions in total\n", nsolutions_global);
        fprintf(stdout, "Walltime: %lf microseconds\n", (t1 - t0) * 1000 * 1000 );
    }
    return EXIT_SUCCESS;
}
