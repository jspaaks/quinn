#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main (int argc, char * argv[]) {

    int irank;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    fprintf(stdout, "Hello world from process %d\n", irank);
    MPI_Finalize();

    return EXIT_SUCCESS;
}
