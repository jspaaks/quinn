#ifndef BLKDECOMP_BLKDECOMP_H_INCLUDED
#include <stddef.h>

size_t blkdecomp_get_first_elem (size_t nelems, size_t nranks, size_t irank);
size_t blkdecomp_get_last_elem (size_t nelems, size_t nranks, size_t irank);
size_t blkdecomp_get_block_size (size_t nelems, size_t nranks, size_t irank);
size_t blkdecomp_get_owner (size_t nelems, size_t nranks, size_t index);

#endif
