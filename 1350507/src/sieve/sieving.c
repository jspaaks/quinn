#include "blkdcmp/blkdcmp.h"
#include "sieving.h"

int accumulate_total_number_of_primes (int blk_sz, const bool * iscomposite) {
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
}

void determine_next_prime (const bool * iscomposite, int * index, int * prime) {
    while (iscomposite[++(*index)]);
    *prime = idx2val(*index);
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
}

int val2idx (int val) {
    return (val - 3) / 2;
}
