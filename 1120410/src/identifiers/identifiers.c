#include <stdio.h>
#include "identifiers/identifiers.h"

bool isvalid (char * identifier, size_t n) {
    // rule 0: character should be in range '0' - '9'
    for (size_t i = 0; i < n; i++) {
        if (identifier[i] < '0') return false;
        if (identifier[i] > '9') return false;
    }

    // rule 1: identifiers can't start with 0
    if (identifier[0] == '0') return false;

    // rule 2: consecutive digits can't be the same
    for (size_t i = 1; i < n; i++) {
        if (identifier[i-1] == identifier[i]) return false;
    }

    // rule 3: identifiers can't start sum to 7, 11, or 13
    int summed = 0;
    for (size_t i = 0; i < n; i++) {
        summed += identifier[i] - '0';
    }

    if (summed == 7) return false;
    if (summed == 11) return false;
    if (summed == 13) return false;

    return true;
}
