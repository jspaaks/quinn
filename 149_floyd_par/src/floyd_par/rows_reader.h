#ifndef ROWS_READER_H_INCLUDED
#define ROWS_READER_H_INCLUDED
#include <mpi.h>
#include <stddef.h>


/*  
 *  incompletely defined / opaque data structure that serves as the context for most functions
 *  defined below
 */
struct rows;


/* release the resources associated with the instance of `struct rows` */
void rows_reader_delete (struct rows ** self);


/* returns the size required for allocating an instance of `struct rows` */
size_t rows_reader_get_size (void);


/*
 *  dynamically allocate memory for storing an instance of `struct rows`, and return a
 *  pointer to it
 */
struct rows * rows_reader_new (void);


/*
 *  use the highest-rank process to read IDX formatted binary data from file self->filepath, and
 *  send horizontal slices of it to the other processes
 */
void rows_reader_read (struct rows * self, const char * filepath, MPI_Comm mpi_comm);

#endif
