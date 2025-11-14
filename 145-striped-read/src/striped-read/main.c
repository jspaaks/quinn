/*
    striped read from file, distribute to other processes
*/

#include "blkdcmp/blkdcmp.h"
#include "idx/idx.h"
#include "rows_reader.h"
#include <inttypes.h>
#include <mpi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef STRIPED_READ_TRAP_DBG
#include <unistd.h>
#endif // STRIPED_READ_TRAP_DBG

typedef struct rows_reader RowsReader;

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
    RowsReader * reader = rows_reader_new();
    rows_reader_init(reader, filepath, MPI_COMM_WORLD);
    rows_reader_read(reader);

    // presumably, you'd do something useful with the contents of reader.buffer here

    // free resources
    rows_reader_delete(&reader);
    MPI_Finalize();

    return EXIT_SUCCESS;

}
