#include "cla/cla.h"
#include <stdio.h>
#include <stdlib.h>


static void count_neighbors(int nrows, int ncols, const bool ** state, int *** nneighs);
static void init_arrays (int nrows, int ncols, bool *** state, int *** nneighs);
static int parse_input_niterations(const struct cla * cla);
static void show_help(FILE * stream);
static void show_state (int nrows, int ncols, const bool ** state);
static void update(int nrows, int ncols, bool *** state, const int ** nneighs);


static void count_neighbors(int nrows, int ncols, const bool ** state, int *** nneighs) {
    constexpr int nneighbors = 8;
    int offset_rows[nneighbors] = {-1, -1, -1,  0,  0,   1,  1,  1};
    int offset_cols[nneighbors] = {-1,  0,  1, -1,  1,  -1,  0,  1};
    {
        int i = 0;
        int dr = offset_rows[i];
        int dc = offset_cols[i];
        for (int irow = 1; irow < nrows - 1; irow++) {
            for (int icol = 1; icol < ncols - 1; icol++) {
                (*nneighs)[irow][icol] = state[irow + dr][icol + dc] ? 1 : 0;
            }
        }
    }
    for (int i = 1; i < nneighbors; i++) {
        int dr = offset_rows[i];
        int dc = offset_cols[i];
        for (int irow = 1; irow < nrows - 1; irow++) {
            for (int icol = 1; icol < ncols - 1; icol++) {
                (*nneighs)[irow][icol] += state[irow + dr][icol + dc] ? 1 : 0;
            }
        }
    }
}


static void init_arrays (int nrows, int ncols, bool *** state, int *** nneighs) {
    int nelems = nrows * ncols;
    {
        bool * buf = calloc(nelems, sizeof(bool));
        *state = calloc(nrows, sizeof(bool *));
        for (int i = 0; i < nrows; i++) {
            (*state)[i] = &buf[i * ncols];
        }
    }
    {
        int * buf = calloc(nelems, sizeof(int));
        *nneighs = calloc(nrows, sizeof(int *));
        for (int i = 0; i < nrows; i++) {
            (*nneighs)[i] = &buf[i * ncols];
        }
    }

    // init some locations in state
    (*state)[1][1] = true;
    (*state)[1][4] = true;
    (*state)[1][5] = true;
    (*state)[2][4] = true;
    (*state)[2][5] = true;
    (*state)[3][5] = true;
    (*state)[4][1] = true;
    (*state)[4][2] = true;
    (*state)[4][3] = true;
    (*state)[4][5] = true;
    (*state)[5][5] = true;
}


static int parse_input_niterations(const struct cla * cla) {
    const char * s = CLA_get_value_positional(cla, 0);
    int niters = atoi(s);
    if (niters <= 0) {
        fprintf(stderr, "ERROR: could not convert the input to a positive integer, aborting.\n");
        exit(EXIT_FAILURE);
    }
    return niters;
}


int main (int argc, const char * argv[]) {

    struct cla * cla = CLA_create();
    CLA_add_positionals(cla, 1);
    CLA_parse(cla, argc, argv);
    if (CLA_help_requested(cla)) {
        show_help(stdout);
        exit(EXIT_SUCCESS);
    }

    constexpr int nrows = 5 + 2;
    constexpr int ncols = 5 + 2;
    int niters = parse_input_niterations((const struct cla *) cla);

    bool ** state = nullptr;
    int ** nneighs = nullptr;

    init_arrays(nrows, ncols, &state, &nneighs);

    for (int iiter = 0; iiter < niters; iiter++) {
        count_neighbors(nrows, ncols, (const bool **) state, &nneighs);
        update(nrows, ncols, &state, (const int **) nneighs);

        fprintf(stdout, "state@%d:\n", iiter);
        show_state(nrows, ncols, (const bool **) state);
        fprintf(stdout, "----------------\n");
    }

    // free memory associated with the command line argument parsing
    CLA_destroy(&cla);

    // free memory associated with the state
    free(&state[0][0]);
    free(&state[0]);
    state = nullptr;

    // free memory associated with the neighbor count
    free(&nneighs[0][0]);
    free(&nneighs[0]);
    nneighs = nullptr;

    return EXIT_SUCCESS;
}


static void show_help(FILE * stream) {
    fprintf(stream,
            "Usage: life N\n"
            "\n"
            "    Run Conway's Game of Life for N iterations, with N being a positive integer.\n"
            "\n");
}


static void show_state (int nrows, int ncols, const bool ** state) {
    for (int irow = 1; irow < nrows - 1; irow++) {
        for (int icol = 1; icol < ncols - 1; icol++) {
            fprintf(stdout, "%c%c", state[irow][icol] ? 'A' : '.', icol == ncols - 2 ? '\n' : ' ');
        }
    }
}


static void update(int nrows, int ncols, bool *** state, const int ** nneighs) {

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
            bool a = nneighs[irow][icol] == 3;
            bool b = (*state)[irow][icol] && nneighs[irow][icol] == 2;
            (*state)[irow][icol] = a || b;
        }
    }
}
