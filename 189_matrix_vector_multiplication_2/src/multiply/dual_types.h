#ifndef MULTIPLY_DUAL_TYPES_INCLUDED
#define MULTIPLY_DUAL_TYPES_INCLUDED

struct dual_int {
    int whole;
    int chunk;
};

struct dual_intp {
    int * whole;
    int * chunk;
};

struct dual_intpp {
    int ** whole;
    int ** chunk;
};

#endif
