#ifndef READ_TYPES_INCLUDED
#define READ_TYPES_INCLUDED
#include <mpi.h>


#define NDIMS 2
#define Topo struct topo
#define TopoElem struct topo_elem
#define Matrix struct matrix


struct topo {
    int nrows;
    int ncols;
    MPI_Comm comm;
};


struct topo_elem {
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
};


#endif
