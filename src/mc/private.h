/* 	$Id: private.h 5497 2008-05-26 12:19:15Z cristianrosa $	 */

/* Copyright (c) 2007 Arnaud Legrand, Bruno Donnassolo.
   All rights reserved.                                          */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#ifndef MC_PRIVATE_H
#define MC_PRIVATE_H

#include <stdio.h>
#include <sys/mman.h>
#include "mc/mc.h"
#include "mc/datatypes.h"
#include "xbt/fifo.h"
#include "xbt/config.h"
#include "xbt/function_types.h"
#include "xbt/mmalloc.h"
#include "../simix/private.h"

/****************************** Snapshots ***********************************/

typedef struct s_mc_mem_region{
  void *start_addr;
  void *data;
  size_t size;
} s_mc_mem_region_t, *mc_mem_region_t;

typedef struct s_mc_snapshot{
  unsigned int num_reg;
  mc_mem_region_t *regions;
} s_mc_snapshot_t, *mc_snapshot_t;

void MC_take_snapshot(mc_snapshot_t);
void MC_restore_snapshot(mc_snapshot_t);
void MC_free_snapshot(mc_snapshot_t);

/********************************* MC Global **********************************/
extern double *mc_time;

/* Bound of the MC depth-first search algorithm */
#define MAX_DEPTH 1000

int MC_deadlock_check(void);
void MC_replay(xbt_fifo_t stack);
void MC_wait_for_requests(void);
void MC_get_enabled_processes();
void MC_show_deadlock(smx_req_t req);
void MC_show_stack(xbt_fifo_t stack);
void MC_dump_stack(xbt_fifo_t stack);

/********************************* Requests ***********************************/
int MC_request_depend(smx_req_t req1, smx_req_t req2);
char* MC_request_to_string(smx_req_t req, int value);
unsigned int MC_request_testany_fail(smx_req_t req);
/*int MC_waitany_is_enabled_by_comm(smx_req_t req, unsigned int comm);*/
int MC_request_is_visible(smx_req_t req);
int MC_request_is_enabled(smx_req_t req);
int MC_request_is_enabled_by_idx(smx_req_t req, unsigned int idx);
int MC_process_is_enabled(smx_process_t process);

/********************************** DPOR **************************************/
void MC_dpor_init(void);
void MC_dpor(void);
void MC_dpor_exit(void);

/******************************** States **************************************/
/* Possible exploration status of a process in a state */
typedef enum {
  MC_NOT_INTERLEAVE=0,      /* Do not interleave (do not execute) */
  MC_INTERLEAVE,            /* Interleave the process (one or more request) */
  MC_DONE                   /* Already interleaved */
} e_mc_process_state_t;

/* On every state, each process has an entry of the following type */
typedef struct mc_procstate{
  e_mc_process_state_t state;       /* Exploration control information */
  unsigned int interleave_count;    /* Number of times that the process was
                                       interleaved */
} s_mc_procstate_t, *mc_procstate_t;

/* An exploration state is composed of: */
typedef struct mc_state {
  unsigned long max_pid;            /* Maximum pid at state's creation time */
  mc_procstate_t proc_status;       /* State's exploration status by process */
  s_smx_action_t internal_comm;     /* To be referenced by the internal_req */
  s_smx_req_t internal_req;         /* Internal translation of request */
  s_smx_req_t executed_req;         /* The executed request of the state */
  int req_num;                      /* The request number (in the case of a
                                       multi-request like waitany ) */
} s_mc_state_t, *mc_state_t;

extern xbt_fifo_t mc_stack;

mc_state_t MC_state_new(void);
void MC_state_delete(mc_state_t state);
void MC_state_interleave_process(mc_state_t state, smx_process_t process);
unsigned int MC_state_interleave_size(mc_state_t state);
int MC_state_process_is_done(mc_state_t state, smx_process_t process);
void MC_state_set_executed_request(mc_state_t state, smx_req_t req, int value);
smx_req_t MC_state_get_executed_request(mc_state_t state, int *value);
smx_req_t MC_state_get_internal_request(mc_state_t state);
smx_req_t MC_state_get_request(mc_state_t state, int *value);

/****************************** Statistics ************************************/
typedef struct mc_stats {
  unsigned long state_size;
  unsigned long visited_states;
  unsigned long expanded_states;
  unsigned long executed_transitions;
} s_mc_stats_t, *mc_stats_t;

extern mc_stats_t mc_stats;

void MC_print_statistics(mc_stats_t);

/********************************** MEMORY ******************************/
/* The possible memory modes for the modelchecker are standard and raw. */
/* Normally the system should operate in std, for switching to raw mode */
/* you must wrap the code between MC_SET_RAW_MODE and MC_UNSET_RAW_MODE */

extern void *std_heap;
extern void *raw_heap;
int raw_heap_fd;
#define STD_HEAP_SIZE   20480000        /* Maximum size of the system's heap */

/* FIXME: Horrible hack! because the mmalloc library doesn't provide yet of */
/* an API to query about the status of a heap, we simply call mmstats and */
/* because I now how does structure looks like, then I redefine it here */

struct mstats {
  size_t bytes_total;           /* Total size of the heap. */
  size_t chunks_used;           /* Chunks allocated by the user. */
  size_t bytes_used;            /* Byte total of user-allocated chunks. */
  size_t chunks_free;           /* Chunks in the free list. */
  size_t bytes_free;            /* Byte total of chunks in the free list. */
};

#define MC_SET_RAW_MEM    mmalloc_set_current_heap(raw_heap)
#define MC_UNSET_RAW_MEM    mmalloc_set_current_heap(std_heap)

/******************************* MEMORY MAPPINGS ***************************/
/* These functions and data structures implements a binary interface for   */
/* the proc maps ascii interface                                           */

/* Each field is defined as documented in proc's manual page  */
typedef struct s_map_region {

  void *start_addr;             /* Start address of the map */
  void *end_addr;               /* End address of the map */
  int prot;                     /* Memory protection */
  int flags;                    /* Aditional memory flags */
  void *offset;                 /* Offset in the file/whatever */
  char dev_major;               /* Major of the device */
  char dev_minor;               /* Minor of the device */
  unsigned long inode;          /* Inode in the device */
  char *pathname;               /* Path name of the mapped file */

} s_map_region;

typedef struct s_memory_map {

  s_map_region *regions;        /* Pointer to an array of regions */
  int mapsize;                  /* Number of regions in the memory */

} s_memory_map_t, *memory_map_t;

memory_map_t get_memory_map(void);


#endif
