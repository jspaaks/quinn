#include "lengths.h"
#include "blkdcmp/blkdcmp.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


int * lengths_calloc (FILE * stream, int nranks) {
    int * lengths = calloc(nranks, sizeof(int));
    if (lengths == nullptr) {
        const int code = __LINE__;
        fprintf(stream, "ERROR %d: encountered problem while allocating memory for lengths array, aborting.\n", code);
        MPI_Finalize();
        exit(code);
    }
    return lengths;
}


void lengths_free(int ** lengths) {
    free(*lengths);
    *lengths = nullptr;
}


void lengths_init (int nelems, int nranks, int ** lengths) {
    for (int irank = 0; irank < nranks; irank++) {
        (*lengths)[irank] = blkdcmp_get_blk_sz(nelems, nranks, irank);
    }
}
