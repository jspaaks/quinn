#include "blkdecomp/blkdecomp.h"

size_t blkdecomp_get_first_elem (size_t nelems, size_t nranks, size_t irank) {
    return irank * nelems / nranks;
}

size_t blkdecomp_get_last_elem (size_t nelems, size_t nranks, size_t irank) {
    return blkdecomp_get_first_elem(nelems, nranks, irank + 1) - 1;
}

size_t blkdecomp_get_block_size (size_t nelems, size_t nranks, size_t irank) {
    size_t first = blkdecomp_get_first_elem(nelems, nranks, irank);
    size_t last = blkdecomp_get_last_elem(nelems, nranks, irank);
    return last - first + 1;
}

size_t blkdecomp_get_owner (size_t nelems, size_t nranks, size_t index) {
    return (nranks * (index + 1) - 1) / nelems;
}
