#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>              // gethostname, getpid, sleep

#define NDIMS 2


int main (int argc, char * argv[]) {

    // if the program exits unexpectedly, the program should reset errcode to a nonzero value
    int errcode = EXIT_SUCCESS;

    // initialize mpi
    MPI_Init(&argc, &argv);

    // conditionally trap the debugger if a compile time variable has been defined
#ifdef DIMS_CREATE_TRAP_DBG
    {
        volatile bool iswaiting = true;
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        fprintf(stdout, "PID %d on %s waiting for debugger attach...\n", getpid(), hostname);
        fflush(stdout);
        while (iswaiting) {
            // attach the debugger with gdb --pid <the pid>; once attached the debugger will halt
            // the program here; use GDB commands to set iswaiting to false, e.g.
            // (gdb) set var iswaiting = 0;
            sleep(3);
        }
    }
#endif // DIMS_CREATE_TRAP_DBG

    // declare variables
    int irank = -1;
    int nranks = -1;

    // let each process get its rank
    MPI_Comm_rank(MPI_COMM_WORLD, &irank);

    // let each process get the number of ranks
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    // verify that the program received the expected number of arguments
    if (argc != 1) {
        if(irank == 0) {
            fprintf(
                stderr,
                "Incorrect number of arguments, aborting.\n"
                "\n"
                "Usage: mpirun -np NRANKS %s\n"
                "\n"
                "  Multiprocess program that illustrates how the number of processes\n"
                "  NRANKS will be divided over a cartesian grid of 2 dimensions.\n"
                "\n",
                argv[0]
            );
        }
        goto cleanup;
    }
    int sizes[NDIMS] = {};

    MPI_Dims_create(nranks, NDIMS, sizes);

    if (irank == 0) {
        fprintf(stdout, "Having %d processes will result in a %dx%d cartesian topology.\n", nranks, sizes[0], sizes[1]);
    }

cleanup:

    // terminate the MPI execution environment
    MPI_Finalize();

    return errcode;
}
