#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "blkdcmp/blkdcmp.h"

int main (int argc, char *argv[]) {
    int    count;        /* Local prime count */
    double elapsed_time; /* Parallel execution time */
    int    first;        /* Index of first multiple */
    int    global_count; /* Global prime count */
    int    index;        /* Index of current prime */
    int    irank;        /* Process rank */
    int    low_value;    /* Lowest value on this proc */
    bool * marked;       /* Portion of 2,...,`n` */
    int    n;            /* Sieving from 2,...,`n` */
    int    nranks;       /* Number of processes */
    int    prime;        /* Current prime */
    int    proc0_size;   /* Size of proc0's subarray */
    int    nelems;         /* Elements in `marked` */

    MPI_Init(&argc, &argv);

    /* Start the timer */

    MPI_Barrier(MPI_COMM_WORLD);
    elapsed_time = -MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    if (argc != 2) {
        if (irank == 0) {
            printf("Command line: %s N\n    Determine the number of primes in [2,...,N]\n", argv[0]);
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    n = atoi(argv[1]);

    /* Figure out this process's share of the array, as
       well as the integers represented by the first and
       last elements */

    low_value = 2 + blkdcmp_get_idx_blk_s(n-1, nranks, irank);
    nelems = blkdcmp_get_blk_sz(n-1, nranks, irank);

    /* Bail out if all the primes used for sieving are
       not all held by process 0 */

    proc0_size = (n-1) / nranks;

    if ((1 + proc0_size) < (int) sqrt((double) n)) {
        if (irank == 0) {
            printf("Too many processes\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    /* Allocate this process's share of the array */

    marked = (bool *) calloc(nelems, sizeof(bool));
    if (marked == nullptr) {
        if (irank == 0) {
            printf("Cannot allocate enough memory.\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    if (irank == 0) {
        index = 0;
    }
    prime = 2;
    do {
        if (prime * prime > low_value) {
            first = prime * prime - low_value;
        } else {
            if (!(low_value % prime)) {
                first = 0;
            } else {
                first = prime - (low_value % prime);
            }
        }
        for (int ielem = first; ielem < nelems; ielem += prime) {
            marked[ielem] = true;
        }
        if (irank == 0) {
            while (marked[++index]);
            prime = index + 2;
        }
        MPI_Bcast(&prime, 1, MPI_INT, 0, MPI_COMM_WORLD);
    } while (prime * prime <= n);

    count = 0;
    for (int ielem = 0; ielem < nelems; ielem++) {
        if (!marked[ielem]) {
            count++;
        }
    }
    MPI_Reduce(&count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    /* Stop the timer */

    elapsed_time += MPI_Wtime();

    /* Print the results */

    if (irank == 0) {
        printf("%d primes are less than or equal to %d\n", global_count, n);
        printf("Total elapsed time: %10.6f\n", elapsed_time);
    }
    MPI_Finalize();

    free(marked);
    return EXIT_SUCCESS;
}
