#include "blkdcmp/blkdcmp.h"     // blkdcmp_*
#include "cla/cla.h"             // CLA_*
#include <mpi.h>                 // MPI_*
#include <stdio.h>               // stdout, FILE, fprintf, fflush
#include <stdlib.h>              // rand, srand
#include <time.h>                // time
#ifdef LIFE_PAR_TRAP_DBG
#include <unistd.h>              // gethostname, getpid, sleep
#endif // LIFE_PAR_TRAP_DBG

struct localized {
    int map;
    int stripe;
};

static int get_ncols (struct cla * cla);
static int get_niters (struct cla * cla);
static int get_nrows (struct cla * cla);
static float get_prob (struct cla * cla);
static int ** neighs_calloc (int nrows, int ncols);
static void neighs_count(int nrows, int ncols, const bool ** stripe, int *** neighs);
static void neighs_free (int *** neighs);
static void show_usage (FILE * stream, struct cla * cla, int irank);
static bool ** stripe_calloc (int nrows, int ncols);
static void stripe_free (bool *** stripe);
static void stripe_init (int nrows, int ncols, bool ** stripe, float prob);
static void stripe_update(int nrows, int ncols, bool *** stripe, const int ** neighs);


static int get_ncols (struct cla * cla) {
    const char * s = CLA_get_value_required(cla, "--ncols");
    int result = atoi(s);
    if (result <= 0) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: could not parse input --ncols %s, aborting.\n", code, s);
        exit(code);
    }
    return result;
}


static int get_niters (struct cla * cla) {
    const char * s = CLA_get_value_required(cla, "--niters");
    int result = atoi(s);
    if (result <= 0) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: could not parse input --ncols %s, aborting.\n", code, s);
        exit(code);
    }
    return result;
}


static int get_nrows (struct cla * cla) {
    const char * s = CLA_get_value_required(cla, "--nrows");
    int result = atoi(s);
    if (result <= 0) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: could not parse input --nrows %s, aborting.\n", code, s);
        exit(code);
    }
    return result;
}


static float get_prob (struct cla * cla) {
    const char * s = CLA_get_value_required(cla, "--prob");
    float result = (float) atof(s);
    if (result < 0.00 || 1.00 < result) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: could not parse input --prob %s, aborting.\n", code, s);
        exit(code);
    }
    return result;
}


int main (int argc, char * argv[]) {
    MPI_Init(&argc, &argv);

#ifdef LIFE_PAR_TRAP_DBG
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
#endif // LIFE_PAR_TRAP_DBG

    int irank = -1;
    int nranks = -1;
    bool ** stripe = nullptr;
    int ** neighs = nullptr;
    struct cla * cla = nullptr;
    struct localized nrows = {.map = -1, .stripe = -1};
    struct localized ncols = {.map = -1, .stripe = -1};
    float prob = -1.00;
    int niters = -1;

    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    // initialize the psuedorandom number generator
    srand(time(nullptr));

    // each process:
    //    get the number of rows from the user
    //    get the number of columns from the user
    //    get the chance of life from the user
    //    randomly initialize the grid using the user-defined chance

    cla = CLA_create();
    CLA_add_required(cla, "--nrows", "-r");
    CLA_add_required(cla, "--ncols", "-c");
    CLA_add_required(cla, "--niters", "-i");
    CLA_add_required(cla, "--prob", "-p");
    CLA_parse(cla, argc, (const char **) argv);
    if (CLA_help_requested(cla)) {
        show_usage(stdout, cla, irank);
        goto success;
    }

    // get the user input
    nrows.map = get_nrows(cla);
    ncols.map = get_ncols(cla);
    prob = get_prob(cla);
    niters = get_niters(cla);

    // determine some derived variables
    nrows.stripe = blkdcmp_get_blk_sz(nrows.map, nranks, irank) + 2;
    ncols.stripe = ncols.map + 2;

    // let each process initialize its stripe
    neighs = neighs_calloc(nrows.stripe, ncols.stripe);
    stripe = stripe_calloc(nrows.stripe, ncols.stripe);
    stripe_init(nrows.stripe, ncols.stripe, stripe, prob);

    // start simulating
    for (int iiter = 0; iiter < niters; iiter++) {
        // communicate borders


        // count neighbors
        neighs_count(nrows.stripe, ncols.stripe, (const bool **) stripe, &neighs);

        // update stripe state
        stripe_update(nrows.stripe, ncols.stripe, &stripe, (const int **) neighs);
    }

success:
    stripe_free(&stripe);
    neighs_free(&neighs);
    CLA_destroy(&cla);
    MPI_Finalize();
    return EXIT_SUCCESS;
}


