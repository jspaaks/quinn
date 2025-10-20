#include <stddef.h>
#include "blkdcmp/blkdcmp.h"
#include <criterion/criterion.h>

Test(blkdcmp_get_idx_blk_s, 100_4_0) {
    size_t actual = blkdcmp_get_idx_blk_s(100, 4, 0);
    size_t expected = 0;
    cr_assert(actual == expected);
}

Test(blkdcmp_get_idx_blk_s, 100_4_1) {
    size_t actual = blkdcmp_get_idx_blk_s(100, 4, 1);
    size_t expected = 25;
    cr_assert(actual == expected);
}

Test(blkdcmp_get_idx_blk_s, 100_4_2) {
    size_t actual = blkdcmp_get_idx_blk_s(100, 4, 2);
    size_t expected = 50;
    cr_assert(actual == expected);
}

Test(blkdcmp_get_idx_blk_s, 100_4_3) {
    size_t actual = blkdcmp_get_idx_blk_s(100, 4, 3);
    size_t expected = 75;
    cr_assert(actual == expected);
}

Test(blkdcmp_get_idx_blk_s, 10_4_0) {
    size_t actual = blkdcmp_get_idx_blk_s(10, 4, 0);
    size_t expected = 0;
    cr_assert(actual == expected);
}

Test(blkdcmp_get_idx_blk_s, 10_4_1) {
    size_t actual = blkdcmp_get_idx_blk_s(10, 4, 1);
    size_t expected = 2;
    cr_assert(actual == expected);
}

Test(blkdcmp_get_idx_blk_s, 10_4_2) {
    size_t actual = blkdcmp_get_idx_blk_s(10, 4, 2);
    size_t expected = 5;
    cr_assert(actual == expected);
}

Test(blkdcmp_get_idx_blk_s, 10_4_3) {
    size_t actual = blkdcmp_get_idx_blk_s(10, 4, 3);
    size_t expected = 7;
    cr_assert(actual == expected);
}
