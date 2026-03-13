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
#include <sys/param.h>
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
    stripe_read_u8(stripe, filepath, MPI_COMM_WORLD);

    int n = (int) stripe_get_ncols(stripe);
    int irow0 = (int) stripe_get_irow0(stripe);
    int irown = (int) stripe_get_irown(stripe);
    uint8_t * distance_from_layover = (uint8_t *) calloc(n, sizeof(uint8_t));
    int iroot = -1;

    for (int ivia = 0; ivia < n; ivia++) {
        iroot = blkdcmp_get_blk_owner(n, nranks, ivia);
        if (irank == iroot) {
            // copy the row corresponding to `ivia` from `stripe` to `distance_from_layover`
            for (int idst = 0; idst < n; idst++) {
                distance_from_layover[idst] = stripe_get_val(stripe, ivia, idst);
            }
        }

        // depending on whether `irank == iroot`, broadcast or receive `distance_from_layover`
        MPI_Bcast(distance_from_layover, n, MPI_UINT8_T, iroot, MPI_COMM_WORLD);

        for (int isrc = irow0; isrc <= irown; isrc++) {
            if (isrc == ivia) continue;
            for (int idst = 0; idst < n; idst++) {
                // conditionally update stripe.matrix using buffer
                uint8_t direct = stripe_get_val(stripe, isrc, idst);
                uint8_t detoured = stripe_get_val(stripe, isrc, ivia) + distance_from_layover[idst];
                stripe_set_val(stripe, isrc, idst, MIN(direct, detoured));
            }
        }
    }

    // free resources
    free(distance_from_layover);
    stripe_delete(&stripe);
    MPI_Finalize();

    return EXIT_SUCCESS;
}
