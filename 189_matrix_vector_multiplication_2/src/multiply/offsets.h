#ifndef MULTIPLY_OFFSETS_INCLUDED
#define MULTIPLY_OFFSETS_INCLUDED
#include <stdio.h>


int * offsets_calloc (FILE * stream, int nranks);
void offsets_free (int ** offsets);
void offsets_init (int nranks, int * lengths, int ** offsets);

#endif

