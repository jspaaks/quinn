#include "blkdcmp/blkdcmp.h"
#include "idx/idx.h"
#include "rows_reader.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

struct rows_reader {
    const char * filepath;
    MPI_Comm mpi_comm;
    uint8_t * buffer;
    uint8_t ** matrix;
};

void rows_reader_delete (struct rows_reader ** self) {
    free((*self)->buffer);
    (*self)->buffer = nullptr;

    free((*self)->matrix);
    (*self)->matrix = nullptr;

    free(*self);
    *self = nullptr;
}

size_t rows_reader_get_size (void) {
    return sizeof(struct rows_reader);
}

void rows_reader_init (struct rows_reader * self, const char * filepath, MPI_Comm mpi_comm) {
    *self = (struct rows_reader) {
        .filepath = filepath,
        .mpi_comm = mpi_comm,
        .buffer = nullptr,
        .matrix = nullptr,
    };
}

struct rows_reader * rows_reader_new (void) {
    struct rows_reader * reader = (struct rows_reader *) malloc(1 * sizeof(struct rows_reader));
    if (reader == nullptr) {
        fprintf(stderr, "Error allocating memory for struct rows_reader, aborting.\n");
        exit(1);
    }   
    return reader;
}

void rows_reader_read (struct rows_reader * self) {
    int irank = 0;
    int nranks = 0;

    // figure out how many ranks there are and which i am
    MPI_Comm_rank(self->mpi_comm, &irank);
    MPI_Comm_size(self->mpi_comm, &nranks);

    // use the last process to read the metadata
    IdxHeader header = {};
    if (irank == nranks - 1) {
        header = idx_read_header(self->filepath);
    }

    // broadcast the header to the other processes
    MPI_Bcast(&header, sizeof(header), MPI_BYTE, nranks-1, MPI_COMM_WORLD);

    uint32_t nrows = header.lengths[0];
    uint32_t ncols = header.lengths[1];

    // determine the number of rows that are going to be processed by this process
    size_t blk_sz = blkdcmp_get_blk_sz((size_t) nrows, (size_t) nranks, (size_t) irank);

    // allocate the right amount of memory for this process
    self->buffer = (uint8_t *) calloc(blk_sz * ncols, sizeof(uint8_t));
    self->matrix = (uint8_t **) calloc(blk_sz, sizeof(uint8_t *));
    for (size_t i = 0; i < blk_sz; i++) {
        self->matrix[i] = &self->buffer[i * ncols];
    }

    if (irank == nranks - 1) {
        // open the IDX file with the adjacency matrix
        FILE * stream = fopen(self->filepath, "rb");
        // move the cursor to the beginning of the data
        fseek(stream, header.bodystart, SEEK_SET);
        // read blocks of data and send it to the target process 0..n-2
        for (int i = 0; i < nranks - 1; ++i) {
            size_t tgt_blk_sz = blkdcmp_get_blk_sz((size_t) nrows, (size_t) nranks, (size_t) i);
            fread(self->buffer, sizeof(uint8_t), tgt_blk_sz * ncols, stream);
            MPI_Send(self->buffer, tgt_blk_sz * ncols, MPI_UINT8_T, i, 0, MPI_COMM_WORLD);
        }
        // read the last block of rows from file, forgo sending it to yourself
        fread(self->buffer, sizeof(uint8_t), blk_sz * ncols, stream);
        // close the file
        fclose(stream);
    } else {
        // receive the data from process nranks - 1
        MPI_Status status = {};
        MPI_Recv(self->buffer, blk_sz * ncols, MPI_UINT8_T, nranks - 1, 0, MPI_COMM_WORLD, &status);
    }
}
