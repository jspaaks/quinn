#include "blkdcmp/blkdcmp.h"
#include "cla/cla.h"
#include "idx/idx.h"
#include <inttypes.h>
#include <errno.h>           // errno
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


#define INPUT_FILEPATH_CAP 4096

struct nrows {
    int lcl;
    int gbl;
};

static FILE * open_file(const char * filepath);
static void print_map(struct nrows nrows, int ncols, int8_t * map, const char * label, MPI_Comm comm);
static void show_usage (FILE * stream, const char * programname);
static void verify_data_is_2d (struct idx_file_object * idx, const char * input_filepath, MPI_Comm comm);
static void verify_data_is_int8 (struct idx_file_object * idx, const char * input_filepath, MPI_Comm comm);
static void verify_data_is_square (struct idx_file_object * idx, const char * input_filepath, MPI_Comm comm);

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

    struct nrows nrows = {.lcl = -1, .gbl = -1};
    int ncols = -1;

    char input_filepath[INPUT_FILEPATH_CAP] = {};

    // the last rank loads the data from file and sends a slice of the data to the other ranks
    if (irank == nranks - 1) {

        // create the command line arguments parser object and its associated dynamic memory
        struct cla * cla = CLA_create();

        // define that this program expects exactly 1 positional argument
        CLA_add_positionals(cla, 1);

        // parse the command line arguments that the user provided
        CLA_parse(cla, argc, argv);

        // if the user requested help, show it and exit
        if (CLA_help_requested(cla)) {
            if (irank == nranks - 1) {
                const char * programname = CLA_get_exename(cla);
                show_usage(stdout, programname);
            }
            MPI_Finalize();
            return EXIT_SUCCESS;
        }

        // retrieve the value of the positional argument at index 0
        const char * tmp = CLA_get_value_positional(cla, 0);
        strcpy(input_filepath, tmp);

        // create the idx object
        struct idx_file_object * idx = idx_create(input_filepath);

        // read the idx metadata
        idx_read_meta(idx);

        // verify the data is in the expected format
        verify_data_is_int8(idx, input_filepath, MPI_COMM_WORLD);
        verify_data_is_2d(idx, input_filepath, MPI_COMM_WORLD);
        verify_data_is_square(idx, input_filepath, MPI_COMM_WORLD);

        // send the number of rows and the number of columns to the other ranks
        nrows.gbl = idx_get_dim_length(idx, 0);
        ncols = idx_get_dim_length(idx, 1);
        for (int idst = 0; idst < nranks; idst++) {
            if (idst != nranks - 1) {
                MPI_Send(&nrows.gbl, 1, MPI_INT, idst, 0, MPI_COMM_WORLD);
                MPI_Send(&ncols, 1, MPI_INT, idst, 0, MPI_COMM_WORLD);
                MPI_Send(&input_filepath[0], INPUT_FILEPATH_CAP, MPI_CHAR, idst, 0, MPI_COMM_WORLD);
            }
        }

        // free the memory associated with the idx file object
        idx_destroy(&idx);

        // free the memory assiociated with the command line arguments parser
        CLA_destroy(&cla);
    } else {
        MPI_Status status = {};    // note: result is not used, just a placeholder
        MPI_Recv(&nrows.gbl, 1, MPI_INT, nranks - 1, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&ncols, 1, MPI_INT, nranks - 1, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&input_filepath[0], INPUT_FILEPATH_CAP, MPI_CHAR, nranks - 1, 0, MPI_COMM_WORLD, &status);
    }

    // in each process, reserve space for some of the body data
    nrows.lcl = blkdcmp_get_blk_sz(nrows.gbl, nranks, irank);
    int8_t * topobuf = calloc(nrows.lcl * ncols, sizeof(int8_t));
    if (topobuf == nullptr) {
        fprintf(stderr, "(%d): Something went wrong allocating memory for storing topography data, aborting.\n", irank);
        MPI_Abort(MPI_COMM_WORLD, 1);
        exit(EXIT_FAILURE);
    }

    if (irank == nranks - 1) {
        // calculate the position of where the body starts
        int ndims = 2;
        int body_start = 4 * sizeof(uint8_t) + ndims * sizeof(uint32_t);

        // open the IDX file and set the cursor to the start of the body
        FILE * fp = open_file(input_filepath);
        fseek(fp, body_start, SEEK_SET);

        // read blocks of data and send it to the relevant destination process
        for (int idst = 0; idst < nranks; idst++) {
            // determine the block size in bytes of the destination
            int nbytes = (int) blkdcmp_get_blk_sz(nrows.gbl, nranks, idst) * ncols * sizeof(int8_t);

            // read nbytes into topobuf
            const int count = (int) fread(topobuf, 1, nbytes, fp);
            if (count != nbytes) {
                fprintf(stderr, "(%d): Something went wrong reading data from IDX file, aborting.\n", irank);
                fclose(fp);
                MPI_Abort(MPI_COMM_WORLD, 2);
                exit(EXIT_FAILURE);
            }

            // send it, unless we would be sending it to ourselves
            if (idst != nranks - 1) {
                MPI_Send(topobuf, nbytes, MPI_INT8_T, idst, 0, MPI_COMM_WORLD);
            }
        }

        // done reading from file, close it
        fclose(fp);
    } else {
        // determine the block size in bytes
        int nbytes = nrows.lcl * ncols * sizeof(int8_t);

        // receive the data from irank - 1
        MPI_Status status = {};
        MPI_Recv(topobuf, nbytes, MPI_INT8_T, nranks - 1, 0, MPI_COMM_WORLD, &status);
    }

    int8_t ** topo = calloc(nrows.lcl, sizeof(int8_t *));
    for (int irow = 0; irow < nrows.lcl; irow++) {
        topo[irow] = &topobuf[irow * ncols];
    }

    // print the topographic map
    print_map(nrows, ncols, topobuf, "topo", MPI_COMM_WORLD);

    // allocate memory for the result
    int8_t * shadebuf = calloc(nrows.lcl * ncols, 1);
    int8_t ** shade = calloc(nrows.lcl, sizeof(int8_t *));
    for (int irow = 0; irow < nrows.lcl; irow++) {
        shade[irow] = &shadebuf[irow * ncols];
    }

    for (int irow = 0; irow < nrows.lcl; irow++) {
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
    print_map(nrows, ncols, shadebuf, "shade", MPI_COMM_WORLD);

    // terminate mpi execution environment
    MPI_Finalize();

    // free memory associated with topography map
    free(topobuf);
    topobuf = nullptr;
    free(topo);
    topo = nullptr;

    // free memory resources associated with shade map
    free(shadebuf);
    shadebuf = nullptr;
    free(shade);
    shade = nullptr;

    return EXIT_SUCCESS;
}


