#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef SIEVE_TRAP
#include <unistd.h>
#endif // SIEVE_TRAP
#include "blkdcmp/blkdcmp.h"

int main (int argc, char *argv[]) {

    // global / constant across processes
    char msg[1001] = {};          // error message buffer
    int n;                        // maximum integer to include in the sieve
    int nranks;                   // number of processes
    int prime;                    // current prime

    // local / variable across processes
    int count;                    // prime count on this process
    double elapsed_time;          // parallel execution time
    int first;                    // index of first multiple of the current prime on this process
    int irank;                    // this process's rank
    int low_value;                // integer value corresponding to isnonprime[0] on this process
    bool * isnonprime = nullptr;  // portion of the sieve handled by this process
    int nelems;                   // number of elements in `isnonprime` handled by this process

    // local to process 0
    int count0;                   // accumulated prime count
    int high_value0;              // integer value corresponding to isnonprime[nelems-1] on process 0
    int index0;                   // index of current prime

    MPI_Init(&argc, &argv);

#ifdef SIEVE_TRAP
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
#endif

    // Start the timer
    MPI_Barrier(MPI_COMM_WORLD);
    elapsed_time = -MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    if (argc != 2) {
        strncpy(msg, "Usage: sieve N\n    Determine the number of primes in [2,...,N]\n", 1000);
        goto err;
    }

    // get the user input
    n = atoi(argv[1]);

    // determine the integer represented by the first element of isnonprime in each process, as well
    // as the length of isnonprime in each process
    low_value = blkdcmp_get_idx_blk_s(n+1, nranks, irank);
    nelems = blkdcmp_get_blk_sz(n+1, nranks, irank);

    // verify that all primes are contained within the first process, otherwise abort
    high_value0 = blkdcmp_get_idx_blk_e(n+1, nranks, 0);
    if (high_value0 < (int) sqrt(n)) {
        strncpy(msg, "Too many processes\n", 1000);
        goto err;
    }

    // allocate this process's share of the sieve
    isnonprime = (bool *) calloc(nelems, sizeof(bool));
    if (isnonprime == nullptr) {
        strncpy(msg, "Error allocating dynamic memory for isnonprime array.\n", 1000);
        goto err;
    }
    if (irank == 0) {
        isnonprime[0] = true;  // integer 0 is not prime
        isnonprime[1] = true;  // integer 1 is not prime
        index0 = 2;            // start assessing at index0 = 2
    }
    prime = 2;

    // Let each process mark its part of the sieve
    do {
        if (low_value < prime * prime) {
            // multiples smaller than prime^2 were already marked in a previous iteration, focus on
            // identifying the index of prime^2
            first = prime * prime - low_value;
        } else {
            // multiples larger than prime^2
            if (low_value % prime == 0) {
                first = 0;
            } else {
                first = prime - (low_value % prime);
            }
        }

        // starting at the index of the first multiple of the current prime in this process's
        // isnonprime, mark it and all subsequent multiples as nonprime
        for (int ielem = first; ielem < nelems; ielem += prime) {
            isnonprime[ielem] = true;
        }

        // use only process 0's isnonprime array to determine which index is the next prime
        if (irank == 0) {
            while (isnonprime[++index0]);
            prime = index0;
        }

        // broadcast the value of the newly found next prime to all other processes
        MPI_Bcast(&prime, 1, MPI_INT, 0, MPI_COMM_WORLD);

    } while (prime * prime <= n);

    // determine the total number of primes found in this process's 'isnonprime' array, and send it
    // to process0 using a reduce operation
    count = 0;
    for (int ielem = 0; ielem < nelems; ielem++) {
        if (isnonprime[ielem] == false) {
            count++;
        }
    }
    MPI_Reduce(&count, &count0, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // print the results
    if (irank == 0) {
        elapsed_time += MPI_Wtime();
        fprintf(stdout, "%d primes are less than or equal to %d\n", count0, n);
        fprintf(stdout, "Total elapsed time: %10.6f\n", elapsed_time);
    }
    MPI_Finalize();

    free(isnonprime);
    return EXIT_SUCCESS;
err:
    if (irank == 0) {
        fprintf(stderr, "%s", msg);
    }
    MPI_Finalize();
    free(isnonprime);
    return EXIT_FAILURE;
}
