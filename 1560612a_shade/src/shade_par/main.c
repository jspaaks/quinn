#include <inttypes.h>
#ifdef SHADE_PAR_TRAP_DBG
#include <limits.h>          // HOST_NAME_MAX
#endif // SHADE_PAR_TRAP_DBG
#include <mpi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef SHADE_PAR_TRAP_DBG
#include <unistd.h>          // gethostname, getpid, sleep
#endif // SHADE_PAR_TRAP_DBG
#include "cla/cla.h"
#include "idx/idx.h"

void print_map(int nrows, int ncols, int8_t ** map, const char * label);
void show_usage (FILE * stream, const char * programname);
void verify_data_is_2d (struct idx_file_object * idx, const char * input_filepath);
void verify_data_is_int8 (struct idx_file_object * idx, const char * input_filepath);
void verify_data_is_square (struct idx_file_object * idx, const char * input_filepath);

#define LIT ((int8_t) 0)
#define SHADED ((int8_t) 1)

int main (int argc, const char * argv[]) {

    MPI_Init(&argc, (char ***) &argv);

#ifdef SHADE_PAR_TRAP_DBG
    {
        volatile int iswaiting = 1;
        char hostname[HOST_NAME_MAX + 1];
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
#endif // SHADE_PAR_TRAP_DBG

    int irank = -1;                    // this process's rank
    int nranks = -1;                   // number of processes

    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    // create the command line arguments parser object and its associated dynamic memory
    struct cla * cla = CLA_create();

    // define that this program expects exactly 1 positional argument
    CLA_add_positionals(cla, 1);

    // parse the command line arguments that the user provided
    CLA_parse(cla, argc, argv);

    // if the user requested help, show it and exit
    if (CLA_help_requested(cla)) {
        const char * programname = CLA_get_exename(cla);
        show_usage(stdout, programname);
        return EXIT_SUCCESS;
    }

    // retrieve the value of the positional argument at index 0
    const char * input_filepath = CLA_get_value_positional(cla, 0);

    // read the idx data
    struct idx_file_object * idx = idx_create_and_read(input_filepath);

    // run some checks
    if (idx_get_ndims(idx) != 2) {
        fprintf(stderr, "Expected 2 dimensions, aborting.\n");
        return EXIT_FAILURE;
    };

    verify_data_is_int8(idx, input_filepath);
    verify_data_is_2d(idx, input_filepath);
    verify_data_is_square(idx, input_filepath);

    // read the topograpic data into memory
    int nrows = idx_get_dim_length(idx, 0);
    int ncols = idx_get_dim_length(idx, 1);
    int nelems = idx_get_nelems(idx);
    int8_t * topobuf = (int8_t *) idx_get_data_int8(idx);
    int8_t ** topo = calloc(nrows, sizeof(int8_t *));
    for (int irow = 0; irow < nrows; irow++) {
        topo[irow] = &topobuf[irow * ncols];
    }

    // print the topographic map
    print_map(nrows, ncols, topo, "topo");
    fprintf(stdout, "\n");

    // allocate memory for the result
    int8_t * shadebuf = calloc(nelems, 1);
    int8_t ** shade = calloc(nrows, sizeof(int8_t *));
    for (int irow = 0; irow < nrows; irow++) {
        shade[irow] = &shadebuf[irow * ncols];
    }

    for (int irow = 0; irow < nrows; irow++) {
        shade[irow][0] = LIT;
        for (int icol = 1; icol < ncols; icol++) {
            for (int k = 1; k <= icol; k++) {
                bool cond = topo[irow][icol - k] >= topo[irow][icol] + 4 * k;
                if (cond) {
                    shade[irow][icol] = SHADED;
                    break;
                }
            }
        }
    }

    // print the shade map
    print_map(nrows, ncols, shade, "shade");

    // terminate mpi execution environment
    MPI_Finalize();

    // free memory resources
    free(topo);
    topo = nullptr;

    free(shadebuf);
    shadebuf = nullptr;
    free(shade);
    shade = nullptr;

    // free the memory associated with the idx file object
    idx_destroy(&idx);

    // free the memory assiociated with the command line arguments parser
    CLA_destroy(&cla);

    return EXIT_SUCCESS;
}

void print_map(int nrows, int ncols, int8_t ** map, const char * label) {
    fprintf(stdout, "%s:\n", label);
    for (int irow = 0; irow < nrows; irow++) {
        for (int icol = 0; icol < ncols; icol++) {
            fprintf(stdout, " %3" PRIi8, map[irow][icol]);
        }
        fprintf(stdout, "\n");
    }
}

void show_usage (FILE * stream, const char * programname) {
    fprintf(stream,
            "Usage: %s FILENAME\n"
            "\n"
            "    Calculate shading based on topographic data from FILENAME, using a parallel\n"
            "    implementation and assuming that the sun is due West.\n"
            "\n"
            "    Arguments\n"
            "\n"
            "        FILENAME   IDX formatted, 2-dimensional, square array of int8_t\n"
            "                   holding the topographic data\n"
            "\n", programname);
}

void verify_data_is_2d (struct idx_file_object * idx, const char * input_filepath) {
    int ndims = idx_get_ndims(idx);
    if (ndims != 2) {
        fprintf(stderr, "Expected number of dimensions in file '%s' to be 2 but got %d, "
                "aborting.\n", input_filepath, ndims);
        exit(EXIT_FAILURE);
    }
}

void verify_data_is_int8 (struct idx_file_object * idx, const char * input_filepath) {
    IdxDataType type = idx_get_type(idx);
    const char * data_type_name = idx_get_type_name(idx);
    if (type != IDX_DATA_TYPE_INT8) {
        fprintf(stderr, "Expected data type in file '%s' to be int8_t but got '%s', "
                "aborting.\n", input_filepath, data_type_name);
        exit(EXIT_FAILURE);
    }
}

void verify_data_is_square (struct idx_file_object * idx, const char * input_filepath) {
    int nrows = idx_get_dim_length(idx, 0);
    int ncols = idx_get_dim_length(idx, 1);
    if (nrows != ncols) {
        fprintf(stderr, "Expected a square matrix in file '%s' but got %dx%d.\n",
                input_filepath, nrows, ncols);
        exit(EXIT_FAILURE);
    }
}
