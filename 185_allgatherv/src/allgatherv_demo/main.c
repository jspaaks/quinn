#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef ALLGATHERV_DEMO_TRAP_DBG
#include <unistd.h>              // gethostname, getpid, sleep
#endif // ALLGATHERV_DEMO_TRAP_DBG

int code = 0;

int main (int argc, char * argv[]) {
    constexpr int nranks_expected = 4;
    constexpr int n = 14;

    // initialize mpi
    MPI_Init(&argc, &argv);

    // conditionally trap the debugger if a compile time variable has been defined
#ifdef ALLGATHERV_DEMO_TRAP_DBG
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
#endif // ALLGATHERV_DEMO_TRAP_DBG

    int irank = -1;
    int nranks = -1;

    // get your own rank and the total number of ranks
    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    if (nranks != nranks_expected) {
        code = __LINE__;
        if (irank == 0) {
            fprintf(stderr, "Program should have %d ranks, aborting.\n", nranks_expected);
        }
        goto failure;
    }

    int * slice = nullptr;
    int replicated[n] = {};
    int caps[nranks_expected] = {2, 3, 4, 5};
    int offsets[nranks_expected] = {0, 2, 5, 9};

    // let each process allocate its own amount of space
    slice = calloc(caps[irank], sizeof(int));
    if (slice == nullptr) {
        code = __LINE__;
        if (irank == 0) {
            fprintf(stderr, "Something went wrong allocating memory for slice in rank %d, aborting.\n", irank);
        }
        goto failure;
    }

    // initialize the allocated memory with values that are unique across processes
    for (int i = 0; i < caps[irank]; i++) {
        slice[i] = offsets[irank] + i + 100;
    }

    // let each process concatenate slices from all processes into one replicated array
    MPI_Allgatherv((void *) slice, caps[irank], MPI_INT, (void *) replicated, caps, offsets, MPI_INT, MPI_COMM_WORLD);


    if (irank == 0) {
        fprintf(stdout, "MPI_Allgatherv demo\n");
        // print the expected result
        fprintf(stdout, "expected: ");
        for (int i = 0; i < n; i++) {
            fprintf(stdout, "%4d%c", i + 100, i == n - 1 ? '\n' : ' ');
        }
        // print the replicated array
        fprintf(stdout, "actual  : ");
        for (int i = 0; i < n; i++) {
            fprintf(stdout, "%4d%c", replicated[i], i == n - 1 ? '\n' : ' ');
        }
        fflush(stdout);
    }

    // terminate the MPI execution environment
    MPI_Finalize();
    return EXIT_SUCCESS;
failure:
    MPI_Finalize();
    return code;
}
