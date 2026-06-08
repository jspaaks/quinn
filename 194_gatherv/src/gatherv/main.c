#include "blkdcmp/blkdcmp.h"
#include "dual_types.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>              // gethostname, getpid, sleep


#define MSGCAP 1000


static void print_vector (int nelems, const int * vector, int irank);


int main (int argc, char * argv[]) {

    int irank = -1;
    int nranks = -1;

    // initialize mpi
    MPI_Init(&argc, &argv);

    // conditionally trap the debugger if a compile time variable has been defined
#ifdef GATHERV_TRAP_DBG
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
#endif // GATHERV_TRAP_DBG

    // get your own rank and the total number of ranks
    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    // declare variables
    int * lengths = nullptr;
    int * offsets = nullptr;
    struct dual_int nelems = {.whole = 17, .chunk = -1};
    struct dual_intp vector = {};

    if (argc != 1) {
        if (irank == 0) {
            fprintf(stderr, "Expected no arguments, aborting.\n");
        }
        goto cleanup;
    }

    if (irank == 0) {
        fprintf(stderr,
                "Print chunks of a distributed vector, gather distributed\n"
                "data, and print the gathered result.\n");
    }
    sleep(1);


    // allocate and initialize the lengths array
    if (blkdcmp_lengths_calloc(stderr, nranks, &lengths)) goto cleanup;
    if (blkdcmp_lengths_init(stderr, nelems.whole, nranks, &lengths)) goto cleanup;
    nelems.chunk = lengths[irank];

    // allocate and initialize the offsets array
    if (blkdcmp_offsets_calloc(stderr, nranks, &offsets)) goto cleanup;
    if (blkdcmp_offsets_init(stderr, nranks, (const int *) lengths, &offsets)) goto cleanup;

    // allocate and zero-initalize the vector array
    vector.chunk = calloc(nelems.chunk, sizeof(int));
    if (irank == 0) {
        vector.whole = calloc(nelems.whole, sizeof(int));
    } else {
        vector.whole = calloc(1, sizeof(int));  // to avoid problems dereferencing, not actually used
    }

    // let each process initialize its chunk of the vector
    for (int ielem = 0; ielem < nelems.chunk; ielem++) {
        vector.chunk[ielem] = 100 + ielem + offsets[irank];
    }

    // let each process print its chunk
    print_vector(nelems.chunk, (const int *) vector.chunk, irank);
    fflush(stdout);

    // gather the contents of vector.whole from each process's vector.chunk using MPI_Gatherv
    MPI_Gatherv(vector.chunk, lengths[irank], MPI_INT, vector.whole, lengths, offsets, MPI_INT, 0, MPI_COMM_WORLD);

    // sleep briefly to help print in order (not guaranteed)
    sleep(1);

    // let root process print the gathered vector
    if (irank == 0) {
        print_vector(nelems.whole, (const int *) vector.whole, 0);
    }

cleanup:

    // free up momory resources associated with vector
    free(vector.chunk);
    free(vector.whole);

    // free the memory resources associated with the lengths array
    blkdcmp_lengths_free(&lengths);

    // free the memory resources associated with the offsets array
    blkdcmp_offsets_free(&offsets);

    // terminate the MPI execution environment
    MPI_Finalize();

    return EXIT_SUCCESS;
}


static void print_vector (int nelems, const int * vector, int irank) {
    char msg[MSGCAP] = {};
    int pos = 0;
    pos += snprintf(msg + pos, MSGCAP - pos, "(%d) ", irank);
    for (int ielem = 0; ielem < nelems; ielem++) {
        pos += snprintf(msg + pos, MSGCAP - pos, "%4d ", vector[ielem]);
    }
    pos += snprintf(msg + pos, MSGCAP - pos, "\n");
    fprintf(stdout, "%s", msg);
}