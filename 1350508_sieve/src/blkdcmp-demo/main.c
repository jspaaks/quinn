#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "blkdcmp/blkdcmp.h"

int val2idx (int val) {
    return (val - 3) / 2;  // truncate
}

int idx2val (int idx) {
    return idx * 2 + 3;
}

int main (int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr,
                "Usage: blkdcmp-demo N NRANKS\n"
                "    Illustrate the partitioning of [3,5,..,N] over NRANKS.\n");
        return EXIT_FAILURE;
    }

    // get the user input
    int n = atoi(argv[1]);
    int nranks = atoi(argv[2]);
    int m = val2idx(n);

    if (n < 3) {
        fprintf(stderr, "N needs to be at least 3, aborting\n");
        return EXIT_FAILURE;
    }

    if (nranks > m + 1) {
        fprintf(stderr, "Too many processes, aborting\n");
        return EXIT_FAILURE;
    }

    fprintf(stdout, "idx      value    owner\n");
    for (int idx = 0; idx <= m; idx++) {
        int val = idx2val(idx);
        int owner = (int) blkdcmp_get_blk_owner(m+1, nranks, idx);
        fprintf(stdout, " %8d %8d %8d\n", idx, val, owner);
    }

    fprintf(stdout, "---------------------------\n\n");

    for (int irank = 0; irank < nranks; irank++) {
        int blk_s = blkdcmp_get_idx_blk_s(m+1, nranks, irank);
        int blk_e = blkdcmp_get_idx_blk_e(m+1, nranks, irank);
        int blk_sz = blkdcmp_get_blk_sz(m+1, nranks, irank);
        fprintf(stdout, "%d: %d..%d (%d)\n", irank, blk_s, blk_e, blk_sz);
    }

    return EXIT_SUCCESS;
}
