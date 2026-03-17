#include <stdio.h>
#include <stdint.h>
#include <mpi.h>
#ifdef PINGPONG_TRAP_DBG
#include <unistd.h>
#endif // PINGPONG_TRAP_DBG
#include <stdlib.h>

void terminate (int irank, const char * program_name);

struct msg {
    int nbytes;
    char * msg;
    MPI_Datatype mpi_datatype;
};

int main (int argc, char * argv[]) {

    MPI_Init(&argc, &argv);

#ifdef FLOYD_PAR_TRAP_DBG
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
#endif // FLOYD_PAR_TRAP_DBG

    int irank = -1;
    int nranks = -1;

    MPI_Comm_rank(MPI_COMM_WORLD, &irank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    if (argc != 2 || nranks != 2) {
        terminate(irank, argv[0]);
        return EXIT_FAILURE;
    }

    struct msg small = {
        .nbytes = 1,
        .msg = calloc(1, 1),
        .mpi_datatype = MPI_UINT8_T,
    };
    struct msg large = {
        .nbytes = 1000000,
        .msg = calloc(1, 1000000),
        .mpi_datatype = MPI_UINT8_T,
    };

    if (small.msg == nullptr || large.msg == nullptr) {
        if (irank == 0) {
            fprintf(stderr, "Problem allocating memory, aborting.\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
        MPI_Barrier(MPI_COMM_WORLD);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    const int ileft = 0;
    const int iright = 1;
    MPI_Status status = {};
    double starttime = -1;
    double endtime = -1;
    if (irank == 0) {
        starttime = MPI_Wtime();
        for (int i = 0; i < n; i++) {
            MPI_Send(small.msg, small.nbytes, small.mpi_datatype, iright, 0, MPI_COMM_WORLD);
            MPI_Recv(small.msg, small.nbytes, small.mpi_datatype, iright, 0, MPI_COMM_WORLD, &status);
        }
        endtime = MPI_Wtime();
        printf("%d ping-pongs of 1 byte took %f seconds\n", n, endtime - starttime);

        starttime = MPI_Wtime();
        for (int i = 0; i < n; i++) {
            MPI_Send(large.msg, large.nbytes, large.mpi_datatype, iright, 0, MPI_COMM_WORLD);
            MPI_Recv(large.msg, large.nbytes, large.mpi_datatype, iright, 0, MPI_COMM_WORLD, &status);
        }
        endtime = MPI_Wtime();
        printf("%d ping-pongs of 1 MB took %f seconds\n", n, endtime - starttime);
    } else {
        for (int i = 0; i < n; i++) {
            MPI_Recv(small.msg, small.nbytes, small.mpi_datatype, ileft, 0, MPI_COMM_WORLD, &status);
            MPI_Send(small.msg, small.nbytes, small.mpi_datatype, ileft, 0, MPI_COMM_WORLD);
        }
        for (int i = 0; i < n; i++) {
            MPI_Recv(large.msg, large.nbytes, large.mpi_datatype, ileft, 0, MPI_COMM_WORLD, &status);
            MPI_Send(large.msg, large.nbytes, large.mpi_datatype, ileft, 0, MPI_COMM_WORLD);
        }
    }

    free(large.msg);
    large.msg = nullptr;

    free(small.msg);
    small.msg = nullptr;

    MPI_Finalize();
    return EXIT_SUCCESS;
}


void terminate (int irank, const char * program_name) {
    if (irank == 0) {
        fprintf(stderr,
                "Usage: mpirun -np 2 %s N\n"
                "    Multiprocess program that goes back and forth between its 2 processes\n"
                "    for a total of N times, subsequently reporting average latency and\n"
                "    bandwidth statistics\n", program_name);
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    MPI_Barrier(MPI_COMM_WORLD);
}