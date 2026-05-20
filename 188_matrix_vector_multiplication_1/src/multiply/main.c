#include "blkdcmp/blkdcmp.h"     // blkdcmp_*
#include "cla/cla.h"             // CLA_*
#include <mpi.h>                 // MPI_*
#include <stdio.h>               // fprintf, fflush, FILE
#include <stdlib.h>              // EXIT_SUCCESS, rand, srand
#include <time.h>                // time
#ifdef MULTIPLY_TRAP_DBG
#include <unistd.h>              // gethostname, getpid, sleep
#endif // MULTIPLY_TRAP_DBG

struct dual_int {
    int lcl;
    int gbl;
};

struct dual_intp {
    int * lcl;
    int * gbl;
};

struct dual_intpp {
    int ** lcl;
    int ** gbl;
};


static void handle_user_input (struct dual_int * nrows, int * ncols, int irank, int nranks, int argc, char ** argv);
static void matrix_calloc (int nrows, int ncols, int *** matrix);
static void matrix_free (int *** matrix);
static void matrix_gatherv (struct dual_int nrows, int ncols, const int ** matrix_lcl, int *** matrix_gbl, int irank, int nranks, MPI_Comm comm);
static void matrix_print (int nrows, int ncols, const int ** matrix);
static void matrix_rand_init (int nrows, int ncols, int *** matrix);
static void multiply (int nrows, int ncols, const int ** lhop, const int * rhop, int ** result);
static int parse_ncols (struct cla * cla);
static int parse_nrows (struct cla * cla);
static void provide_seeds (int irank, int nranks, MPI_Comm comm);
static void result_gatherv (int nelems_lcl, const int * result_lcl, int nelems_gbl, int ** result_gbl, int irank, int nranks, MPI_Comm comm);
static void show_usage (FILE * stream, struct cla * cla);
static void vector_bcast (int nelems, int ** vector, MPI_Comm comm);
static void vector_calloc (int nelems, int ** vector);
static void vector_free (int ** vector);
static void vector_print (int nelems, const int * vector);
static void vector_rand_init (int nelems, int ** vector, int irank);


static int code = 0;


static void handle_user_input (struct dual_int * nrows, int * ncols, int irank, int nranks, int argc, char ** argv) {
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
    int nrows_gbl = parse_nrows(cla);
    *nrows = (struct dual_int) {
        .gbl = nrows_gbl,
        .lcl = blkdcmp_get_blk_sz(nrows_gbl, nranks, irank)
    };
    *ncols = parse_ncols(cla);
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

    struct dual_int nrows = {
        .lcl = -1,
        .gbl = -1,
    };

    int ncols = -1;

    struct dual_intpp lhop = {
        .lcl = nullptr,
        .gbl = nullptr,
    };

    int * rhop = nullptr;

    struct dual_intp result = {
        .lcl = nullptr,
        .gbl = nullptr,
    };

    // get the number of rows and number of columns from the user's input
    handle_user_input(&nrows, &ncols, irank, nranks, argc, argv);

    // ensure that each process gets their own pseudorandom number generator seed
    provide_seeds(irank, nranks, MPI_COMM_WORLD);

    // allocate space to accomodate the left operand, right operand, and the result
    matrix_calloc(nrows.lcl, ncols, &lhop.lcl);
    vector_calloc(ncols, &rhop);
    vector_calloc(nrows.lcl, &result.lcl);
    if (irank == 0) {
        vector_calloc(nrows.gbl, &result.gbl);
        matrix_calloc(nrows.gbl, ncols, &lhop.gbl);
    } else {
        // minimal 2-d placeholder array so I can use it as argument to MPI_gatherv later
        matrix_calloc(1, 1, &lhop.gbl);
    }

    // initialize the right operand and left operand
    matrix_rand_init(nrows.lcl, ncols, &lhop.lcl);
    vector_rand_init(ncols, &rhop, irank);

    // have rank 0 broadcast the vector to the other processes
    vector_bcast(ncols, &rhop, MPI_COMM_WORLD);

    // multiply the operands, store the result in `result`
    multiply(nrows.lcl, ncols, (const int **) lhop.lcl, (const int *) rhop, &result.lcl);

    // use rank 0 to gather the distributed slices into result.gbl
    result_gatherv(nrows.lcl, (const int *) result.lcl, nrows.gbl, &result.gbl, irank, nranks, MPI_COMM_WORLD);
    matrix_gatherv(nrows, ncols, (const int **) lhop.lcl, &lhop.gbl, irank, nranks, MPI_COMM_WORLD);

    // print the result
    if (irank == 0) {
        fprintf(stdout, "matrix (left hand operand):\n");
        matrix_print(nrows.gbl, ncols, (const int **) lhop.gbl);
        fprintf(stdout, "vector (right hand operand):\n");
        vector_print(ncols, (const int *) rhop);
        fprintf(stdout, "vector (result):\n");
        vector_print(nrows.gbl, (const int *) result.gbl);
    }

    // release the memory resources associated with the operands and the result
    vector_free(&result.gbl);
    vector_free(&result.lcl);
    vector_free(&rhop);
    matrix_free(&lhop.lcl);
    matrix_free(&lhop.gbl);

    // terminate the MPI execution environment
    MPI_Finalize();

    return EXIT_SUCCESS;
}


