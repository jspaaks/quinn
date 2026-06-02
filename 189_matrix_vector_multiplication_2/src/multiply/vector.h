#ifndef MULTIPLY_VECTOR_H_INCLUDED
#define MULTIPLY_VECTOR_H_INCLUDED

#include <mpi.h>
#include <stdio.h>

int * vector_calloc (int nelems);
void vector_free (int ** vector);
void vector_print (FILE * stream, int nelems_chunk, int nelems_whole, const int * lengths, const int * offsets, const int * vector_chunk, int ** vector_whole, MPI_Comm comm);
void vector_rand_init (FILE * stream, int nelems, int ** vector);

#endif
