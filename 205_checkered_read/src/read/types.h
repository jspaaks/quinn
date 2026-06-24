#ifndef READ_TYPES_INCLUDED
#define READ_TYPES_INCLUDED
#include <mpi.h>


#define NDIMS 2
#define CartTopo struct cart_topo
#define CartTopoElem struct cart_topo_elem
#define Matrix struct matrix


struct cart_topo {
    int nrows;
    int ncols;
    MPI_Comm comm;
};


struct cart_topo_elem {
    int irank;
    int coords[NDIMS];
};


struct matrix {
    struct {
        int nrows;
        int ncols;
        int ** data;
    } whole;
    struct {
        int nrows;
        int ncols;
        int ** data;
    } chunk;
    struct {
        int nelems;
        int * data;
    } rowbuf;
};

#endif
