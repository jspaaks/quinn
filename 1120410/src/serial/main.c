#include <stdio.h>
#include <stdlib.h>
#include "identifiers/identifiers.h"

int main (void) {
    const size_t n = 1000000;
    char identifier[7] = "......";
    size_t count = 0;
    for (size_t i = 0; i < n; i++) {
        sprintf(&identifier[0], "%06zu", i);
        bool b = isvalid(&identifier[0], 6);
        if (b) {
            //fprintf(stdout, "%s\n", identifier);
            count++;
        }
    }
    fprintf(stdout, "Found %zu valid identifiers\n", count);
    return EXIT_SUCCESS;
}
