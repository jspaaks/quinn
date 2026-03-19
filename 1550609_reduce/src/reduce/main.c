#include <mpi.h>             // MPI_*
#include <stdio.h>           // fflush, fprintf, stdout
#include <stdlib.h>          // EXIT_FAILURE, EXIT_SUCCESS
#include <string.h>          // strcmp
#ifdef REDUCE_TRAP_DBG
#include <limits.h>          // HOST_NAME_MAX
#include <unistd.h>          // gethostname, getpid, sleep
#endif // REDUCE_TRAP_DBG
#include "blkdcmp/blkdcmp.h"


int calc_local_sum (const int * arr, int nelems);
void reduce (const void * sendbuf, void * recvbuf, int root, MPI_Comm comm);
void show_usage (FILE * stream, const char * programname);


int calc_local_sum (const int * arr, int nelems) {
    int summed = 0;
    for (int i = 0; i < nelems; i++) {
        summed += arr[i];
    }
    return summed;
}


int main (int argc, const char * argv[]) {
    MPI_Init(&argc, (char ***) &argv);

#ifdef REDUCE_TRAP_DBG
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
#endif // REDUCE_TRAP_DBG

    int irank = -1;
    int nranks = -1;

    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    if (argc != 2) {
        if (irank == 0) {
            show_usage(stderr, argv[0]);
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
        // wait for rank 0's printing
        MPI_Barrier(MPI_COMM_WORLD);
        return EXIT_FAILURE;
    }
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        if (irank == 0) {
            show_usage(stdout, argv[0]);
        }
        MPI_Finalize();
        return EXIT_SUCCESS;
    }

    // read the user's input for nelemsg from the argument list
    const int nelemsg = atoi(argv[1]) + 1;

    // determine the local boundaries of the section this process is going to handle
    const int istart = (int) blkdcmp_get_idx_blk_s(nelemsg, nranks, irank);
    const int nelems = (int) blkdcmp_get_blk_sz(nelemsg, nranks, irank);

    // initialize the array that we are going to reduce
    int * arr = calloc(nelemsg, sizeof(int));
    for (int i = istart; i < istart + nelems; i++) {
        arr[i - istart] = i;
    }

    // specify which rank is the root
    const int iroot = 0;

    // calc local sum
    int summed = 0;
    int summedg = 0;
    fprintf(stdout, "rank %d will calculate the sum of %d-%d (%d)\n", irank, istart, istart + nelems - 1, nelems);
    fflush(stdout);
    if (irank == iroot) {
        summedg = calc_local_sum(arr, nelems);
    } else {
        summed = calc_local_sum(arr, nelems);
    }

    // combine the local sums
    reduce(&summed, &summedg, 0, MPI_COMM_WORLD);

    // report result to standard out
    MPI_Barrier(MPI_COMM_WORLD);
    if (irank == iroot) {
        fprintf(stdout, "The sum of [0..%d] is %d\n", nelemsg - 1, summedg);
        fflush(stdout);
    }

    // terminate mpi execution environment
    MPI_Finalize();

    // free memory resources
    free(arr);
    arr = nullptr;

    return EXIT_SUCCESS;
}


void reduce (const void * sendbuf, void * recvbuf, int iroot, MPI_Comm comm) {

    int irank = -1;
    int nranks = -1;

    MPI_Comm_rank(comm, &irank);
    MPI_Comm_size(comm, &nranks);

    if (irank == iroot) {
        int sum = 0;
        for (int i = 0; i < nranks; i++) {
            if (i == iroot) continue;
            int received = -1;
            MPI_Status status = {};
            MPI_Recv(&received, 1, MPI_INT, MPI_ANY_SOURCE, 0, comm, &status);
            sum += received;
        }
        *(int *)recvbuf += sum;
    } else {
        MPI_Send(sendbuf, 1, MPI_INT, iroot, 0, comm);
    }
}

void show_usage (FILE * stream, const char * programname) {
    fprintf(stream,
            "Usage: mpirun -np N %s M\n"
            "\n"
            "    Use N processes to sum integers 0..M using a reduce operation.\n"
            "\n", programname);
}
