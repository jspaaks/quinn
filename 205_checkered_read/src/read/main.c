#include "blkdcmp/blkdcmp.h"
#include "types.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int matrix_calloc (int nrows, int ncols, int *** data, int errcode);
void matrix_free (int *** data);
void matrix_init (int nrows, int ncols, int *** data);
int vector_calloc (int nelems, int ** data, int errcode);
void vector_free (int ** data);


int matrix_calloc (int nrows, int ncols, int *** data, int errcode) {
    // allocate contiguous buffer
    int * buf = calloc(nrows * ncols, sizeof(int));
    if (buf == nullptr) {
        fprintf(stderr, "ERROR %d: Could not allocate memory for matrix data buffer, aborting.\n", errcode);
        return errcode;
    }
    // allocate array of pointers
    *data = calloc(nrows, sizeof(int *));
    if (*data == nullptr) {
        fprintf(stderr, "ERROR %d: Could not allocate memory for data, aborting.\n", errcode);
        return errcode;
    }
    // let *data point to elements of buf
    for (int irow = 0; irow < nrows; irow++) {
        (*data)[irow] = &buf[irow * ncols];
    }
    return 0;
}


void matrix_free (int *** data) {
    if (*data == nullptr) return;
    if ((*data)[0] == nullptr) return;
    free(&(*data)[0][0]);
    free(&(*data)[0]);
    *data = nullptr;
}


void matrix_init (int nrows, int ncols, int *** data) {
    // initialize the pseudorandom number generator with the current time
    srand(time(nullptr));

    // assign random integers [0..9] to elements of *data
    for (int irow = 0; irow < nrows; irow++) {
        for (int icol = 0; icol < ncols; icol++) {
            (*data)[irow][icol] = rand() % 10;
        }
    }
}


