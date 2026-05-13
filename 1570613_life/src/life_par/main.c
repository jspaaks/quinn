#include "blkdcmp/blkdcmp.h"     // blkdcmp_*
#include "cla/cla.h"             // CLA_*
#include "locale.h"              // setlocale, LC_ALL
#include <mpi.h>                 // MPI_*
#include <stdio.h>               // stdout, FILE, fflush
#include <stdlib.h>              // rand, srand
#include <sys/param.h>           // MAX
#include <time.h>                // time
#ifdef LIFE_PAR_TRAP_DBG
#include <unistd.h>              // gethostname, getpid, sleep
#endif // LIFE_PAR_TRAP_DBG
#include <wchar.h>               // wchar_t, fwprintf

struct geom {
    struct {
        int nrows;
        int ncols;
    } map;
    struct {
        int nrows;
        int ncols;
    } stripe;
};

static int get_ncols (struct cla * cla, int irank);
static int get_niters (struct cla * cla, int irank);
static int get_nrows (struct cla * cla, int irank);
static float get_prob (struct cla * cla, int irank);
static int ** neighs_calloc (struct geom geom);
static void neighs_count(struct geom geom, const bool ** stripe, int *** neighs);
static void neighs_free (int *** neighs);
static void show_usage (FILE * stream, struct cla * cla, int irank);
static bool ** stripe_calloc (struct geom geom);
static void stripe_communicate_borders (struct geom geom, bool *** stripe, int irank, int nranks, MPI_Comm comm);
static void stripe_free (bool *** stripe);
static void stripe_init (struct geom geom, bool ** stripe, float prob, int irank, int nranks, MPI_Comm comm);
static void stripe_print(FILE * stream, struct geom geom, bool ** stripe, int iiter, int irank, int nranks, MPI_Comm comm);
static void stripe_update(struct geom geom, bool *** stripe, const int ** neighs);


static int get_ncols (struct cla * cla, int irank) {
    const char * s = CLA_get_value_required(cla, "--ncols");
    int result = atoi(s);
    if (result <= 0 && irank == 0) {
        const int code = __LINE__;
        fwprintf(stderr, L"ERROR %d: could not parse input --ncols %s, aborting.\n", code, s);
        MPI_Finalize();
        exit(code);
    }
    return result;
}


static int get_niters (struct cla * cla, int irank) {
    const char * s = CLA_get_value_required(cla, "--niters");
    int result = atoi(s);
    if (result <= 0 && irank == 0) {
        const int code = __LINE__;
        fwprintf(stderr, L"ERROR %d: could not parse input --niters %s, aborting.\n", code, s);
        MPI_Finalize();
        exit(code);
    }
    return result;
}


static int get_nrows (struct cla * cla, int irank) {
    const char * s = CLA_get_value_required(cla, "--nrows");
    int result = atoi(s);
    if (result <= 0 && irank == 0) {
        const int code = __LINE__;
        fwprintf(stderr, L"ERROR %d: could not parse input --nrows %s, aborting.\n", code, s);
        MPI_Finalize();
        exit(code);
    }
    return result;
}


static float get_prob (struct cla * cla, int irank) {
    const char * s = CLA_get_value_required(cla, "--prob");
    float result = (float) atof(s);
    bool outofbounds = result < 0.00 || 1.00 < result;
    if (outofbounds && irank == 0) {
        const int code = __LINE__;
        fwprintf(stderr, L"ERROR %d: could not parse input --prob %s, aborting.\n", code, s);
        MPI_Finalize();
        exit(code);
    }
    return result;
}