static void matrix_calloc (int nrows, int ncols, int *** matrix) {
    int nelems = nrows * ncols;
    int * buf = calloc(nelems, sizeof(int));
    if (buf == nullptr) {
        code = __LINE__;
        fprintf(stderr, "ERROR %d: could not allocate memory for horizontal stripe buffer, aborting.\n", code);
        goto failure;
    }
    *matrix = calloc(nrows, sizeof(int *));
    if (*matrix == nullptr) {
        code = __LINE__;
        fprintf(stderr, "ERROR %d: could not allocate memory for matrix stripe, aborting.\n", code);
        goto failure;
    }
    for (int irow = 0; irow < nrows; irow++) {
        (*matrix)[irow] = &buf[irow * ncols];
    }
    return;
failure:
    MPI_Finalize();
    exit(code);
}


static void matrix_free (int *** matrix) {
    if (*matrix == nullptr) return;

    // free the 1-d buffer
    free(&(*matrix)[0][0]);

    // free the pointers to the 1-d buffer
    free(&(*matrix)[0]);

    // set the caller's matrix to nullptr;
    *matrix = nullptr;
}


static void matrix_gatherv (struct dual_int nrows, int ncols, const int ** matrix_lcl, int *** matrix_gbl, int irank, int nranks, MPI_Comm comm) {
    int * lens = nullptr;
    int * offsets = nullptr;
    if (irank == 0) {
        lens = calloc(nranks, sizeof(int));
        if (lens == nullptr) {
            code = __LINE__;
            fprintf(stderr, "ERROR %d: Problem allocating memory for lens array, aborting.\n", code);
            goto failure;
        }
        offsets = calloc(nranks, sizeof(int));
        if (offsets == nullptr) {
            code = __LINE__;
            fprintf(stderr, "ERROR %d: Problem allocating memory for offsets array, aborting.\n", code);
            goto failure;
        }
        int acc = 0;
        for (int i = 0; i < nranks; i++) {
            lens[i] = blkdcmp_get_blk_sz(nrows.gbl, nranks, i) * ncols;
            offsets[i] = acc;
            acc += lens[i];
        }
    }
    MPI_Barrier(comm);
    MPI_Gatherv(&matrix_lcl[0][0], nrows.lcl * ncols, MPI_INT, &(*matrix_gbl)[0][0], lens, offsets, MPI_INT, 0, comm);
    free(lens);
    free(offsets);
    return;
failure:
    free(lens);
    free(offsets);
    MPI_Finalize();
    exit(code);
}


