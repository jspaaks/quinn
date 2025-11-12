/*
    striped read from file, distribute to other processes
*/

#include "blkdcmp/blkdcmp.h"
#include "idx/idx.h"
#include <inttypes.h>
#include <mpi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef STRIPED_READ_TRAP_DBG
#include <unistd.h>
#endif // STRIPED_READ_TRAP_DBG

int main (int argc, char *argv[]) {

    MPI_Init(&argc, &argv);

#ifdef STRIPED_READ_TRAP_DBG
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
#endif // STRIPED_READ_TRAP_DBG

    int irank = 0;
    int nranks = 0;

    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    if (argc != 2) {
        if (irank == 0) {
            fprintf(stderr,
                    "Usage: striped-read FILEPATH\n"
                    "    Read IDX formatted adjacency matrix from FILEPATH and distribute\n"
                    "    stripes of it to other processes.");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    const char * filepath = argv[1];

    IdxHeader header = {};
    uint32_t nrows = UINT32_MAX;
    uint32_t ncols = UINT32_MAX;

    // use the last process to read the metadata
    if (irank == nranks - 1) {
        header = idx_read_header(filepath);
        nrows = header.lengths[0];
    }

    // broadcast the number of rows
    MPI_Bcast(&nrows, 1, MPI_UINT32_T, nranks - 1, MPI_COMM_WORLD);

    // determine the number of rows that are going to be processed by this process
    size_t blk_s = blkdcmp_get_idx_blk_s((size_t) nrows, (size_t) nranks, (size_t) irank);
    size_t blk_e = blkdcmp_get_idx_blk_e((size_t) nrows, (size_t) nranks, (size_t) irank);
    size_t blk_sz = blkdcmp_get_blk_sz((size_t) nrows, (size_t) nranks, (size_t) irank);
    fprintf(stdout, "rank %d processes %zu..%zu (%zu)\n", irank, blk_s, blk_e, blk_sz);

    // allocate the right amount of memory for this process
    ncols = nrows;
    uint8_t * mem = (uint8_t *) calloc(blk_sz * ncols, sizeof(uint8_t));
    uint8_t ** rows = (uint8_t **) calloc(blk_sz, sizeof(uint8_t *));
    for (size_t i = 0; i < blk_sz; i++) {
        rows[i] = &mem[i * ncols];
    }

    if (irank == nranks - 1) {
        // open the IDX file with the adjacency matrix
        FILE * stream = fopen(filepath, "rb");
        // move the cursor to the beginning of the data
        fseek(stream, header.bodystart, SEEK_SET);
        // read blocks of data and send it to the target process 0..n-2
        for (int i = 0; i < nranks - 1; ++i) {
            size_t tgt_blk_sz = blkdcmp_get_blk_sz((size_t) nrows, (size_t) nranks, (size_t) i);
            fread(mem, sizeof(uint8_t), tgt_blk_sz * ncols, stream);
            MPI_Send(mem, tgt_blk_sz * ncols, MPI_UINT8_T, i, 0, MPI_COMM_WORLD);
        }
        // read the last block of rows from file, forgo sending it to yourself
        fread(mem, sizeof(uint8_t), blk_sz * ncols, stream);
        // close the file
        fclose(stream);
    } else {
        // receive the data from process nranks - 1
        MPI_Status status = {};
        MPI_Recv(mem, blk_sz * ncols, MPI_UINT8_T, nranks - 1, 0, MPI_COMM_WORLD, &status);
    }

    // TODO do something useful with the data

    // free dynamically allocated memory
    free(rows);
    free(mem);

    // free MPI resources
    MPI_Finalize();

    return EXIT_SUCCESS;
}
