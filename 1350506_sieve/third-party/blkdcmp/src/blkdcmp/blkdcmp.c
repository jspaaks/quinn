#include "blkdcmp/blkdcmp.h"

size_t blkdcmp_get_idx_blk_s (size_t nelems, size_t nranks, size_t irank) {
    return irank * nelems / nranks;
}

size_t blkdcmp_get_idx_blk_e (size_t nelems, size_t nranks, size_t irank) {
    return blkdcmp_get_idx_blk_s(nelems, nranks, irank + 1) - 1;
}

size_t blkdcmp_get_blk_sz (size_t nelems, size_t nranks, size_t irank) {
    size_t start = blkdcmp_get_idx_blk_s(nelems, nranks, irank);
    size_t end = blkdcmp_get_idx_blk_e(nelems, nranks, irank);
    return end - start + 1;
}

size_t blkdcmp_get_blk_owner (size_t nelems, size_t nranks, size_t index) {
    return (nranks * (index + 1) - 1) / nelems;
}
