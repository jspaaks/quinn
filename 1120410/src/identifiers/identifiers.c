#include <stdio.h>
#include "identifiers/identifiers.h"

bool isvalid (char * identifier, size_t n) {

    // rule 0: characters at [0,..,n-1] should be in range '0' - '9', character at n should be '\0'
    // rule 1: identifiers can't start with 0
    // rule 2: consecutive digits can't be the same
    // rule 3: identifiers can't sum to 7, 11, or 13

    if (identifier[0] == '0') return false;

    int summed = 0;
    for (size_t i = 0; i < n; i++) {
        if (identifier[i] < '0') return false;
        if (identifier[i] > '9') return false;
        if (identifier[i] == identifier[i+1]) return false;  // critically dependant on rule 0
        summed += identifier[i] - '0';
    }

    if (summed == 7) return false;
    if (summed == 11) return false;
    if (summed == 13) return false;

    return true;
}
