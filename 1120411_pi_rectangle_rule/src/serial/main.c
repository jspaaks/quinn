#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "equation/equation.h"

int main (void) {
    double x;
    double ysummed = 0.0;
    size_t nintervals = 10000000000;
    double w = (1.0 - 0.0) / nintervals;
    for (size_t i = 0; i < nintervals; i++) {
        x = (i + 0.5) * w;
        ysummed += eval(x);
    }
    double area = ysummed * w;
    fprintf(stdout, "Area under the curve is: %.10lf\n", area);
    return EXIT_SUCCESS;
}
