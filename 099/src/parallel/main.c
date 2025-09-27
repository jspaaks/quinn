#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "circuit/circuit.h"

int main (int argc, char * argv[]) {

    MPI_Init(&argc, &argv);

    int irank;
    int nranks;
    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    if (irank == 0) {
        fprintf(stdout, "parallel version:\n");
    }

    for (size_t i = irank; i < 0xffff; i+=nranks) {
        circuit_check(irank, i);
    }

    fprintf(stdout, "Process %d is done\n", irank);
    fflush(stdout);

    MPI_Finalize();
    return EXIT_SUCCESS;
}
