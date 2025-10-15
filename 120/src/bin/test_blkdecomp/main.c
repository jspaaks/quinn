#include <stddef.h>
#include "blkdecomp/blkdecomp.h"
#include <criterion/criterion.h>

Test(test_blkdecomp, get_first_elem_100_4_0) {
    size_t actual = blkdecomp_get_first_elem(100, 4, 0);
    size_t expected = 0;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_first_elem_100_4_1) {
    size_t actual = blkdecomp_get_first_elem(100, 4, 1);
    size_t expected = 25;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_first_elem_100_4_2) {
    size_t actual = blkdecomp_get_first_elem(100, 4, 2);
    size_t expected = 50;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_first_elem_100_4_3) {
    size_t actual = blkdecomp_get_first_elem(100, 4, 3);
    size_t expected = 75;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_first_elem_10_4_0) {
    size_t actual = blkdecomp_get_first_elem(10, 4, 0);
    size_t expected = 0;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_first_elem_10_4_1) {
    size_t actual = blkdecomp_get_first_elem(10, 4, 1);
    size_t expected = 2;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_first_elem_10_4_2) {
    size_t actual = blkdecomp_get_first_elem(10, 4, 2);
    size_t expected = 5;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_first_elem_10_4_3) {
    size_t actual = blkdecomp_get_first_elem(10, 4, 3);
    size_t expected = 7;
    cr_assert(actual == expected);
}


Test(test_blkdecomp, get_last_elem_100_4_0) {
    size_t actual = blkdecomp_get_last_elem(100, 4, 0);
    size_t expected = 24;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_last_elem_100_4_1) {
    size_t actual = blkdecomp_get_last_elem(100, 4, 1);
    size_t expected = 49;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_last_elem_100_4_2) {
    size_t actual = blkdecomp_get_last_elem(100, 4, 2);
    size_t expected = 74;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_last_elem_100_4_3) {
    size_t actual = blkdecomp_get_last_elem(100, 4, 3);
    size_t expected = 99;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_last_elem_10_4_0) {
    size_t actual = blkdecomp_get_last_elem(10, 4, 0);
    size_t expected = 1;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_last_elem_10_4_1) {
    size_t actual = blkdecomp_get_last_elem(10, 4, 1);
    size_t expected = 4;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_last_elem_10_4_2) {
    size_t actual = blkdecomp_get_last_elem(10, 4, 2);
    size_t expected = 6;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_last_elem_10_4_3) {
    size_t actual = blkdecomp_get_last_elem(10, 4, 3);
    size_t expected = 9;
    cr_assert(actual == expected);
}


Test(test_blkdecomp, get_block_size_100_4_0) {
    size_t actual = blkdecomp_get_block_size(100, 4, 0);
    size_t expected = 25;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_block_size_100_4_1) {
    size_t actual = blkdecomp_get_block_size(100, 4, 1);
    size_t expected = 25;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_block_size_100_4_2) {
    size_t actual = blkdecomp_get_block_size(100, 4, 2);
    size_t expected = 25;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_block_size_100_4_3) {
    size_t actual = blkdecomp_get_block_size(100, 4, 3);
    size_t expected = 25;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_block_size_10_4_0) {
    size_t actual = blkdecomp_get_block_size(10, 4, 0);
    size_t expected = 2;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_block_size_10_4_1) {
    size_t actual = blkdecomp_get_block_size(10, 4, 1);
    size_t expected = 3;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_block_size_10_4_2) {
    size_t actual = blkdecomp_get_block_size(10, 4, 2);
    size_t expected = 2;
    cr_assert(actual == expected);
}

Test(test_blkdecomp, get_block_size_10_4_3) {
    size_t actual = blkdecomp_get_block_size(10, 4, 3);
    size_t expected = 3;
    cr_assert(actual == expected);
}



Test(test_blkdecomp, get_block_owner_100_4) {

    {
        size_t expected = 0;
        for (size_t i = 0; i < 25; i++) {
            size_t actual = blkdecomp_get_owner(100, 4, i);
            cr_assert(expected == actual);
        }
    }

    {
        size_t expected = 1;
        for (size_t i = 25; i < 50; i++) {
            size_t actual = blkdecomp_get_owner(100, 4, i);
            cr_assert(expected == actual);
        }
    }

    {
        size_t expected = 2;
        for (size_t i = 50; i < 75; i++) {
            size_t actual = blkdecomp_get_owner(100, 4, i);
            cr_assert(expected == actual);
        }
    }

    {
        size_t expected = 3;
        for (size_t i = 75; i < 100; i++) {
            size_t actual = blkdecomp_get_owner(100, 4, i);
            cr_assert(expected == actual);
        }
    }

}


Test(test_blkdecomp, get_block_owner_10_4) {

    {
        size_t expected = 0;
        for (size_t i = 0; i < 2; i++) {
            size_t actual = blkdecomp_get_owner(10, 4, i);
            cr_assert(expected == actual);
        }
    }

    {
        size_t expected = 1;
        for (size_t i = 2; i < 5; i++) {
            size_t actual = blkdecomp_get_owner(10, 4, i);
            cr_assert(expected == actual);
        }
    }

    {
        size_t expected = 2;
        for (size_t i = 5; i < 7; i++) {
            size_t actual = blkdecomp_get_owner(10, 4, i);
            cr_assert(expected == actual);
        }
    }

    {
        size_t expected = 3;
        for (size_t i = 7; i < 10; i++) {
            size_t actual = blkdecomp_get_owner(10, 4, i);
            cr_assert(expected == actual);
        }
    }

}
