/**
 * $Id$tag 
 *
 * smpi_coll_private.h -- functions of smpi_coll.c that are exported to other SMPI modules.
 *
 *
 *
 **/
#include "private.h"

int nary_tree_bcast(void *buf, int count, MPI_Datatype datatype, int root, MPI_Comm comm,int arity);
int nary_tree_barrier( MPI_Comm comm, int arity );
