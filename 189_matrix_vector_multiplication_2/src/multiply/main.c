#include "blkdcmp/blkdcmp.h"
#include "cla/cla.h"
#include "dual_types.h"
#include "lengths.h"
#include "offsets.h"
#include "matrix.h"
#include "vector.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef MULTIPLY_TRAP_DBG
#include <unistd.h>              // gethostname, getpid, sleep
#endif // MULTIPLY_TRAP_DBG


static void handle_user_input (int * nrows, struct dual_int * ncols, int irank, int nranks, int argc, char ** argv);
static void multiply (int nrows, int ncols, const int ** matrix, const int * vector, int ** result);
static int parse_ncols (struct cla * cla);
static int parse_nrows (struct cla * cla);
static void provide_seeds (int irank, int nranks, MPI_Comm comm);
static void reduce (int nrows, const int * result_chunk, int ** result_whole, MPI_Comm comm);
static void result_print (FILE * stream, int nelems, const int * result_whole, MPI_Comm comm);
static void show_usage (FILE * stream, struct cla * cla);


static void handle_user_input (int * nrows, struct dual_int * ncols, int irank, int nranks, int argc, char ** argv) {
    struct cla * cla = CLA_create();
    CLA_add_required(cla, "--nrows", "-r");
    CLA_add_required(cla, "--ncols", "-c");
    CLA_parse(cla, argc, (const char **) argv);
    if (CLA_help_requested(cla)) {
        if (irank == 0) {
            show_usage(stdout, cla);
            fflush(stdout);
        }
        CLA_destroy(&cla);
        MPI_Finalize();
        exit(EXIT_SUCCESS);
    }
    *nrows = parse_nrows(cla);
    int tmp = parse_ncols(cla);
    *ncols = (struct dual_int) {
        .whole = tmp,
        .chunk = blkdcmp_get_blk_sz(tmp, nranks, irank)
    };
    CLA_destroy(&cla);
}


int main (int argc, char * argv[]) {

    int irank = -1;
    int nranks = -1;

    // initialize mpi
    MPI_Init(&argc, &argv);

    // conditionally trap the debugger if a compile time variable has been defined
#ifdef MULTIPLY_TRAP_DBG
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
#endif // MULTIPLY_TRAP_DBG

    // get your own rank and the total number of ranks
    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    // get the number of rows and the number of columns from the user input
    struct dual_int ncols;
    int nrows;
    handle_user_input(&nrows, &ncols, irank, nranks, argc, argv);

    // calculate the array of lengths given the number of ranks
    int * lengths = lengths_calloc(stderr, nranks);
    lengths_init(ncols.whole, nranks, &lengths);

    // calculate the array of offsets given the number of ranks
    int * offsets = offsets_calloc(stderr, nranks);
    offsets_init(nranks, lengths, &offsets);

    // seed each process
    provide_seeds(irank, nranks, MPI_COMM_WORLD);

    // allocate memory space for each process' chunk of the matrix, and the whole matrix if you're rank 0
    struct dual_intpp matrix;
    matrix.chunk = matrix_calloc(nrows, ncols.chunk);
    if (irank == 0) {
        matrix.whole = matrix_calloc(nrows, ncols.whole);
    } else {
        matrix.whole = matrix_calloc(nrows, 1);  // to avoid problems with MPI_Gatherv when printing
    }

    // allocate memory space for each process' chunk of the vector, and the whole vector if you're rank 0
    struct dual_intp vector;    
    vector.chunk = vector_calloc(ncols.chunk);
    if (irank == 0) {
        vector.whole = vector_calloc(ncols.whole);
    } else {
        vector.whole = vector_calloc(1);  // to avoid problems with MPI_Gatherv when printing
    }

    // allocate memory space for each process' (whole) result, and the reduced result if you're rank 0
    struct dual_intp result;
    result.chunk = vector_calloc(nrows);
    if (irank == 0) {
        result.whole = vector_calloc(nrows);
    } else { 
        result.whole = vector_calloc(1);  // to avoid problems with MPI_Gatherv when printing
    }

    // initialize the matrix with random integers 0..9
    matrix_rand_init(stderr, nrows, ncols.chunk, &matrix.chunk);

    // print the distributed matrix
    matrix_print(stdout, nrows, ncols.chunk, ncols.whole, (const int *) lengths, (const int *) offsets, (const int **) matrix.chunk, &matrix.whole, MPI_COMM_WORLD);

    // initialize the vector with random integers 0..9
    vector_rand_init(stderr, ncols.chunk, &vector.chunk);

    // print the distributed vector
    vector_print(stdout, ncols.chunk, ncols.whole, (const int *) lengths, (const int *) offsets, (const int *) vector.chunk, &vector.whole, MPI_COMM_WORLD);

    // multiply each process' chunk of matrix with its chunk of the vector to get the result unique to this process
    multiply(nrows, ncols.chunk, (const int **) matrix.chunk, (const int *) vector.chunk, &result.chunk);

    // reduce the partial results
    reduce(nrows, (const int *) result.chunk, &result.whole, MPI_COMM_WORLD);

    // print the distributed vector
    result_print(stdout, nrows, (const int *) result.whole, MPI_COMM_WORLD);

    // free the memory resources related to the matrix
    matrix_free(&matrix.chunk);
    matrix_free(&matrix.whole);

    // free the memory resources related to the vector
    vector_free(&vector.chunk);
    vector_free(&vector.whole);

    // free the memory resources related to the result
    vector_free(&result.chunk);
    vector_free(&result.whole);

    // free the memory resources associated with the lengths array
    lengths_free(&lengths);

    // free the memory resources associated with the offsets array
    offsets_free(&offsets);

    // terminate the MPI execution environment
    MPI_Finalize();

    return EXIT_SUCCESS;
}


