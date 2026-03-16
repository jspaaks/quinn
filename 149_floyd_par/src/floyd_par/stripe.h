#ifndef STRIPE_READER_H_INCLUDED
#define STRIPE_READER_H_INCLUDED
#include <mpi.h>
#include <stddef.h>
#include <stdio.h>


/*  
 *  incompletely defined / opaque data structure that serves as the context for most functions
 *  defined below
 */
struct stripe;


/* release the resources associated with the instance of `struct stripe` */
void stripe_delete (struct stripe ** self);

/* returns the row number where this process' slice of the matrix starts */
uint32_t stripe_get_irow0 (const struct stripe * self);

/* returns the row number where this process' slice of the matrix ends (inclusive) */
uint32_t stripe_get_irown (const struct stripe * self);

/* returns the number of cols in the entire matrix */
uint32_t stripe_get_ncols (const struct stripe * self);

/* returns the number of rows in the entire matrix */
uint32_t stripe_get_nrows (const struct stripe * self);

/* returns the matrix element value at irow, icol */
uint8_t stripe_get_val (const struct stripe * self, uint32_t irow, uint32_t icol);

/*
 *  dynamically allocate memory for storing an instance of `struct stripe`, and return a
 *  pointer to it
 */
struct stripe * stripe_new (void);

/* print the contents of matrix to stream */
void stripe_print (const struct stripe * self, FILE * stream);

/*
 *  use the highest-rank process to read IDX formatted binary data from file self->filepath, and
 *  send horizontal slices of it to the other processes
 */
void stripe_read_u8 (struct stripe * self, const char * filepath, MPI_Comm mpi_comm);

/* set matrix element at irow, icol to val */
void stripe_set_val (struct stripe * self, uint32_t irow, uint32_t icol, uint8_t val);

#endif
