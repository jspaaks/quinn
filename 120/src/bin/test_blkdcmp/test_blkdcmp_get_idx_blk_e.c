#include <stddef.h>
#include "blkdcmp/blkdcmp.h"
#include <criterion/criterion.h>

Test(blkdcmp_get_idx_blk_e, 100_4_0) {
    size_t actual = blkdcmp_get_idx_blk_e(100, 4, 0);
    size_t expected = 24;
    cr_assert(actual == expected);
}

Test(blkdcmp_get_idx_blk_e, 100_4_1) {
    size_t actual = blkdcmp_get_idx_blk_e(100, 4, 1);
    size_t expected = 49;
    cr_assert(actual == expected);
}

Test(blkdcmp_get_idx_blk_e, 100_4_2) {
    size_t actual = blkdcmp_get_idx_blk_e(100, 4, 2);
    size_t expected = 74;
    cr_assert(actual == expected);
}

Test(blkdcmp_get_idx_blk_e, 100_4_3) {
    size_t actual = blkdcmp_get_idx_blk_e(100, 4, 3);
    size_t expected = 99;
    cr_assert(actual == expected);
}

Test(blkdcmp_get_idx_blk_e, 10_4_0) {
    size_t actual = blkdcmp_get_idx_blk_e(10, 4, 0);
    size_t expected = 1;
    cr_assert(actual == expected);
}

Test(blkdcmp_get_idx_blk_e, 10_4_1) {
    size_t actual = blkdcmp_get_idx_blk_e(10, 4, 1);
    size_t expected = 4;
    cr_assert(actual == expected);
}

Test(blkdcmp_get_idx_blk_e, 10_4_2) {
    size_t actual = blkdcmp_get_idx_blk_e(10, 4, 2);
    size_t expected = 6;
    cr_assert(actual == expected);
}

Test(blkdcmp_get_idx_blk_e, 10_4_3) {
    size_t actual = blkdcmp_get_idx_blk_e(10, 4, 3);
    size_t expected = 9;
    cr_assert(actual == expected);
}
