#include "offsets.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


int * offsets_calloc (FILE * stream, int nranks) {
    int * offsets = calloc(nranks, sizeof(int));
    if (offsets == nullptr) {
        const int code = __LINE__;
        fprintf(stream, "ERROR %d: encountered problem while allocating memory for offsets array, aborting.\n", code);
        MPI_Finalize();
        exit(code);
    }
    return offsets;
}


void offsets_free(int ** offsets) {
    free(*offsets);
    *offsets = nullptr;
}


void offsets_init (int nranks, int * lengths, int ** offsets) {
    int length = 0;
    for (int irank = 0; irank < nranks; irank++) {
        (*offsets)[irank] = length;
        length += lengths[irank];
    }
}
