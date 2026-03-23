#include <assert.h>
#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "equation/equation.h"

int main (int argc, char * argv[]) {
    int irank;
    int nranks;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    size_t n = 50;
    assert(n % 2 == 0 && "n should be even");
    size_t m = n / 2;
    double x0 = 0.0;
    double xa;
    double xb;
    double xn = 1.0;
    double summed = 0.0;
    for (size_t i = 1 + irank; i <= m; i += nranks) {
        xa = x0 + (xn - x0) * (2 * i - 1) / n;
        xb = x0 + (xn - x0) * (2 * i) / n;
        summed += 4 * eval(xa) + 2 * eval(xb);
    }
    double summed_gbl;
    MPI_Reduce(&summed, &summed_gbl, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (irank == 0) {
        double numer = eval(x0) - eval(xn) + summed_gbl;
        double denom = 3 * n;
        double area = numer / denom;
        fprintf(stdout, "Area under the curve is: %.10lf\n", area);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
