#include "blkdcmp/blkdcmp.h"
#include "dual_types.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>              // gethostname, getpid, sleep

#define NDIMS 2
#define NROWS 4
#define NCOLS 3


int main (int argc, char * argv[]) {

    // initialize mpi
    MPI_Init(&argc, &argv);

    // conditionally trap the debugger if a compile time variable has been defined
#ifdef CART_CREATE_TRAP_DBG
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
#endif // CART_CREATE_TRAP_DBG

    // get your own rank
    int irank = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &irank);

    // get the number of ranks
    int nranks = -1;
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);
    if (nranks != 12) {
        if (irank == 0) {
            fprintf(stderr, "Program is meant to run on 12 processes, aborting.\n");
        }
        goto cleanup;
    }

    // let's create a Cartesian grid of processes of 4 rows and 3 columns, with periodic boundaries
    // horizontally but not vertically.
    int dims[NDIMS] = {NROWS, NCOLS};
    int isperiodic[NDIMS] = {0, 1};
    MPI_Comm comm_cart = {0};
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, isperiodic, 0, &comm_cart);

    // use the rank to derive my coordinates in the topology
    int coords[NDIMS] = {-1, -1};
    MPI_Cart_coords(comm_cart, irank, NDIMS, coords);

    // use my rank to find the ranks of the nodes in the topology that are above, to the left, below,
    // and to the right of me, or -2 if there aren't any nodes in that direction
    int irank_top = -1;
    int irank_bottom = -1;
    int irank_left = -1;
    int irank_right = -1;
    {
        int vertically = 0;
        int displacement = 1;
        MPI_Cart_shift(comm_cart, vertically, displacement, &irank_top, &irank_bottom);
    }
    {
        int horizontally = 1;
        int displacement = 1;
        MPI_Cart_shift(comm_cart, horizontally, displacement, &irank_left, &irank_right);
    }

    // let each rank print its diagnositic information
    if (irank == 0) {
        fprintf(stdout,
                "Using a Cartesian topology communicator with %d rows and %d columns of processes\n"
                "with a periodic boundary horizontally, but not vertically.\n\n"
                "row  |  column  |  rank  |  top  |  right  |  bottom  |  left\n", NROWS, NCOLS);
        fflush(stdout);
    }
    sleep(1);
    MPI_Barrier(comm_cart);

    fprintf(stdout, "%4d | %8d | %6d | %5d | %7d | %8d | %6d \n", coords[0], coords[1], irank, irank_top, irank_right, irank_bottom, irank_left);

cleanup:

    // terminate the MPI execution environment
    MPI_Finalize();

    return EXIT_SUCCESS;
}
