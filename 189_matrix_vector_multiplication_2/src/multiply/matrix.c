#include "matrix.h"
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>


int ** matrix_calloc (int nrows, int ncols) {
    // allocate a contiguous block of memory for the contents of the matrix
    int nelems = nrows * ncols;
    int * buf = calloc(nelems, sizeof(int));
    if (buf == nullptr) {
        int code = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating buffer for matrix, aborting.\n", code);
        MPI_Finalize();
        exit(code);
    }
    // allocate another contiguous block of memory for the pointers to the beginning of each row in the matrix
    int ** matrix = calloc(nrows, sizeof(int *));
    if (matrix == nullptr) {
        int code = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating matrix, aborting.\n", code);
        MPI_Finalize();
        exit(code);
    }
    // let each pointer point to an element in `buf`
    for (int irow = 0; irow < nrows; irow++) {
        matrix[irow] = &buf[irow * ncols];
    }
    return matrix;
}


void matrix_free (int *** matrix) {
    if (matrix == nullptr) return;
    if (*matrix == nullptr) return;
    if (**matrix == nullptr) return;
    free(&(*matrix)[0][0]);
    free(&(*matrix)[0]);
    *matrix = nullptr;
}


void matrix_print (FILE * stream, int nrows, int ncols_chunk, int ncols_whole, const int * lengths, const int * offsets, const int ** matrix_chunk, int *** matrix_whole, MPI_Comm comm) {
    const int iroot = 0;
    int irank = -1;
    MPI_Comm_rank(comm, &irank);
    if (irank == iroot) {
        fprintf(stream, "LHS operand (matrix):\n");
    }
    for (int irow = 0; irow < nrows; irow++) {
        MPI_Gatherv(matrix_chunk[irow], ncols_chunk, MPI_INT, (*matrix_whole)[irow], lengths, offsets, MPI_INT, iroot, comm);
        if (irank == iroot) {
            for (int icol = 0; icol < ncols_whole; icol++) {
                fprintf(stream, "% 2d%c", (*matrix_whole)[irow][icol], icol < ncols_whole - 1 ? ' ' : '\n');
            }
        }
    }
}


void matrix_rand_init (FILE * stream, int nrows, int ncols, int *** matrix) {
    if (*matrix == nullptr || **matrix == nullptr) {
        int code = __LINE__;
        fprintf(stream, "ERROR %d: didn't expect matrix to be nullptr, aborting.\n", code);
        MPI_Finalize();
        exit(code);
    }
    // populate `matrix` with random integers 0..9
    for (int irow = 0; irow < nrows; irow++) {
        for (int icol = 0; icol < ncols; icol++) {
            (*matrix)[irow][icol] = rand() % 10;
        }
    }
}
