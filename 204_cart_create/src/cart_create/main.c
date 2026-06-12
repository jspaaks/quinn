#include "dual_types.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>              // gethostname, getpid, sleep


#define NDIMS 2
#define NROWS 4
#define NCOLS 3


struct world {
    int irank;
    MPI_Comm comm;
};


struct cart {
    int irank;
    int coords[NDIMS];
    struct {
        int top;
        int right;
        int bottom;
        int left;
    } neighbors;
    MPI_Comm comm;
};


int main (int argc, char * argv[]) {

    // if the program exits unexpectedly, the program should reset errcode to a nonzero value
    int errcode = EXIT_SUCCESS;

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

    // initialize the struct that holds information about the world topology
    struct world world = {
        .comm = MPI_COMM_WORLD,
        .irank = -1,
    };

    // let each process get its own rank within MPI_COMM_WORLD
    MPI_Comm_rank(world.comm, &world.irank);

    // get the number of ranks
    int nranks = -1;
    MPI_Comm_size(world.comm, &nranks);
    if (nranks != 12) {
        if (world.irank == 0) {
            fprintf(stderr, "Program is meant to run on 12 processes, aborting.\n");
        }
        goto cleanup;
    }

    // empty-initialize the struct that holds information about the cartesian topology
    struct cart cart = {};

    // let's create a Cartesian grid of processes of 4 rows and 3 columns, with periodic boundaries
    // horizontally but not vertically.
    int dims[NDIMS] = {NROWS, NCOLS};
    int isperiodic[NDIMS] = {0, 1};
    {
        int reorder = 0;
        MPI_Cart_create(world.comm, NDIMS, dims, isperiodic, reorder, &cart.comm);
    }

    // let each process get its own rank within cart.comm
    MPI_Comm_rank(cart.comm, &cart.irank);

    // use the rank to derive my coordinates in the topology
    MPI_Cart_coords(cart.comm, cart.irank, NDIMS, cart.coords);

    // use my rank to find the ranks of the nodes in the topology that are above, to the left, below,
    // and to the right of me, or -2 if there aren't any nodes in that direction
    {
        const int vertically = 0;
        const int displacement = 1;
        MPI_Cart_shift(cart.comm, vertically, displacement, &cart.neighbors.top, &cart.neighbors.bottom);
    }
    {
        const int horizontally = 1;
        const int displacement = 1;
        MPI_Cart_shift(cart.comm, horizontally, displacement, &cart.neighbors.left, &cart.neighbors.right);
    }

    // if you're root, allocate space for the array of struct carts; otherwise do nothing
    struct cart * carts = nullptr;
    if (cart.irank == 0) {
        carts = calloc(nranks, sizeof(struct cart));
        if (carts == nullptr) {
            errcode = __LINE__;
            fprintf(stderr, "ERROR %d: could not allocate memory for carts, aborting.\n", errcode);
            goto cleanup;
        }
    }

    // gather each process's cart struct on root
    MPI_Gather(&cart, sizeof(struct cart), MPI_BYTE, carts, sizeof(struct cart), MPI_BYTE, 0, cart.comm);

    // if you're root, print the cart information for all processes within cart.comm
    if (cart.irank == 0) {
        fprintf(stdout,
                "Using a Cartesian topology communicator with %d rows and %d columns of processes\n"
                "with a periodic boundary horizontally, but not vertically.\n\n"
                "row  |  column  |  rank  |  top  |  right  |  bottom  |  left\n", NROWS, NCOLS);
        for (int i = 0; i < nranks; i++) {
            fprintf(stdout,
                    "%4d | %8d | %6d | %5d | %7d | %8d | %6d \n",
                    carts[i].coords[0],
                    carts[i].coords[1],
                    carts[i].irank,
                    carts[i].neighbors.top,
                    carts[i].neighbors.right,
                    carts[i].neighbors.bottom,
                    carts[i].neighbors.left);
        }
    }

cleanup:

    // release memory resources associated with carts
    free(carts);
    carts = nullptr;

    // terminate the MPI execution environment
    MPI_Finalize();

    return errcode;
}
