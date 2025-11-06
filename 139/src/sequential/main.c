#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>

void calc_floyd (int n, int ** mat);
void initialize_data(int n, const int * vec, int ** mat);
void print_matrix(int n, const int ** mat);

static constexpr int basically_inf = INT_MAX / 2;

void calc_floyd (int n, int ** mat) {
    for (int ivia = 0; ivia < n; ivia++) {
        fprintf(stdout, "ivia=%d\n", ivia);
        print_matrix(n, (const int **) mat);
        for (int isrc = 0; isrc < n; isrc++) {
            for (int idst = 0; idst < n; idst++) {
                int direct = mat[isrc][idst];
                int detour = mat[isrc][ivia] + mat[ivia][idst];
                mat[isrc][idst] = MIN(direct, detour);
            }
        }
        fprintf(stdout, "\n\n");
    }
}

int main (void) {

    int n = 6;

    int * vec = (int *) calloc(n*n, sizeof(int));
    if (vec == nullptr) {
        fprintf(stderr, "Encountered problem during memory allocation for variable 'vec', aborting\n");
    }
    int ** mat = (int **) calloc(n, sizeof(int *));
    if (mat == nullptr) {
        fprintf(stderr, "Encountered problem during memory allocation for variable 'mat', aborting\n");
    }
    initialize_data(n, vec, mat);


    fprintf(stdout, "Example of Floyd's algorithm\n\n");
    fprintf(stdout, "adjacency:\n");
    print_matrix(n, (const int **) mat);
    fprintf(stdout, "\n");

    fprintf(stdout, "shortest path:\n");
    calc_floyd(n, mat);
    print_matrix(n, (const int **) mat);

    return EXIT_SUCCESS;
}

void initialize_data(int n, const int * vec, int ** mat) {
    for (int isrc = 0; isrc < n; isrc++) {
        mat[isrc] = (int *) &vec[isrc * n];
        for (int idst = 0; idst < n; idst++) {
            if (isrc == idst) {
                mat[isrc][idst] = 0;
            } else {
                mat[isrc][idst] = basically_inf;
            }
        }
    }
    mat[0][1] = 2;
    mat[0][2] = 5;
    mat[1][2] = 7;
    mat[1][3] = 1;
    mat[1][5] = 8;
    mat[2][3] = 4;
    mat[3][4] = 3;
    mat[4][2] = 2;
    mat[4][5] = 3;
    mat[5][1] = 5;
    mat[5][3] = 2;
    mat[5][4] = 4;
}

void print_matrix(int n, const int ** mat) {
    for (int isrc = 0; isrc < n; isrc++) {
        for (int idst = 0; idst < n; idst++) {
            if (mat[isrc][idst] == basically_inf) {
                fprintf(stdout, "%5s ", "Inf");
            } else {
                fprintf(stdout, "%5d ", mat[isrc][idst]);
            }
        }
        fprintf(stdout, "\n");
    }
}
