#ifndef MULTIPLY_LENGTHS_INCLUDED
#define MULTIPLY_LENGTHS_INCLUDED
#include <stdio.h>


int * lengths_calloc (FILE * stream, int nranks);
void lengths_free (int ** lengths);
void lengths_init (int nelems, int nranks, int ** lengths);

#endif

