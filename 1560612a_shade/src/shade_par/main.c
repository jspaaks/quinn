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
#include "idx/idx.h"

void print_map(int nrows, int ncols, int8_t ** map, const char * label);
void show_usage (FILE * stream, const char * programname);
void verify_data_is_2d (IdxHeader * header, const char * input_filepath);
void verify_data_is_int8 (IdxHeader * header, const char * input_filepath);
void verify_data_is_square (IdxHeader * header, const char * input_filepath);

#define LIT ((int8_t) 0)
#define SHADED ((int8_t) 1)

int main (int argc, char * argv[]) {

    MPI_Init(&argc, &argv);

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

    if (argc == 2) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            show_usage(stdout, argv[0]);
            return EXIT_SUCCESS;
        }
    } else {
        show_usage(stderr, argv[0]);
        return EXIT_FAILURE;
    }

    const char * input_filepath = argv[1];

    IdxHeader header = idx_read_header(input_filepath);

    // run some checks
    verify_data_is_int8(&header, input_filepath);
    verify_data_is_2d(&header, input_filepath);
    verify_data_is_square(&header, input_filepath);

    // read the topograpic data into memory
    int nrows = (int) header.lengths[0];
    int ncols = (int) header.lengths[1];
    int8_t * topobuf = calloc(header.nelems, 1);
    int8_t ** topo = calloc(nrows, sizeof(int8_t *));
    for (int irow = 0; irow < nrows; irow++) {
        topo[irow] = &topobuf[irow * ncols];
    }
    idx_read_body_as_int8(input_filepath, &header, topobuf);

    // print the topographic map
    print_map(nrows, ncols, topo, "topo");
    fprintf(stdout, "\n");

    // allocate memory for the result
    int8_t * shadebuf = calloc(header.nelems, 1);
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
    free(topobuf);
    topobuf = nullptr;
    free(topo);
    topo = nullptr;
    free(shadebuf);
    shadebuf = nullptr;
    free(shade);
    shade = nullptr;

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
            "    Calculate shading based on topographic data from FILENAME, assuming that the\n"
            "    sun is due West, and using a parallel implementation.\n"
            "\n"
            "    Arguments\n"
            "\n"
            "        FILENAME   IDX formatted, 2-dimensional, square array of int8_t\n"
            "                   holding the topographic data\n"
            "\n", programname);
}

void verify_data_is_2d (IdxHeader * header, const char * input_filepath) {
    if (header->ndims != 2) {
        fprintf(stderr, "Expected number of dimensions in file '%s' to be 2 but got %" PRIu8", "
                "aborting.\n", input_filepath, header->ndims);
        exit(EXIT_FAILURE);
    }
}

void verify_data_is_int8 (IdxHeader * header, const char * input_filepath) {
    if (header->type != 0x09) {
        fprintf(stderr, "Expected data type in file '%s' to be int8_t but got something else, "
                "aborting.\n", input_filepath);
        exit(EXIT_FAILURE);
    }
}

void verify_data_is_square (IdxHeader * header, const char * input_filepath) {
    if (header->lengths[0] != header->lengths[1]) {
        fprintf(stderr, "Expected a square matrix in file '%s' but got %" PRIu32 "x%" PRIu32 ".\n",
                input_filepath, header->lengths[0], header->lengths[1]);
        exit(EXIT_FAILURE);
    }
}
