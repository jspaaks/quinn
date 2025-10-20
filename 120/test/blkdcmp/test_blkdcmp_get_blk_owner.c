#include <stddef.h>
#include "blkdcmp/blkdcmp.h"
#include <criterion/criterion.h>


Test(blkdcmp_get_blk_owner, 100_4) {

    {
        size_t expected = 0;
        for (size_t i = 0; i < 25; i++) {
            size_t actual = blkdcmp_get_blk_owner(100, 4, i);
            cr_assert(expected == actual);
        }
    }

    {
        size_t expected = 1;
        for (size_t i = 25; i < 50; i++) {
            size_t actual = blkdcmp_get_blk_owner(100, 4, i);
            cr_assert(expected == actual);
        }
    }

    {
        size_t expected = 2;
        for (size_t i = 50; i < 75; i++) {
            size_t actual = blkdcmp_get_blk_owner(100, 4, i);
            cr_assert(expected == actual);
        }
    }

    {
        size_t expected = 3;
        for (size_t i = 75; i < 100; i++) {
            size_t actual = blkdcmp_get_blk_owner(100, 4, i);
            cr_assert(expected == actual);
        }
    }

}


Test(blkdcmp_get_blk_owner, 10_4) {

    {
        size_t expected = 0;
        for (size_t i = 0; i < 2; i++) {
            size_t actual = blkdcmp_get_blk_owner(10, 4, i);
            cr_assert(expected == actual);
        }
    }

    {
        size_t expected = 1;
        for (size_t i = 2; i < 5; i++) {
            size_t actual = blkdcmp_get_blk_owner(10, 4, i);
            cr_assert(expected == actual);
        }
    }

    {
        size_t expected = 2;
        for (size_t i = 5; i < 7; i++) {
            size_t actual = blkdcmp_get_blk_owner(10, 4, i);
            cr_assert(expected == actual);
        }
    }

    {
        size_t expected = 3;
        for (size_t i = 7; i < 10; i++) {
            size_t actual = blkdcmp_get_blk_owner(10, 4, i);
            cr_assert(expected == actual);
        }
    }

}
