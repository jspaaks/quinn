#include "vector.h"
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>


int * vector_calloc (int nelems) {
    // allocate a contiguous block of memory for the contents of the vector
    int * vector = calloc(nelems, sizeof(int));
    if (vector == nullptr) {
        int code = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating buffer for vector, aborting.\n", code);
        MPI_Finalize();
        exit(code);
    }
    return vector;
}


void vector_free (int ** vector) {
    if (vector == nullptr) return;
    if (*vector == nullptr) return;
    free(&(*vector)[0]);
    *vector = nullptr;
}


void vector_print (FILE * stream, int nelems_chunk, int nelems_whole, const int * lengths, const int * offsets, const int * vector_chunk, int ** vector_whole, MPI_Comm comm) {
    const int iroot = 0;
    int irank = -1;
    MPI_Comm_rank(comm, &irank);
    MPI_Gatherv(vector_chunk, nelems_chunk, MPI_INT, *vector_whole, lengths, offsets, MPI_INT, iroot, comm);
    if (irank == iroot) {
        fprintf(stream, "RHS operand (column vector):\n");
        for (int ielem = 0; ielem < nelems_whole; ielem++) {
            fprintf(stream, " % 2d\n", (*vector_whole)[ielem]);
        }
    }
}


void vector_rand_init (FILE * stream, int nelems, int ** vector) {
    if (*vector == nullptr) {
        int code = __LINE__;
        fprintf(stream, "ERROR %d: didn't expect vector to be nullptr, aborting.\n", code);
        MPI_Finalize();
        exit(code);
    }
    // populate `vector` with random integers 0..9
    for (int ielem = 0; ielem < nelems; ielem++) {
        (*vector)[ielem] = rand() % 10;
    }
}