int main (int argc, char * argv[]) {

    // initialize locale support
    setlocale(LC_ALL, "");

    // initialize mpi
    MPI_Init(&argc, &argv);

    // conditionally trap the debugger if a compile time variable has been defined
#ifdef LIFE_PAR_TRAP_DBG
    {
        volatile bool iswaiting = true;
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        fwprintf(stdout, L"PID %d on %s waiting for debugger attach...\n", getpid(), hostname);
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
    struct geom geom = {
        .map = {
            .nrows = -1,
            .ncols = -1,
        },
        .stripe = {
            .nrows = -1,
            .ncols = -1,
        },
    };
    float prob = -1.00;
    int niters = -1;

    // get your own rank and the total number of ranks
    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    // register the names of expected command line arguments
    cla = CLA_create();
    CLA_add_required(cla, "--nrows", "-r");
    CLA_add_required(cla, "--ncols", "-c");
    CLA_add_required(cla, "--niters", "-i");
    CLA_add_required(cla, "--prob", "-p");

    // parse the input that the user provided
    CLA_parse(cla, argc, (const char **) argv);

    // if the user asked for help, show it and exit
    if (CLA_help_requested(cla)) {
        show_usage(stdout, cla, irank);
        goto success;
    }

    // get the user input
    geom.map.nrows = get_nrows(cla, irank);
    geom.map.ncols = get_ncols(cla, irank);
    prob = get_prob(cla, irank);
    niters = get_niters(cla, irank);

    // derive the value of some variables related to the user input
    geom.stripe.nrows = blkdcmp_get_blk_sz(geom.map.nrows, nranks, irank) + 2;
    geom.stripe.ncols = geom.map.ncols + 2;

    // let each process allocate memory for its stripe and neighbor count arrays
    stripe = stripe_calloc(geom);
    neighs = neighs_calloc(geom);

    // let each process initialize its stripe arrays using the user defined probability
    stripe_init(geom, stripe, prob, irank, nranks, MPI_COMM_WORLD);

    // start simulating
    for (int iiter = 0; iiter < niters; iiter++) {

        // print the state
        stripe_print(stdout, geom, stripe, iiter, irank, nranks, MPI_COMM_WORLD);

        // communicate borders
        stripe_communicate_borders(geom, &stripe, irank, nranks, MPI_COMM_WORLD);

        // count neighbors
        neighs_count(geom, (const bool **) stripe, &neighs);

        // update stripe state
        stripe_update(geom, &stripe, (const int **) neighs);
    }

success:
    stripe_free(&stripe);
    neighs_free(&neighs);
    CLA_destroy(&cla);
    MPI_Finalize();
    return EXIT_SUCCESS;
}


static int ** neighs_calloc (struct geom geom) {
    int * buf = calloc(geom.stripe.nrows * geom.stripe.ncols, sizeof(int));
    if (buf == nullptr) {
        const int code = __LINE__;
        fwprintf(stderr, L"ERROR %d: Problem allocating memory for buffer, aborting.\n", code);
        MPI_Finalize();
        exit(code);
    }
    int ** buf2d = calloc(geom.stripe.nrows, sizeof(int *));
    if (buf2d == nullptr) {
        const int code = __LINE__;
        fwprintf(stderr, L"ERROR %d: Problem allocating memory for buffer, aborting.\n", code);
        MPI_Finalize();
        exit(code);
    }
    for (int irow = 0; irow < geom.stripe.nrows; irow++) {
        buf2d[irow] = &buf[irow * geom.stripe.ncols];
    }
    return buf2d;
}


static void neighs_count(struct geom geom, const bool ** stripe, int *** neighs) {
    constexpr int n = 8;  // there are 8 neighbors
    int offset_rows[n] = {-1, -1, -1,  0,  0,   1,  1,  1};
    int offset_cols[n] = {-1,  0,  1, -1,  1,  -1,  0,  1};
    {
        int i = 0;
        int dr = offset_rows[i];
        int dc = offset_cols[i];
        for (int irow = 1; irow < geom.stripe.nrows - 1; irow++) {
            for (int icol = 1; icol < geom.stripe.ncols - 1; icol++) {
                (*neighs)[irow][icol] = stripe[irow + dr][icol + dc] ? 1 : 0;
            }
        }
    }
    for (int i = 1; i < n; i++) {
        int dr = offset_rows[i];
        int dc = offset_cols[i];
        for (int irow = 1; irow < geom.stripe.nrows - 1; irow++) {
            for (int icol = 1; icol < geom.stripe.ncols - 1; icol++) {
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
        fwprintf(stream,
                 L"Usage: mpirun -np NP %s [REQUIREDS]\n"
                 "\n"
                 "  Multiprocess implementation of Conway's Game of Life, using NP processes.\n"
                 "\n"
                 "  Required named arguments\n"
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


static bool ** stripe_calloc (struct geom geom) {
    bool * buf = calloc(geom.stripe.nrows * geom.stripe.ncols, sizeof(bool));
    if (buf == nullptr) {
        const int code = __LINE__;
        fwprintf(stderr, L"ERROR %d: Problem allocating memory for buffer, aborting.\n", code);
        MPI_Finalize();
        exit(code);
    }
    bool ** buf2d = calloc(geom.stripe.nrows, sizeof(bool *));
    if (buf2d == nullptr) {
        const int code = __LINE__;
        fwprintf(stderr, L"ERROR %d: Problem allocating memory for buffer, aborting.\n", code);
        MPI_Finalize();
        exit(code);
    }
    for (int irow = 0; irow < geom.stripe.nrows; irow++) {
        buf2d[irow] = &buf[irow * geom.stripe.ncols];
    }
    return buf2d;
}


static void stripe_communicate_borders (struct geom geom, bool *** stripe, int irank, int nranks, MPI_Comm comm) {

    bool irank_is_even = irank % 2 == 0;
    bool irank_is_odd = !irank_is_even;
    bool next_rank_exists = irank + 1 < nranks;
    bool prev_rank_exists = irank - 1 >= 0;

    // send a row of data downward and receive it in the next rank
    if (irank_is_even && next_rank_exists) {
        bool * buf = &(*stripe)[geom.stripe.nrows - 2][1];
        MPI_Send(buf, geom.stripe.ncols - 2, MPI_C_BOOL, irank + 1, 0, comm);
    }
    if (irank_is_odd && prev_rank_exists) {
        bool * buf = &(*stripe)[0][1];
        MPI_Status status = {};
        MPI_Recv(buf, geom.stripe.ncols - 2, MPI_C_BOOL, irank - 1, 0, comm, &status);
    }
    // send a row of data upward and receive it in the previous rank
    if (irank_is_even && next_rank_exists) {
        bool * buf = &(*stripe)[geom.stripe.nrows - 1][1];
        MPI_Status status = {};
        MPI_Recv(buf, geom.stripe.ncols - 2, MPI_C_BOOL, irank + 1, 0, comm, &status);
    }
    if (irank_is_odd && prev_rank_exists) {
        bool * buf = &(*stripe)[1][1];
        MPI_Send(buf, geom.stripe.ncols - 2, MPI_C_BOOL, irank - 1, 0, comm);
    }
}


static void stripe_free (bool *** stripe) {
    if (*stripe == nullptr) return;
    free(&(*stripe)[0][0]);
    free(&(*stripe)[0]);
    *stripe = nullptr;
}


static void stripe_init (struct geom geom, bool ** stripe, float prob, int irank, int nranks, MPI_Comm comm) {

    if (irank == 0) {
        // initialize the pseudorandom number generator using the current time
        srand(time(nullptr));
        for (int idst = 1; idst < nranks; idst++) {
            int seed = rand();
            MPI_Send(&seed, 1, MPI_INT, idst, 0, comm);
        }
    } else {
        MPI_Status status = {};
        int seed = -1;
        MPI_Recv(&seed, 1, MPI_INT, 0, 0, comm, &status);
        // initialize the pseudorandom number generator using the seed from rank 0
        srand(seed);
    }

    int nelems = (geom.stripe.nrows - 2) * (geom.stripe.ncols - 2);
    int nalive = MAX(1, (int) (prob * nelems));
    for (int irow = 1; irow < geom.stripe.nrows - 1; irow++) {
        for (int icol = 1; icol < geom.stripe.ncols - 1; icol++) {
            bool cond = rand() % nelems < nalive;
            stripe[irow][icol] = cond;
            nalive -= cond ? 1 : 0;
            nelems--;
        }
    }
}


static void stripe_print(FILE * stream, struct geom geom, bool ** stripe, int iiter, int irank, int nranks, MPI_Comm comm) {
    bool ** recvbuf = nullptr;        // buffer for receiving the data from the other processes
    wchar_t dead_dark = L'\u2591';    // symbol to use for cell that is dead (darker background)
    wchar_t dead_light = L'\u2592';   // symbol to use for cell that is dead (lighter background)
    wchar_t alive = L'\u2588';        // symbol to use for cell that is alive
    int itgt = nranks - 1;            // last rank
    int irow_checkerboard = 0;
    // allocate memory for receiving data from the other processes
    if (irank == itgt) {
        recvbuf = stripe_calloc(geom);
        fwprintf(stream, L"t=%d:\n", iiter);
    }
    // let the other processes send their data to itgt who then prints it
    for (int isrc = 0; isrc < itgt; isrc++) {
        if (irank == itgt) {
            MPI_Status status = {};
            struct geom src = {
                .stripe = {
                    .nrows = (int) blkdcmp_get_blk_sz(geom.map.nrows, nranks, isrc) + 2,
                    .ncols = geom.stripe.ncols,
                },
                .map = geom.map,
            };
            MPI_Recv(&recvbuf[0][0], src.stripe.nrows * src.stripe.ncols, MPI_C_BOOL, isrc, 0, comm, &status);
            for (int irow = 1; irow < src.stripe.nrows - 1; irow++) {
                for (int icol = 1; icol < src.stripe.ncols - 1; icol++) {
                    wchar_t dead = (irow_checkerboard + irow) % 2 == icol % 2 ? dead_dark : dead_light;
                    wchar_t ch = recvbuf[irow][icol] ? alive : dead;
                    fwprintf(stream, L"%lc%lc", ch, ch);
                }
                fwprintf(stream, L"\n");
            }
            irow_checkerboard += src.stripe.nrows - 2;
            fflush(stream);
        }
        if (irank == isrc) {
            MPI_Send(&stripe[0][0], geom.stripe.nrows * geom.stripe.ncols, MPI_C_BOOL, itgt, 0, comm);
        }
        MPI_Barrier(comm);
    }
    // finally print the stripe for itgt without any sending/receiving, data is local
    if (irank == itgt) {
        stripe_free(&recvbuf);
        for (int irow = 1; irow < geom.stripe.nrows - 1; irow++) {
            for (int icol = 1; icol < geom.stripe.ncols - 1; icol++) {
                wchar_t dead = (irow_checkerboard + irow) % 2 == icol % 2 ? dead_dark : dead_light;
                wchar_t ch = stripe[irow][icol] ? alive : dead;
                fwprintf(stream, L"%lc%lc", ch, ch);
            }
            fwprintf(stream, L"\n");
        }
        fflush(stream);
    }
}


static void stripe_update(struct geom geom, bool *** stripe, const int ** neighs) {

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

    for (int irow = 1; irow < geom.stripe.nrows - 1; irow++ ) {
        for (int icol = 1; icol < geom.stripe.ncols - 1; icol++) {
            bool a = neighs[irow][icol] == 3;
            bool b = (*stripe)[irow][icol] && neighs[irow][icol] == 2;
            (*stripe)[irow][icol] = a || b;
        }
    }
}
