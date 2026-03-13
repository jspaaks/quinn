#include "blkdcmp/blkdcmp.h"
#include "idx/idx.h"
#include "stripe.h"
#include <assert.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct stripe {
    uint8_t * buffer;
    uint32_t irow0;
    uint32_t irown;
    uint8_t ** matrix;
    uint32_t ncols;
    uint32_t ncolsg;
    uint32_t nrows;
    uint32_t nrowsg;
};

void stripe_delete (struct stripe ** self) {
    free((*self)->buffer);
    (*self)->buffer = nullptr;

    free((*self)->matrix);
    (*self)->matrix = nullptr;

    free(*self);
    *self = nullptr;
}

uint32_t stripe_get_irow0 (struct stripe * self) {
    return self->irow0;
}

uint32_t stripe_get_irown (struct stripe * self) {
    return self->irown;
}

uint32_t stripe_get_ncols (struct stripe * self) {
    return self->ncolsg;
}

uint32_t stripe_get_nrows (struct stripe * self) {
    return self->nrowsg;
}

uint8_t stripe_get_val (struct stripe * self, uint32_t irowg, uint32_t icolg) {
    uint32_t irow = irowg - self->irow0;
    return self->matrix[irow][icolg];
}

struct stripe * stripe_new (void) {
    struct stripe * reader = (struct stripe *) calloc(1, sizeof(struct stripe));
    if (reader == nullptr) {
        fprintf(stderr, "Error allocating memory for struct stripe, aborting.\n");
        exit(1);
    }
    return reader;
}

void stripe_read_u8 (struct stripe * self, const char * filepath, MPI_Comm mpi_comm) {
    int irank = -1;
    int nranks = -1;

    // figure out how many ranks there are and which i am
    MPI_Comm_rank(mpi_comm, &irank);
    MPI_Comm_size(mpi_comm, &nranks);

    // use the last process to read the metadata
    IdxHeader header = {};
    if (irank == nranks - 1) {
        header = idx_read_header(filepath);
    }

    // broadcast the header to the other processes
    MPI_Bcast(&header, sizeof(header), MPI_BYTE, nranks-1, MPI_COMM_WORLD);

    self->nrowsg = header.lengths[0];
    self->ncolsg = header.lengths[1];

    if (irank == 0 && self->nrowsg != self->ncolsg) {
        printf("Expected matrix to be square, aborting\n");
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    {
        bool incorrect_data_type = strcmp(idx_get_type_name(&header), "uint8") != 0;
        if (irank == 0 && incorrect_data_type) {
            fprintf(stderr, "Expected data type of '%s' body to be uint8_t, aborting.\n", filepath);
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
    }

    // determine the number of rows that are going to be processed by this process
    self->nrows = blkdcmp_get_blk_sz((size_t) self->nrowsg, (size_t) nranks, (size_t) irank);
    self->ncols = self->ncolsg;

    // determine the row numbers where the stripe starts and ends
    self->irow0 = blkdcmp_get_idx_blk_s((size_t) self->nrowsg, (size_t) nranks, (size_t) irank);
    self->irown = blkdcmp_get_idx_blk_e((size_t) self->nrowsg, (size_t) nranks, (size_t) irank);

    // allocate the right amount of memory for this process
    self->buffer = (uint8_t *) calloc(self->nrows * self->ncols, sizeof(uint8_t));
    self->matrix = (uint8_t **) calloc(self->nrows, sizeof(uint8_t *));
    for (size_t i = 0; i < self->nrows; i++) {
        self->matrix[i] = &self->buffer[i * self->ncols];
    }

    if (irank == nranks - 1) {
        // open the IDX file with the adjacency matrix
        FILE * stream = fopen(filepath, "rb");
        // move the cursor to the beginning of the data
        fseek(stream, header.bodystart, SEEK_SET);
        // read blocks of data and send it to the target process 0..n-2
        for (int i = 0; i < nranks - 1; ++i) {
            size_t tgt_blk_sz = blkdcmp_get_blk_sz((size_t) self->nrowsg, (size_t) nranks, (size_t) i);
            fread(self->buffer, sizeof(uint8_t), tgt_blk_sz * self->ncols, stream);
            MPI_Send(self->buffer, tgt_blk_sz * self->ncols, MPI_UINT8_T, i, 0, MPI_COMM_WORLD);
        }
        // read the last block of rows from file, forgo sending it to yourself
        fread(self->buffer, sizeof(uint8_t), self->nrows * self->ncols, stream);
        // close the file
        fclose(stream);
    } else {
        // receive the data from process nranks - 1
        MPI_Status status = {};
        MPI_Recv(self->buffer, self->nrows * self->ncols, MPI_UINT8_T, nranks - 1, 0, MPI_COMM_WORLD, &status);
    }
}

void stripe_set_val (struct stripe * self, uint32_t irowg, uint32_t icolg, uint8_t val) {
    uint32_t irow = irowg - self->irow0;
    self->matrix[irow][icolg] = val;
}
