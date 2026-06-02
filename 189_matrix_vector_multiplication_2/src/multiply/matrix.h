#ifndef MULTIPLY_MATRIX_H_INCLUDED
#define MULTIPLY_MATRIX_H_INCLUDED
#include <mpi.h>
#include <stdio.h>


int ** matrix_calloc (int nrows, int ncols);
void matrix_free (int *** matrix);
void matrix_print (FILE * stream, int nrows, int ncols_chunk, int ncols_whole, const int * lengths, const int * offsets, const int ** matrix_chunk, int *** matrix_whole, MPI_Comm comm);
void matrix_rand_init (FILE * stream, int nrows, int ncols, int *** matrix);

#endif
