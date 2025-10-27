#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "blkdcmp/blkdcmp.h"

int main (int argc, char *argv[]) {
    int    count;        // Prime count on this process
    double elapsed_time; // Parallel execution time
    int    first;        // Index of first multiple
    int    global_count; // Global prime count
    int    high_value0;  // Highest value on process 0
    int    index;        // Index of current prime
    int    irank;        // This process's rank
    int    low_value;    // Lowest value on this process
    bool * isnonprime;   // Portion of 2,...,`n`
    int    n;            // Determine the number of primes that are smaller than this integer
    int    nranks;       // Number of processes
    int    prime;        // Current prime
    int    nelems0;      // Size of proc0's subarray
    int    nelems;       // Elements in `isnonprime`

    MPI_Init(&argc, &argv);

    // Start the timer

    MPI_Barrier(MPI_COMM_WORLD);
    elapsed_time = -MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    if (argc != 2) {
        if (irank == 0) {
            fprintf(stderr, "Command line: %s N\n    Determine the number of primes in [2,...,N]\n", argv[0]);
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    n = atoi(argv[1]);

    // Figure out this process's share of the array, as well as the integers represented by the
    // first and last elements
    low_value = 2 + blkdcmp_get_idx_blk_s(n-1, nranks, irank);
    nelems = blkdcmp_get_blk_sz(n-1, nranks, irank);

    // Bail out if all the primes used for sieving are not all held by process 0
    high_value0 = 2 + blkdcmp_get_idx_blk_e(n-1, nranks, 0);
    if (high_value0 < (int) sqrt((double) n)) {
        if (irank == 0) {
            fprintf(stderr, "Too many processes\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    // Allocate this process's share of the array
    isnonprime = (bool *) calloc(nelems, sizeof(bool));
    if (isnonprime == nullptr) {
        if (irank == 0) {
            fprintf(stderr, "Cannot allocate enough memory.\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    // Start marking elements of 'isnonprime' as nonprime until you get to the prime whose square is
    // larger than 'n'. Each time you find a new prime, tell the other processes about it using a
    // broadcast so that they can mark the prime's multiples in their respective 'isnonprime' arrays
    if (irank == 0) {
        index = 0;
    }
    prime = 2;
    do {
        if (prime * prime > low_value) {
            first = prime * prime - low_value;
        } else {
            if (low_value % prime == 0) {
                first = 0;
            } else {
                first = prime - (low_value % prime);
            }
        }
        for (int ielem = first; ielem < nelems; ielem += prime) {
            isnonprime[ielem] = true;
        }
        if (irank == 0) {
            while (isnonprime[++index]);
            prime = index + 2;
        }
        MPI_Bcast(&prime, 1, MPI_INT, 0, MPI_COMM_WORLD);
    } while (prime * prime <= n);

    // Determine the total number of primes found in this process's 'isnonprime' array, and send it
    // to process0 using a reduce operation
    count = 0;
    for (int ielem = 0; ielem < nelems; ielem++) {
        if (isnonprime[ielem] == false) {
            count++;
        }
    }
    MPI_Reduce(&count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Stop the timer
    elapsed_time += MPI_Wtime();

    // Print the results
    if (irank == 0) {
        fprintf(stdout, "%d primes are less than or equal to %d\n", global_count, n);
        fprintf(stdout, "Total elapsed time: %10.6f\n", elapsed_time);
    }
    MPI_Finalize();

    free(isnonprime);
    return EXIT_SUCCESS;
}
