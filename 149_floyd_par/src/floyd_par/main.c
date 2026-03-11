/*
    parallel implementation of floyd's algorithm
*/

#include "blkdcmp/blkdcmp.h"
#include "idx/idx.h"
#include "stripe.h"
#include <inttypes.h>
#include <mpi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef FLOYD_PAR_TRAP_DBG
#include <unistd.h>
#endif // FLOYD_PAR_TRAP_DBG

typedef struct stripe Stripe;

int main (int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

#ifdef FLOYD_PAR_TRAP_DBG
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
#endif // FLOYD_PAR_TRAP_DBG

    int irank = -1;
    int nranks = -1;

    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    if (argc != 2) {
        if (irank == 0) {
            fprintf(stderr,
                    "Usage: floyd-par FILEPATH\n"
                    "    Read IDX formatted adjacency matrix from FILEPATH and calculate\n"
                    "    the all-pairs shortest path using a parallel implementation of\n"
                    "    Floyd's algorithm.\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    const char * filepath = argv[1];
    Stripe * stripe = stripe_new();
    stripe_read(stripe, filepath, MPI_COMM_WORLD);

    // presumably, each process would do something useful with
    // its slice of the data in rows.buffer here

    // free resources
    stripe_delete(&stripe);
    MPI_Finalize();

    return EXIT_SUCCESS;
}
