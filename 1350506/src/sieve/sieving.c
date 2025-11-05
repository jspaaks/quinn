#include "sieving.h"
#include <mpi.h>

int accumulate_total_number_of_primes (int blk_sz, const bool * iscomposite) {
    // determine the total number of primes found in this process's 'iscomposite' array
    int count = 0;
    for (int i = 0; i < blk_sz; i++) {
        if (iscomposite[i] == false) {
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
    return;
}

void determine_next_prime (int irank, const bool * iscomposite, int * index0, int * prime) {
    // use only process 0's iscomposite array to determine which index is the next prime
    if (irank == 0) {
        while (iscomposite[++(*index0)]);
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

void mark_sieve (int prime, int high_value, int first, int blk_s, bool * iscomposite) {
    for (int i = first; i <= high_value; i += 2 * prime) {
        int j = val2idx(i) - blk_s;
        iscomposite[j] = true;
    }
    return;
}

int val2idx (int val) {
    return (val - 3) / 2;  // truncate
}
