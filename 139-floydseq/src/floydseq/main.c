#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/param.h>
#include "idx/idx.h"

void calc_floyd (uint32_t n, uint8_t ** mat);
void print_matrix(uint32_t n, const uint8_t ** mat);
void show_help(const char * argv[]);

static constexpr uint8_t basically_inf = 0x63;

void calc_floyd (uint32_t n, uint8_t ** mat) {
    for (uint32_t ivia = 0; ivia < n; ivia++) {
        for (uint32_t isrc = 0; isrc < n; isrc++) {
            for (uint32_t idst = 0; idst < n; idst++) {
                uint8_t direct = mat[isrc][idst];
                uint8_t detour = mat[isrc][ivia] + mat[ivia][idst];
                mat[isrc][idst] = MIN(direct, detour);
            }
        }
    }
}

int main (int argc, const char * argv[]) {
    if (argc != 2) {
        show_help(argv);
        return EXIT_FAILURE;
    }

    const char * filepath = argv[1];
    IdxHeader header = idx_read_header(filepath);
    uint8_t * body = idx_read_body_as_uint8(filepath, &header);
    uint8_t ** mat = calloc(header.nelems, sizeof(uint8_t *));
    if (mat == nullptr) {
        fprintf(stderr, "Encountered problem during memory allocation for variable 'mat', aborting\n");
    }
    uint32_t nrows = header.lengths[0];
    uint32_t ncols = header.lengths[1];
    for (uint32_t irow = 0; irow < nrows; irow++) {
        mat[irow] = &body[irow * ncols];
    }

    fprintf(stdout, "\nExample of Floyd's algorithm\n\n");
    fprintf(stdout, "adjacency matrix:\n");
    print_matrix(nrows, (const uint8_t **) mat);
    fprintf(stdout, "\n");

    calc_floyd(header.lengths[0], mat);

    fprintf(stdout, "shortest-path matrix:\n");
    print_matrix(nrows, (const uint8_t **) mat);
    fprintf(stdout, "\n");

    idx_free_body((void **) &body);
    free(mat);
    return EXIT_SUCCESS;

}

void print_matrix(uint32_t n, const uint8_t ** mat) {
    for (uint32_t isrc = 0; isrc < n; isrc++) {
        for (uint32_t idst = 0; idst < n; idst++) {
            if (mat[isrc][idst] == basically_inf) {
                fprintf(stdout, "%5s ", "Inf");
            } else {
                fprintf(stdout, "%5" PRIu8 " ", mat[isrc][idst]);
            }
        }
        fprintf(stdout, "\n");
    }
}


void show_help (const char * argv[]) {
    fprintf(stderr,
            "Usage: %s FILEPATH\n"
            "    Read a directed acyclic graph's adjacency matrix from FILEPATH\n"
            "    and use Floyd's algorithm to determine the shortest-path\n"
            "    matrix. FILEPATH should point to a binary file in IDX format.\n"
            , argv[0]);
}
