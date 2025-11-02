#ifndef SIEVE_SIEVING_H_INCLUDED
#define SIEVE_SIEVING_H_INCLUDED

int accumulate_total_number_of_primes (int blk_sz, const bool * isnonprime);
void determine_first (int prime, int low_value, int * first);
void determine_next_prime (int irank, const bool * isnonprime, int * index0, int * prime);
int idx2val (int idx);
bool iseven (int v);
void mark_sieve (int prime, int high_value, int first, int blk_s, bool * isnonprime);
int val2idx (int val);

#endif //SIEVE_SIEVING_H_INCLUDED
