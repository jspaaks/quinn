/*
    In this iteration of the parallel implementation of the sieve of Erathostenes, we
       (1) skip dealing with even integers
       (2) have each process determine what the next prime is on its own
       (3) improve cache hits by marking subsections of the sieve for all primes before moving on to
           the next subsection
*/

#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#ifdef SIEVE_TRAP_DBG
#include <unistd.h>
#endif // SIEVE_TRAP_DBG
#include "blkdcmp/blkdcmp.h"
#include "sieving.h"


struct pair {
    int l;
    int r;
};

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
    struct pair m;                // number of odd integers between 3 and n inclusive, i.e. length of the sieve
    int n;                        // domain value corresponding to last element of sieve
    int nranks;                   // number of processes

    // local / variable across processes
    struct pair blk_e;            // index into sieve where this process's chunk ends
    struct pair blk_s;            // index into sieve where this process's chunk starts
    struct pair blk_sz;           // number of elements from sieve that are handled by this process
    int count;                    // prime count on this process
    double elapsed_time;          // parallel execution time
    struct pair first;            // domain value of first multiple of the current prime on this process
    struct pair high_value;       // domain value corresponding to isnonprime[blk_e] on this process
    int prime;                    // current prime
    int index;                    // index into isnonprime of current prime
    int irank;                    // this process's rank
    struct pair low_value;        // domain value corresponding to isnonprime[0] on this process
    bool * isnonprime = nullptr;  // portion of the sieve handled by this process

    // local to process 0
    int count0;                   // accumulated prime count

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
    m = (struct pair){
       .l = val2idx((int) floor(sqrt(n)) + 2) + 1,
       .r = val2idx(n) + 1,
    };
    blk_s = (struct pair){
        .l = 0,
        .r = blkdcmp_get_idx_blk_s(m.r, nranks, irank)
    };
    blk_e = (struct pair){
        .l = m.l - 1,
        .r = blkdcmp_get_idx_blk_e(m.r, nranks, irank)
    };
    blk_sz = (struct pair){
        .l = m.l,
        .r = blkdcmp_get_blk_sz(m.r, nranks, irank)
    };

    // determine the domain value corresponding to the first element of isnonprime in each process
    low_value = (struct pair){
        .l = idx2val(blk_s.l),
        .r = idx2val(blk_s.r)
    };
    high_value = (struct pair){
        .l = idx2val(blk_e.l),
        .r = idx2val(blk_e.r)
    };

    // allocate this process's share of the sieve
    isnonprime = (bool *) calloc(blk_sz.l + blk_sz.r, sizeof(bool));
    if (isnonprime == nullptr) {
        strncpy(msg, "Error allocating dynamic memory for isnonprime array.\n", 1000);
        goto err;
    }

    // the first index of the sieve that we want to consider is 0, which corresponds to domain value 3
    index = 0;
    prime = idx2val(index);

    // Let each process mark the left partition of the sieve
    do {
        // determine the domain value of the first element that is a multiple of prime
        determine_first(prime, low_value.l, &first.l);

        // starting at the index of the first multiple of the current prime of isnonprime, mark it
        // and all subsequent multiples as nonprime
        mark_sieve(prime, high_value.l, first.l, blk_s.l, &isnonprime[0]);

        // determine the value of the next prime as well as its index
        determine_next_prime(&isnonprime[0], &index, &prime);

    } while (prime * prime <= high_value.l);

    // Let each process mark the right partition of the sieve using a section by section approach

    const int section_sz = 100000; // guess the right section size
    int low = low_value.r;

    while (low <= high_value.r) {

        // reset index and prime
        index = 0;
        prime = idx2val(index);

        int high = low + section_sz;
        high = MIN(high, high_value.r);

        do {
            // determine the domain value of the first element that is a multiple of prime
            determine_first(prime, low, &first.r);

            // starting at the index of the first multiple of the current prime of isnonprime, mark it
            // and all subsequent multiples as nonprime
            mark_sieve(prime, high, first.r, blk_s.r, &isnonprime[blk_sz.l]);

            // determine the value of the next prime as well as its index
            determine_next_prime(&isnonprime[0], &index, &prime);
        } while (prime * prime <= high);

        low = high + 2; // +2 because low and high are domain values, and even integers are omitted
    };

    // determine the total number of primes found in this process's 'isnonprime' array, and send it
    // to process0 using a reduce operation
    count = accumulate_total_number_of_primes(blk_sz.r, &isnonprime[blk_sz.l]);
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
