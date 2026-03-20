#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef BCAST_TRAP_DBG
#include <limits.h>          // HOST_NAME_MAX
#include <unistd.h>          // gethostname, getpid, sleep
#endif // BCAST_TRAP_DBG

void bcast (char * message, int count, int iroot, MPI_Comm comm);
void show_usage (FILE * stream, const char * programname);


void bcast (char * message, int count, int iroot, MPI_Comm comm) {
    int irank = -1;
    int nranks = -1;

    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    if (irank == iroot) {
        for (int idst = 0; idst < nranks; idst++) {
            if (idst == irank) continue;
            MPI_Send(message, count, MPI_CHAR, idst, 0, comm);
        }
    } else {
        MPI_Status status = {};
        MPI_Recv(message, count, MPI_CHAR, iroot, 0, comm, &status);
    }
}

int main (int argc, char * argv[]) {

    MPI_Init(&argc, (char ***) &argv);

#ifdef BCAST_TRAP_DBG
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
#endif // BCAST_TRAP_DBG

    int irank = -1;
    int nranks = -1;

    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    if (argc == 2) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            if (irank == 0) {
                show_usage(stdout, argv[0]);
            }
            MPI_Finalize();
            return EXIT_SUCCESS;
        } else {
            if (irank == 0) {
                show_usage(stderr, argv[0]);
                MPI_Abort(MPI_COMM_WORLD, -1);
            }
            // wait for rank 0's printing
            MPI_Barrier(MPI_COMM_WORLD);
            return EXIT_FAILURE;
        }
    }
    if (argc != 1) {
        if (irank == 0) {
            show_usage(stderr, argv[0]);
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
        // wait for rank 0's printing
        MPI_Barrier(MPI_COMM_WORLD);
        return EXIT_FAILURE;
    }

    // specify the buffer size of the message to be broadcast
    constexpr int nchars = 11 + 1;

    // allocate space for the message that we are going to broadcast
    char * msg = calloc(nchars, sizeof(int));

    // specify which rank is the root
    const int iroot = 0;

    // only on root, initialize the message
    if (irank == iroot) {
        strcpy(msg, "hello bcast");
    } else {
        strcpy(msg, "...........");
    }

    // let each process print the contents of its `msg` before broadcasting
    fprintf(stdout, "before (%d) %s\n", irank, msg);

    // help output messages in order (success not guaranteed)
    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);

    // depending on irank, send or receive the bcast
    bcast(msg, nchars, iroot, MPI_COMM_WORLD);

    // let each process print the contents of its `msg` after broadcasting
    fprintf(stdout, "after  (%d) %s\n", irank, msg);

    // terminate mpi execution environment
    MPI_Finalize();

    return EXIT_SUCCESS;
}


void show_usage (FILE * stream, const char * programname) {
    fprintf(stream,
            "Usage: mpirun -np N %s\n"
            "\n"
            "    Broadcast a message to N processes.\n"
            "\n", programname);
}
