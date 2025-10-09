#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "equation/equation.h"

int main (void) {

    size_t n = 50;
    assert(n % 2 == 0 && "n should be even");
    size_t m = n / 2;

    double x0 = 0.0;
    double xa;
    double xb;
    double xn = 1.0;
    double summed = 0.0;
    for (size_t i = 1; i <= m; i++) {
        xa = x0 + (xn - x0) * (2 * i - 1) / n;
        xb = x0 + (xn - x0) * (2 * i) / n;
        summed += 4 * eval(xa) + 2 * eval(xb);
    }
    double area = (eval(x0) - eval(xn) + summed) / 3 / n;
    fprintf(stdout, "Area under the curve is: %.10lf\n", area);

    return EXIT_SUCCESS;
}
