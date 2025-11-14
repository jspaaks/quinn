#ifndef ROWS_READER_H_INCLUDED
#define ROWS_READER_H_INCLUDED
#include <mpi.h>
#include <stddef.h>


/*  
 *  incompletely defined / opaque data structure that serves as the context for most functions
 *  defined below
 */
struct rows_reader;


/* release the resources associated with the instance of `struct rows_reader` */
void rows_reader_delete (struct rows_reader ** self);


/* returns the size required for allocating an instance of `struct rows_reader` */
size_t rows_reader_get_size (void);


/* initialize the members of an instance of `struct rows_reader` with values */
void rows_reader_init (struct rows_reader * self, const char * filepath, MPI_Comm mpi_comm);


/*
 *  dynamically allocate memory for storing an instance of `struct rows_reader`, and return a
 *  pointer to it
 */
struct rows_reader * rows_reader_new (void);


/*
 *  use the highest-rank process to read IDX formatted binary data from file self->filepath, and
 *  send horizontal slices of it to the other processes
 */
void rows_reader_read (struct rows_reader * self);

#endif