static FILE * open_file(const char * filepath) {
    errno = 0;
    FILE * fp = fopen(filepath, "rb");
    if (fp == nullptr) {
        fprintf(stderr, "%s\nError reading binary data from file '%s', aborting.\n",
                strerror(errno), filepath);
        exit(EXIT_FAILURE);
    }
    return fp;
}


static void print_map(struct nrows nrows, int ncols, int8_t * slice, const char * label, MPI_Comm comm) {

    int irank = -1;                    // this process's rank
    int nranks = -1;                   // number of processes

    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    int8_t * tmp = nullptr;
    char * str = nullptr;
    int pos = 0;

    if (irank == nranks - 1) {
        tmp = calloc(nrows.lcl * ncols, sizeof(int8_t));
        if (tmp == nullptr) {
            const int code = 7;
            fprintf(stderr, "ERROR %d: Could not allocate memory for temporary storage, aborting.\n", code);
            MPI_Abort(comm, code);
            exit(code);
        }
        {
            const int newline = 1;
            const int colon = 1;
            const int nul = 1;
            str = calloc(strlen(label) + colon + newline +
                         nrows.gbl * (4 * ncols) + nul, sizeof(char));
        }
        if (str == nullptr) {
            const int code = 8;
            fprintf(stderr, "ERROR %d: Could not allocate memory for temporary string, aborting.\n", code);
            MPI_Abort(comm, code);
            exit(code);
        }
        int written = 0;
        sprintf(&str[pos], "%s:\n%n", label, &written);
        pos += written;
    }

    // let all ranks catch up before continuing
    MPI_Barrier(comm);

    // iterate over all ranks except the last one
    for (int isrc = 0; isrc < nranks - 1; isrc++) {

        // if it is my turn, send the slice local to this rank to the last rank
        if (irank == isrc) {
            MPI_Send(slice, nrows.lcl * ncols, MPI_INT8_T, nranks - 1, 0, comm);
        }

        // if my rank is the last rank,
        if (irank == nranks - 1) {

            // receive the data sent by rank isrc
            int nrows_src = blkdcmp_get_blk_sz(nrows.gbl, nranks, isrc);
            MPI_Status status = {};
            MPI_Recv(tmp, nrows_src * ncols, MPI_INT8_T, isrc, 0, comm, &status);

            // print the data just received, and keep track of where we are in the buffer `str`
            for (int irow = 0; irow < nrows_src; irow++) {
                for (int icol = 0; icol < ncols; icol++) {
                    int i = irow * ncols + icol;
                    char c = i % ncols == ncols - 1 ? '\n' : ' ';
                    int written = 0;
                    sprintf(&str[pos], "%3" PRIi8 "%c%n", tmp[i], c, &written);
                    pos += written;
                }
            }
        }

        // let all ranks catch up before continuing with the next send/recv
        MPI_Barrier(comm);
    }


    // if my rank is the last rank,
    if (irank == nranks - 1) {

        // render the slice local to this rank to string -- no need for sending and receiving
        for (int irow = 0; irow < nrows.lcl; irow++) {
            for (int icol = 0; icol < ncols; icol++) {
                int i = irow * ncols + icol;
                char c = i % ncols == ncols - 1 ? '\n' : ' ';
                int written = 0;
                sprintf(&str[pos], "%3" PRIi8 "%c%n", slice[i], c, &written);
                pos += written;
            }
        }

        // print the string we have been rendering starting at position 0
        fprintf(stdout, "%s", str);
        fflush(stdout);

        // free resources used for receiving data
        free(tmp);
        tmp = nullptr;

        // free resources used for rendering to string
        free(str);
        str = nullptr;
    }

    // put another barrier to avoid out-of-order lines in case we're
    // printing something after this function 
    MPI_Barrier(comm);
}


