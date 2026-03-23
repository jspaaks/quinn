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

    double x;
    double ysummed = 0.0;
    size_t nintervals = 10000000000;
    double w = (1.0 - 0.0) / nintervals;
    for (size_t i = irank; i < nintervals; i += nranks) {
        x = (i + 0.5) * w;
        ysummed += eval(x);
    }
    double area = ysummed * w;

    double area_gbl;
    MPI_Reduce(&area, &area_gbl, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (irank == 0) {
        fprintf(stdout, "Area under the curve is: %.10lf\n", area_gbl);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
