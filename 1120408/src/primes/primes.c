#include <stddef.h>
#include "primes/primes.h"

bool isprime(unsigned int n) {
    for (size_t i = 2; i < n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}