static void multiply (int nrows, int ncols, const int ** matrix, const int * vector, int ** result) {
    for (int irow = 0; irow < nrows; irow++) {
        for (int icol = 0; icol < ncols; icol++) {
            (*result)[irow] += matrix[irow][icol] * vector[icol];
        }
    }
}


static int parse_ncols (struct cla * cla) {
    const char * s = CLA_get_value_required(cla, "--ncols");
    return atoi(s);
}


static int parse_nrows (struct cla * cla) {
    const char * s = CLA_get_value_required(cla, "--nrows");
    return atoi(s);
}


static void provide_seeds (int irank, int nranks, MPI_Comm comm) {
    int seed = -1;
    if (irank == 0) {
        // use the current time to set the pseudorandom number generator for rank 0
        srand(time(nullptr));
        // send the other processes their pseudorandom number generator seed
        for (int itgt = 1; itgt < nranks; itgt++) {
            seed = rand();
            MPI_Send(&seed, 1, MPI_INT, itgt, 0, comm);
        }
    } else {
        MPI_Status status = {};
        // receive the pseudorandom number generator seed and use it
        MPI_Recv(&seed, 1, MPI_INT, 0, 0, comm, &status);
        srand(seed);
    }
}


static void reduce (int nrows, const int * result_chunk, int ** result_whole, MPI_Comm comm) {
    for (int irow = 0; irow < nrows; irow++) {
        MPI_Reduce(&result_chunk[irow], &(*result_whole)[irow], 1, MPI_INT, MPI_SUM, 0, comm);
    }
}


static void result_print (FILE * stream, int nelems, const int * result_whole, MPI_Comm comm) {
    const int iroot = 0;
    int irank = -1;
    MPI_Comm_rank(comm, &irank);
    if (irank == iroot) {
        fprintf(stream, "result (column vector):\n");
        for (int ielem = 0; ielem < nelems; ielem++) {
            fprintf(stream, "% 6d\n", result_whole[ielem]);
        }
    }
}


static void show_usage (FILE * stream, struct cla * cla) {
    fprintf(stream,
            "Usage mpirun -np NP %s [REQUIREDS]\n"
            "\n"
            "  Matrix-vector multiplication using NP processes (column-striped matrix implementation).\n"
            "\n"
            "  The matrix is the left hand side operand; the vector is the right hand side\n"
            "  operand. The program will initialize the operands with random integers 0..9,\n"
            "  calculate the result, then print both operands and the resulting vector.\n"
            "\n"
            "  Required arguments\n"
            "    --ncols, -c   Number of columns in the matrix (number of elements in the vector).\n"
            "    --nrows, -r   Number of rows in the matrix.\n"
            "\n",
            CLA_get_exename(cla));
}