int main (int argc, char * argv[]) {
    int errcode = EXIT_SUCCESS;

    // initialize mpi
    MPI_Init(&argc, &argv);

    // conditionally trap the debugger if a compile time variable has been defined
#ifdef READ_TRAP_DBG
    {
        volatile bool iswaiting = true;
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        fprintf(stdout, "PID %d on %s waiting for debugger attach...\n", getpid(), hostname);
        fflush(stdout);
        while (iswaiting) {
            // attach the debugger with gdb --pid <the pid>; once attached the debugger will halt
            // the program here; use GDB commands to set iswaiting to false, e.g.
            // (gdb) set var iswaiting = 0;
            sleep(3);
        }
    }
#endif // READ_TRAP_DBG

    // declare some variables
    Matrix matrix = {};
    int * rowbuf = nullptr;
    int * lengths = nullptr;
    int * offsets = nullptr;

    // arbitrariliy set matrix dimensions to some values
    // TODO read dimensions from argument list argv
    matrix.whole.nrows = 7;
    matrix.whole.ncols = 11;

    // create the Cartesian communicator
    Topo grid = {};
    {
        int dims[NDIMS] = {0, 0};
        int isperiodic[NDIMS] = {0, 0};
        int nnodes = -1;
        int reorder = 0;
        MPI_Comm_size(MPI_COMM_WORLD, &nnodes);
        MPI_Dims_create(nnodes, NDIMS, dims);
        MPI_Cart_create(MPI_COMM_WORLD, NDIMS, dims, isperiodic, reorder, &grid.comm);
        grid.nrows = dims[0];
        grid.ncols = dims[1];
    }

    // get your own rank in the Cartesian communicator
    TopoElem me = {.irank = -1, .coords = {-1, -1}};
    MPI_Comm_rank(grid.comm, &me.irank);

    // get the coordinates of this process in the Cartesian communicator
    MPI_Cart_coords(grid.comm, me.irank, NDIMS, me.coords);

    // create a subset of grid.comm containing only topoelems that are in the same row
    Topo row = {.nrows = 1, .ncols = grid.ncols};
    MPI_Comm_split(grid.comm, me.coords[0], me.coords[1], &row.comm);

    // print some diagnostic information about the topology
    if (me.coords[0] == 0 && me.coords[1] == 0) {
        fprintf(stdout, "(%d,%d) topology: %dx%d\n", me.coords[0], me.coords[1], grid.nrows, grid.ncols);
    }

    // instead of reading from file, just initialize a 2-D array with random integers, then read
    // row by row later on as if you're reading
    if (me.coords[0] == 0 && me.coords[1] == 0) {
        if (matrix_calloc(matrix.whole.nrows, matrix.whole.ncols, &matrix.whole.data, __LINE__)) goto cleanup;
        matrix_init(matrix.whole.nrows, matrix.whole.ncols, &matrix.whole.data);
    }

    // allocate space for each process' chunk of the data
    {
        matrix.chunk.nrows = blkdcmp_get_blk_sz(matrix.whole.nrows, grid.nrows, me.coords[0]);
        matrix.chunk.ncols = blkdcmp_get_blk_sz(matrix.whole.ncols, grid.ncols, me.coords[1]);
        fprintf(stdout, "(%d,%d) has submatrix of size %dx%d\n", me.coords[0], me.coords[1], matrix.chunk.nrows, matrix.chunk.ncols);
        if (matrix_calloc(matrix.chunk.nrows, matrix.chunk.ncols, &matrix.chunk.data, __LINE__)) goto cleanup;
    }

    // now for the distribution of data using a tree-like data distribution scheme
    if (me.coords[0] == 0 && me.coords[1] == 0) {
        // if this process is the top left one in the Cartesian topology:
        // while there are rows in matrix.whole.data
        // find which process should receive it using blkdcmp_get_blk_owner
        // send whole rows there one at a time using MPI_Send()
        for (int irow = 0; irow < matrix.whole.nrows; irow++) {
            TopoElem dst = {
                .irank = -1,
                .coords = {
                    blkdcmp_get_blk_owner(matrix.whole.nrows, grid.nrows, irow),
                    0,
                },
            };
            MPI_Cart_rank(grid.comm, dst.coords, &dst.irank);
            MPI_Send(matrix.whole.data[irow], matrix.whole.ncols, MPI_INT, dst.irank, 0, grid.comm);
        }
    }
    if (me.coords[1] == 0) {
        // if this process is in the first column of the Cartesian topology,
        // allocate an extra buffer to receive a row of data
        if (me.coords[1] == 0) {
            if (vector_calloc(matrix.whole.ncols, &rowbuf, __LINE__)) goto cleanup;
        }

        // determine the lengths array needed for scatterving across a row
        blkdcmp_lengths_calloc(stderr, row.ncols, &lengths);
        blkdcmp_lengths_init(stderr, matrix.whole.ncols, row.ncols, &lengths);

        // determine the offsets array needed for scatterving across a row
        blkdcmp_offsets_calloc(stderr, row.ncols, &offsets);
        blkdcmp_offsets_init(stderr, row.ncols, (const int *) lengths, &offsets);
     
        // determine from which rank this process is expected to receive data
        TopoElem src = {
            .irank = -1,
            .coords = {0, 0},
        };
        MPI_Cart_rank(grid.comm, src.coords, &src.irank);

        for (int irow = 0; irow < matrix.chunk.nrows; irow++) {
            // receive matrix.chunk.nrows rows from src.irank using MPI_Recv()
            MPI_Status status = {};
            MPI_Recv(rowbuf, matrix.whole.ncols, MPI_INT, src.irank, 0, grid.comm, &status);

            // use the result of an MPI_Split to scatter the row to other processes in my row
            MPI_Scatterv(rowbuf, lengths, offsets, MPI_INT, matrix.chunk.data[irow], matrix.chunk.ncols, MPI_INT, 0, row.comm);
        }
    }

    MPI_Barrier(grid.comm);

    for (int irow = 0; irow < matrix.chunk.nrows; irow++) {
        fprintf(stdout, "%d,%d: ", me.coords[0], me.coords[1]);
        for (int icol = 0; icol < matrix.chunk.ncols; icol++) {
            fprintf(stdout, "%d%c", matrix.chunk.data[irow][icol], icol < matrix.chunk.ncols - 1 ? ' ' : '\n');
        }
    }


cleanup:

    // release dynamic memory associated with offsets array
    blkdcmp_offsets_free(&offsets);

    // release dynamic memory associated with lengths array
    blkdcmp_lengths_free(&lengths);

    // release dynamic memory associated with matrix.whole.data
    matrix_free(&matrix.whole.data);

    // release dynamic memory associated with matrix.chunk.data
    matrix_free(&matrix.chunk.data);

    // release dynamic memory associated with rowbuf
    vector_free(&rowbuf);

    // free the communicators
    MPI_Comm_free(&row.comm);
    MPI_Comm_free(&grid.comm);

    // terminate the MPI execution environment
    MPI_Finalize();

    return errcode;
}


int vector_calloc (int nelems, int ** data, int errcode) {
    // allocate contiguous buffer
    *data = calloc(nelems, sizeof(int));
    if (*data == nullptr) {
        fprintf(stderr, "ERROR %d: Could not allocate memory for vector data buffer, aborting.\n", errcode);
        return errcode;
    }
    return 0;
}


void vector_free (int ** data) {
    if (*data == nullptr) return;
    free(&(*data)[0]);
    *data = nullptr;
}