static void show_usage (FILE * stream, const char * programname) {
    fprintf(stream,
            "Usage: mpirun -np N %s FILENAME\n"
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


static void verify_data_is_2d (struct idx_file_object * idx, const char * input_filepath, MPI_Comm comm) {
    int ndims = idx_get_ndims(idx);
    if (ndims != 2) {
        fprintf(stderr, "Expected number of dimensions in file '%s' to be 2 but got %d, "
                "aborting.\n", input_filepath, ndims);
        MPI_Abort(comm, 3);
        exit(EXIT_FAILURE);
    }
}


static void verify_data_is_int8 (struct idx_file_object * idx, const char * input_filepath, MPI_Comm comm) {
    IdxDataType type = idx_get_type(idx);
    const char * data_type_name = idx_get_type_name(idx);
    if (type != IDX_DATA_TYPE_INT8) {
        fprintf(stderr, "Expected data type in file '%s' to be int8_t but got '%s', "
                "aborting.\n", input_filepath, data_type_name);
        MPI_Abort(comm, 4);
        exit(EXIT_FAILURE);
    }
}


static void verify_data_is_square (struct idx_file_object * idx, const char * input_filepath, MPI_Comm comm) {
    int nrows = idx_get_dim_length(idx, 0);
    int ncols = idx_get_dim_length(idx, 1);
    if (nrows != ncols) {
        fprintf(stderr, "Expected a square matrix in file '%s' but got %dx%d.\n",
                input_filepath, nrows, ncols);
        MPI_Abort(comm, 5);
        exit(EXIT_FAILURE);
    }
}
