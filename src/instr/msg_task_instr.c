/*
 * msg.c
 *
 *  Created on: Nov 24, 2009
 *      Author: Lucas Schnorr
 *     License: This program is free software; you can redistribute
 *              it and/or modify it under the terms of the license
 *              (GNU LGPL) which comes with this package.
 *
 *     Copyright (c) 2009 The SimGrid team.
 */

#include "instr/private.h"

#ifdef HAVE_TRACING

static xbt_dict_t current_task_category = NULL;

void __TRACE_msg_init (void)
{
  current_task_category = xbt_dict_new();
}

void __TRACE_current_category_set (m_task_t task)
{
  char processid[100];
  snprintf (processid, 100, "%p", SIMIX_process_self());
  xbt_dict_set (current_task_category, processid, xbt_strdup (task->category), xbt_free);
}

void __TRACE_current_category_unset ()
{
  char processid[100];
  snprintf (processid, 100, "%p", SIMIX_process_self());
  xbt_dict_remove (current_task_category, processid);
}

char *__TRACE_current_category_get (smx_process_t proc)
{
  char processid[100];
  snprintf (processid, 100, "%p", proc);
  return xbt_dict_get_or_null (current_task_category, processid);
}

void __TRACE_task_location (m_task_t task)
{
  char container[200];
  m_process_t process = MSG_process_self();
  m_host_t host = MSG_process_get_host (process);
  if (IS_TRACING_PROCESSES){
	//container is a process
	TRACE_process_alias_container (process, host, container, 200);
	__TRACE_msg_process_location (process);
  }else{
	//container is a host
	TRACE_host_container (host, container, 200);
  }

  char name[200], alias[200];
  TRACE_task_container (task, name, 200);
  TRACE_task_alias_container (task, process, host, alias, 200);
  if (IS_TRACING_TASKS) pajeCreateContainer (MSG_get_clock(), alias, "TASK", container, name);
}

/*
 * TRACE_msg_set_task_category: tracing interface function
 */
void TRACE_msg_set_task_category(m_task_t task, const char *category)
{
  if (!IS_TRACING) return;

  //set task category
  task->category = xbt_new (char, strlen (category)+1);
  strncpy(task->category, category, strlen(category)+1);

  char name[200];//, alias[200], process_alias[200];
  TRACE_task_container (task, name, 200);
  //create container of type "task" to indicate behavior
  if (IS_TRACING_TASKS) pajeCreateContainer (MSG_get_clock(), name, "task", category, name);
  if (IS_TRACING_TASKS) pajePushState (MSG_get_clock(), "task-state", name, "created");

  //tracing task location based on process/host
  __TRACE_task_location (task);
}

/* MSG_task_create related function*/
void TRACE_msg_task_create (m_task_t task)
{
  static long long counter = 0;
  task->counter = counter++;
  task->category = NULL;
}

/* MSG_task_execute related functions */
void TRACE_msg_task_execute_start (m_task_t task)
{
  if (!IS_TRACING || !IS_TRACED(task)) return;

  char name[200];
  TRACE_task_container (task, name, 200);
  if (IS_TRACING_TASKS) pajePushState (MSG_get_clock(), "task-state", name, "execute");

  __TRACE_current_category_set (task);
}

void TRACE_msg_task_execute_end (m_task_t task)
{
  if (!IS_TRACING || !IS_TRACED(task)) return;

  char name[200];
  TRACE_task_container (task, name, 200);
  if (IS_TRACING_TASKS) pajePopState (MSG_get_clock(), "task-state", name);

  __TRACE_current_category_unset();
}

/* MSG_task_destroy related functions */
void TRACE_msg_task_destroy (m_task_t task)
{
  if (!IS_TRACING || !IS_TRACED(task)) return;

  char name[200];
  TRACE_task_container (task, name, 200);
  if (IS_TRACING_TASKS) pajeDestroyContainer (MSG_get_clock(), "task", name);

  //free category
  xbt_free (task->category);
  return;
}

/* MSG_task_get related functions */
void TRACE_msg_task_get_start (void)
{
  if (!IS_TRACING) return;
}

void TRACE_msg_task_get_end (double start_time, m_task_t task)
{
  if (!IS_TRACING || !IS_TRACED(task)) return;

  char name[200];
  TRACE_task_container (task, name, 200);
  if (IS_TRACING_TASKS) pajePopState (MSG_get_clock(), "task-state", name);

  //tracing task location based on process/host
  __TRACE_task_location (task);
}

/* MSG_task_put related functions */
int TRACE_msg_task_put_start (m_task_t task)
{
  if (!IS_TRACING || !IS_TRACED(task)) return 0;

  char name[200];
  TRACE_task_container (task, name, 200);
  if (IS_TRACING_TASKS) pajePopState (MSG_get_clock(), "task-state", name);
  if (IS_TRACING_TASKS) pajePushState (MSG_get_clock(), "task-state", name, "communicate");

  __TRACE_current_category_set (task);
  return 1;
}

void TRACE_msg_task_put_end (void)
{
  if (!IS_TRACING) return;

  __TRACE_current_category_unset ();
}

#endif