static int ** neighs_calloc (int nrows, int ncols) {
    int * buf = calloc(nrows * ncols, sizeof(int));
    if (buf == nullptr) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: Problem allocating memory for buffer, aborting.\n", code);
        exit(code);
    }
    int ** buf2d = calloc(nrows, sizeof(int *));
    if (buf2d == nullptr) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: Problem allocating memory for buffer, aborting.\n", code);
        exit(code);
    }
    for (int irow = 0; irow < nrows; irow++) {
        buf2d[irow] = &buf[irow * ncols];
    }
    return buf2d;
}


static void neighs_count(int nrows, int ncols, const bool ** stripe, int *** neighs) {
    constexpr int n = 8;  // there are 8 neighbors
    int offset_rows[n] = {-1, -1, -1,  0,  0,   1,  1,  1};
    int offset_cols[n] = {-1,  0,  1, -1,  1,  -1,  0,  1};
    {
        int i = 0;
        int dr = offset_rows[i];
        int dc = offset_cols[i];
        for (int irow = 1; irow < nrows - 1; irow++) {
            for (int icol = 1; icol < ncols - 1; icol++) {
                (*neighs)[irow][icol] = stripe[irow + dr][icol + dc] ? 1 : 0;
            }
        }
    }
    for (int i = 1; i < n; i++) {
        int dr = offset_rows[i];
        int dc = offset_cols[i];
        for (int irow = 1; irow < nrows - 1; irow++) {
            for (int icol = 1; icol < ncols - 1; icol++) {
                (*neighs)[irow][icol] += stripe[irow + dr][icol + dc] ? 1 : 0;
            }
        }
    }
}


static void neighs_free (int *** neighs) {
    if (*neighs == nullptr) return;
    free(&(*neighs)[0][0]);
    free(&(*neighs)[0]);
    *neighs = nullptr;
}


static void show_usage (FILE * stream, struct cla * cla, int irank) {
    if (irank == 0) {
        fprintf(stream,
                "Usage: mpirun -np NP %s [REQUIREDS]\n"
                "\n"
                "  Multiprocess implementation of Conway's Game of Life, using NP processes.\n"
                "\n"
                "  Requireds\n"
                "\n"
                "    --niters, -i   Number of iterations to simulate\n"
                "\n"
                "    --ncols, -c    Number of columns in the map\n"
                "\n"
                "    --nrows, -r    Number of rows in the map\n"
                "\n"
                "    --prob, -p     Probability [0, 1] of life in an individual cell at\n"
                "                   the beginning of the simulation\n"
                "\n",
                CLA_get_exename(cla));
    }
}


static bool ** stripe_calloc (int nrows, int ncols) {
    bool * buf = calloc(nrows * ncols, sizeof(bool));
    if (buf == nullptr) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: Problem allocating memory for buffer, aborting.\n", code);
        exit(code);
    }
    bool ** buf2d = calloc(nrows, sizeof(bool *));
    if (buf2d == nullptr) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: Problem allocating memory for buffer, aborting.\n", code);
        exit(code);
    }
    for (int irow = 0; irow < nrows; irow++) {
        buf2d[irow] = &buf[irow * ncols];
    }
    return buf2d;
}


static void stripe_free (bool *** stripe) {
    if (*stripe == nullptr) return;
    free(&(*stripe)[0][0]);
    free(&(*stripe)[0]);
    *stripe = nullptr;
}


static void stripe_init (int nrows, int ncols, bool ** stripe, float prob) {
    int z0 = (int) (prob * 100);
    for (int irow = 1; irow < nrows - 1; irow++) {
        for (int icol = 1; icol < ncols - 1; icol++) {
            int zi = rand() % 100;
            stripe[irow][icol] = zi < z0;
        }
    }
}


static void stripe_update(int nrows, int ncols, bool *** stripe, const int ** neighs) {

    // transitions if cell dead
    // 3 neighbors: alive
    // else: dead

    // transitions if cell alive
    // 2 neighbors: alive
    // 3 neighbors: alive
    // else: dead

    // which is the same as
    // nneighs[irow][icol] == 3 ||
    // (state[irow][icol] && nneighs[irow][icol] == 2)

    for (int irow = 1; irow < nrows - 1; irow++ ) {
        for (int icol = 1; icol < ncols - 1; icol++) {
            bool a = neighs[irow][icol] == 3;
            bool b = (*stripe)[irow][icol] && neighs[irow][icol] == 2;
            (*stripe)[irow][icol] = a || b;
        }
    }
}
