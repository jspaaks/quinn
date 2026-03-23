#ifdef SIEVE_TRAP_DBG
#include <limits.h>          // HOST_NAME_MAX
#endif // SIEVE_TRAP_DBG
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef SIEVE_TRAP_DBG
#include <unistd.h>          // gethostname, getpid, sleep
#endif // SIEVE_TRAP_DBG
#include "blkdcmp/blkdcmp.h"

void show_usage (FILE * stream, const char * programname);

int main (int argc, char * argv[]) {

    MPI_Init(&argc, &argv);

#ifdef SIEVE_TRAP_DBG
    {
        volatile int iswaiting = 1;
        char hostname[HOST_NAME_MAX + 1];
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
#endif // SIEVE_TRAP_DBG

    int irank = -1;                    // this process's rank
    int nranks = -1;                   // number of processes

    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    // verify that the user is using the program as intended
    if (argc != 2) {
        if (irank == 0) {
            show_usage(stderr, argv[0]);
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
        // wait for rank 0's printing
        MPI_Barrier(MPI_COMM_WORLD);
        return EXIT_FAILURE;
    }

    // optionally show the help and exit
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        if (irank == 0) {
            show_usage(stdout, argv[0]);
        }
        MPI_Finalize();
        return EXIT_SUCCESS;
    }

    constexpr int msg_cap = 1000; // error message capacity

    int count;                    // prime count on this process
    int count0;                   // accumulated prime count (only on process 0)
    double elapsed_time;          // parallel execution time
    int first;                    // index of first multiple of the current prime on this process
    int index0;                   // index of current prime (only on process 0)
    bool * isnonprime = nullptr;  // portion of the sieve handled by this process
    int low_value;                // integer value corresponding to isnonprime[0] on this process
    char msg[msg_cap] = {};       // error message buffer
    int nelems;                   // number of elements in `isnonprime` handled by this process
    int prime;                    // current prime

    // start the timer
    MPI_Barrier(MPI_COMM_WORLD);
    elapsed_time = -MPI_Wtime();

    // get the maximum integer to include in the sieve from user input
    int n = atoi(argv[1]);

    // determine the integer represented by the first element of isnonprime in each process, as well
    // as the length of isnonprime in each process
    low_value = blkdcmp_get_idx_blk_s(n+1, nranks, irank);
    nelems = blkdcmp_get_blk_sz(n+1, nranks, irank);

    // verify that all primes are contained within the first process, otherwise abort
    if (irank == 0) {
        // calculate integer value corresponding to isnonprime[nelems-1] on process 0
        int high_value = blkdcmp_get_idx_blk_e(n+1, nranks, 0);
        if (high_value < (int) sqrt(n)) {
            strncpy(msg, "Too many processes\n", msg_cap - 1);
            goto err;
        }
    }

    // allocate this process's share of the sieve
    isnonprime = (bool *) calloc(nelems, sizeof(bool));
    if (isnonprime == nullptr) {
        strncpy(msg, "Error allocating dynamic memory for isnonprime array.\n", msg_cap - 1);
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

        if (irank == 0) {
            // use only process 0's isnonprime array to determine which index is the next prime
            while (isnonprime[++index0]);
            prime = index0;
            // send the prime to the next process
            MPI_Send(&prime, 1, MPI_INT, irank + 1, 0, MPI_COMM_WORLD);
        } else if (irank == nranks - 1) {
            // receive a prime from the previous process
            MPI_Status status = {};
            MPI_Recv(&prime, 1, MPI_INT, irank - 1, 0, MPI_COMM_WORLD, &status);
        } else {
            // receive a prime from the previous process and send it on to the next
            MPI_Status status = {};
            MPI_Recv(&prime, 1, MPI_INT, irank - 1, 0, MPI_COMM_WORLD, &status);
            MPI_Send(&prime, 1, MPI_INT, irank + 1, 0, MPI_COMM_WORLD);
        }

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
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    free(isnonprime);
    return EXIT_FAILURE;
}

void show_usage (FILE * stream, const char * programname) {
    fprintf(stream,
            "Usage: mpirun -np P %s N\n"
            "\n"
            "    Use P processes to determine the number of primes in the\n"
            "    interval [2, N] using the Sieve of Eratosthenes with\n"
            "    Lester's performance improvements.\n"
            "\n", programname);
}
