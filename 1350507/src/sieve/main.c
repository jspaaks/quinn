#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef SIEVE_TRAP_DBG
#include <unistd.h>
#endif // SIEVE_TRAP_DBG
#include "blkdcmp/blkdcmp.h"

/* --------------------------------  function declarations   ------------------------------------ */

int accumulate_total_number_of_primes (int blk_sz, bool * isnonprime);
void determine_first (int prime, int low_value, int * first);
void determine_next_prime (int irank, int * index0, int * prime, bool * isnonprime);
int idx2val (int idx);
bool iseven (int v);
void mark_sieve (int prime, int high_value, int first, int blk_s, bool * isnonprime);
int val2idx (int val);

/* ----------------------------------  function defitions   ------------------------------------- */

int accumulate_total_number_of_primes (int blk_sz, bool * isnonprime) {
    int count = 0;
    for (int i = 0; i < blk_sz; i++) {
        if (isnonprime[i] == false) {
            count++;
        }
    }
    return count;
}

void determine_first (int prime, int low_value, int * first) {
    if (prime * prime >= low_value) {
        // multiples smaller than prime^2 were already marked in a previous iteration,
        // start at prime * prime
        *first = prime * prime;
    } else {
        // multiples larger than prime * prime
        if (low_value % prime == 0) {
            *first = low_value;
        } else {
            int tmp = low_value + prime - (low_value % prime);
            // in case the domain value is even, add one prime distance, because even integers
            // have no representation in the sieve
            *first = iseven(tmp) ? tmp + prime : tmp;
        }
    }
}

void determine_next_prime (int irank, int * index0, int * prime, bool * isnonprime) {
    if (irank == 0) {
        while (isnonprime[++(*index0)]);
        *prime = idx2val(*index0);
    }
    // broadcast the value of the newly found next prime to all other processes
    MPI_Bcast(prime, 1, MPI_INT, 0, MPI_COMM_WORLD);
    return;
}

int idx2val (int idx) {
    return idx * 2 + 3;
}

bool iseven (int v) {
    return v % 2 == 0;
}

int main (int argc, char *argv[]) {

    MPI_Init(&argc, &argv);

#ifdef SIEVE_TRAP_DBG
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
#endif // SIEVE_TRAP_DBG

    // global / constant across processes
    char msg[1001] = {};          // error message buffer
    int m;                        // number of odd integers between 3 and n inclusive, i.e. length of the sieve
    int n;                        // domain value corresponding to last element of sieve
    int nranks;                   // number of processes
    int prime;                    // current prime

    // local / variable across processes
    int blk_e;                    // index into sieve where this process's chunk ends
    int blk_s;                    // index into sieve where this process's chunk starts
    int blk_sz;                   // number of elements from sieve that are handled by this process
    int count;                    // prime count on this process
    double elapsed_time;          // parallel execution time
    int first;                    // domain value of first multiple of the current prime on this process
    int high_value;               // domain value corresponding to isnonprime[blk_e] on this process
    int irank;                    // this process's rank
    int low_value;                // domain value corresponding to isnonprime[0] on this process
    bool * isnonprime = nullptr;  // portion of the sieve handled by this process

    // local to process 0
    int blk_e0;                   // index into sieve where process 0's chunk ends
    int count0;                   // accumulated prime count
    int high_value0;              // domain value corresponding to isnonprime[nelems-1] on process 0
    int index0;                   // index into isnonprime of current prime

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

    if (n < 3) {
        fprintf(stderr, "N needs to be at least 3, aborting\n");
        return EXIT_FAILURE;
    }

    // derive the number of odd integers between 3 and n inclusive, and use the blkdcmp library to
    // determine the starting index, the ending index, and size of each block
    m = val2idx(n) + 1;
    blk_s = blkdcmp_get_idx_blk_s(m, nranks, irank);
    blk_e = blkdcmp_get_idx_blk_e(m, nranks, irank);
    blk_e0 = blkdcmp_get_idx_blk_e(m, nranks, 0);
    blk_sz = blkdcmp_get_blk_sz(m, nranks, irank);

    // determine the domain value corresponding to the first element of isnonprime in each process
    low_value = idx2val(blk_s);
    high_value = idx2val(blk_e);

    // verify that all primes are contained within the first process, otherwise abort
    high_value0 = idx2val(blk_e0);
    if (high_value0 < (int) sqrt(n)) {
        strncpy(msg, "Too many processes\n", 1000);
        goto err;
    }

    // allocate this process's share of the sieve
    isnonprime = (bool *) calloc(blk_sz, sizeof(bool));
    if (isnonprime == nullptr) {
        strncpy(msg, "Error allocating dynamic memory for isnonprime array.\n", 1000);
        goto err;
    }

    // the first index of the sieve that we want to consider is 0, which corresponds to domain value 3
    index0 = 0;
    prime = idx2val(index0);

    // Let each process mark its part of the sieve
    do {
        // determine the domain value of the first element that is a multiple of prime
        determine_first(prime, low_value, &first);

        // starting at the index of the first multiple of the current prime in this process's
        // isnonprime, mark it and all subsequent multiples as nonprime
        mark_sieve(prime, high_value, first, blk_s, &isnonprime[0]);

        // determine the value of the next prime as well as its index0
        determine_next_prime(irank, &index0, &prime, &isnonprime[0]);

    } while (prime * prime <= n);

    // determine the total number of primes found in this process's 'isnonprime' array, and send it
    // to process0 using a reduce operation
    count = accumulate_total_number_of_primes (blk_sz, &isnonprime[0]);
    MPI_Reduce(&count, &count0, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // print the results
    if (irank == 0) {
        elapsed_time += MPI_Wtime();
        fprintf(stdout, "%d primes are less than or equal to %d\n", count0 + 1, n);
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

void mark_sieve (int prime, int high_value, int first, int blk_s, bool * isnonprime) {
    for (int i = first; i <= high_value; i += 2 * prime) {
        int j = val2idx(i) - blk_s;
        isnonprime[j] = true;
    }
}

int val2idx (int val) {
    return (val - 3) / 2;
}
