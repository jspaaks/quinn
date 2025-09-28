#include <inttypes.h>
#include <mpi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char * argv[]) {

    int irank;
    int nranks;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    uint64_t p = (uint64_t) atoi(argv[1]);
    if (irank == 0) {
        if (argc != 2 || p == 0) {
            fprintf(stdout, "Expected exactly one integer command line argument, aborting.\n");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
    }

    uint64_t actual = 0;
    uint64_t local = 0;
    for (uint64_t i = irank; i < p; i += nranks) {
        local += i + 1;
    }

    MPI_Reduce(&local, &actual, 1, MPI_UINT64_T,
               MPI_SUM, 0, MPI_COMM_WORLD);

    if (irank == 0) {
        uint64_t expected = p * (p + 1) / 2;
        if (actual != expected) {
            fprintf(stdout, "Calculated value %" PRIu64 " does not match expected va"
                            "lue of %" PRIu64 "\n", actual, expected);
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        } else {
            fprintf(stdout, "The sum was accurately calculated as %" PRIu64 "\n", actual);
        }
    }

    MPI_Finalize();

    return EXIT_SUCCESS;
}
