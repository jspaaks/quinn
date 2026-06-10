#ifndef CART_CREATE_DUAL_TYPES_INCLUDED
#define CART_CREATE_DUAL_TYPES_INCLUDED

struct dual_int {
    int whole;
    int chunk;
};

struct dual_intp {
    int * whole;
    int * chunk;
};

#endif