static void matrix_print (int nrows, int ncols, const int ** matrix) {
    for (int irow = 0; irow < nrows; irow++) {
        for (int icol = 0; icol < ncols; icol++) {
            fprintf(stdout, "%5d%c", matrix[irow][icol], icol < ncols - 1 ? ' ' : '\n');
        }
    }
}


static void matrix_rand_init (int nrows, int ncols, int *** matrix) {
    // populate the matrix with random integers 0..9
    for (int irow = 0; irow < nrows; irow++) {
        for (int icol = 0; icol < ncols; icol++) {
            (*matrix)[irow][icol] = rand() % 10;
        }
    }
}


static void multiply (int nrows, int ncols, const int ** lhop, const int * rhop, int ** result) {
    for (int irow = 0; irow < nrows; irow++) {
        (*result)[irow] = 0;
        for (int icol = 0; icol < ncols; icol++) {
            (*result)[irow] += lhop[irow][icol] * rhop[icol];
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


static void result_gatherv (int nelems_lcl, const int * result_lcl, int nelems_gbl, int ** result_gbl, int irank, int nranks, MPI_Comm comm) {
    int * lens = nullptr;
    int * offsets = nullptr;
    if (irank == 0) {
        lens = calloc(nranks, sizeof(int));
        if (lens == nullptr) {
            code = __LINE__;
            fprintf(stderr, "ERROR %d: Problem allocating memory for lens array, aborting.\n", code);
            goto failure;
        }
        for (int i = 0; i < nranks; i++) {
            lens[i] = blkdcmp_get_blk_sz(nelems_gbl, nranks, i);
        }
        offsets = calloc(nranks, sizeof(int));
        if (offsets == nullptr) {
            code = __LINE__;
            fprintf(stderr, "ERROR %d: Problem allocating memory for offsets array, aborting.\n", code);
            goto failure;
        }
        int acc = lens[0];
        for (int i = 1; i < nranks; i++) {
            offsets[i] = acc;
            acc += lens[i];
        }
    }
    MPI_Barrier(comm);
    MPI_Gatherv(result_lcl, nelems_lcl, MPI_INT, *result_gbl, lens, offsets, MPI_INT, 0, comm);
    free(lens);
    free(offsets);
    return;
failure:
    free(lens);
    free(offsets);
    MPI_Finalize();
    exit(code);
}


static void show_usage (FILE * stream, struct cla * cla) {
    fprintf(stream,
            "Usage mpirun -np NP %s [REQUIREDS]\n"
            "\n"
            "  Matrix-vector multiplication using NP processes (row-striped matrix implementation).\n"
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


static void vector_bcast (int nelems, int ** vector, MPI_Comm comm) {
    MPI_Bcast(*vector, nelems, MPI_INT, 0, comm);
}


static void vector_calloc (int nelems, int ** vector) {
    *vector = calloc(nelems, sizeof(int));
    if (*vector == nullptr) {
        code = __LINE__;
        fprintf(stderr, "ERROR %d: could not allocate memory for vector, aborting.\n", code);
        goto failure;
    }
    return;
failure:
    MPI_Finalize();
    exit(code);
}


static void vector_free (int ** vector) {
    if (*vector == nullptr) return;

    // free the 1-d buffer
    free(&(*vector)[0]);

    // set the caller's vector to nullptr;
    *vector = nullptr;
}


static void vector_print (int nelems, const int * vector) {
    for (int ielem = 0; ielem < nelems; ielem++) {
        fprintf(stdout, "%5d\n", vector[ielem]);
    }
}


static void vector_rand_init (int nelems, int ** vector, int irank) {
    // populate the vector with random integers 0..9
    if (irank == 0) {
        for (int ielem = 0; ielem < nelems; ielem++) {
            (*vector)[ielem] = rand() % 10;
        }
    }
}
