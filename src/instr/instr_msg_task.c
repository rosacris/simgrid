/* Copyright (c) 2010. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
  * under the terms of the license (GNU LGPL) which comes with this package. */

#include "instr/instr_private.h"

#ifdef HAVE_TRACING

XBT_LOG_NEW_DEFAULT_SUBCATEGORY (instr_msg, instr, "MSG");

/*
 * TRACE_msg_set_task_category: tracing interface function
 */
void TRACE_msg_set_task_category(m_task_t task, const char *category)
{
  if (!TRACE_is_active())
    return;

  xbt_assert(task->category == NULL, "Task %p(%s) already has a category (%s).",
      task, task->name, task->category);
  if (TRACE_msg_task_is_enabled()){
    xbt_assert(task->name != NULL,
        "Task %p(%s) must have a unique name in order to be traced, if --cfg=tracing/msg/task:1 is used.",
        task, task->name);
    xbt_assert(getContainer(task->name)==NULL,
        "Task %p(%s). Tracing already knows a task with name %s."
        "The name of each task must be unique, if --cfg=tracing/msg/task:1 is used.", task, task->name, task->name);
  }

  if (category == NULL) {
    //if user provides a NULL category, task is no longer traced
    xbt_free (task->category);
    task->category = NULL;
    return;
  }

  //set task category
  task->category = xbt_strdup (category);
  XBT_DEBUG("MSG task %p(%s), category %s", task, task->name, task->category);

  if (TRACE_msg_task_is_enabled()){
    m_host_t host = MSG_host_self();
    container_t host_container = getContainer(host->name);
    //check to see if there is a container with the task->name
    xbt_assert(getContainer(task->name) == NULL,
        "Task %p(%s). Tracing already knows a task with name %s."
        "The name of each task must be unique, if --cfg=tracing/msg/task:1 is used.", task, task->name, task->name);
    container_t msg = newContainer(task->name, INSTR_MSG_TASK, host_container);
    type_t type = getType (task->category, msg->type);
    if (!type){
      type = getVariableType(task->category, NULL, msg->type);
    }
    new_pajeSetVariable (SIMIX_get_clock(), msg, type, 1);

    type = getType ("MSG_TASK_STATE", msg->type);
    val_t value = getValueByName ("created", type);
    new_pajePushState (MSG_get_clock(), msg, type, value);
  }
}

/* MSG_task_create related function*/
void TRACE_msg_task_create(m_task_t task)
{
  static long long counter = 0;
  task->counter = counter++;
  task->category = NULL;
  XBT_DEBUG("CREATE %p, %lld", task, task->counter);
}

/* MSG_task_execute related functions */
void TRACE_msg_task_execute_start(m_task_t task)
{
  XBT_DEBUG("EXEC,in %p, %lld, %s", task, task->counter, task->category);

  if (TRACE_msg_task_is_enabled()){
    container_t task_container = getContainer (task->name);
    type_t type = getType ("MSG_TASK_STATE", task_container->type);
    val_t value = getValueByName ("MSG_task_execute", type);
    new_pajePushState (MSG_get_clock(), task_container, type, value);
  }

  if (TRACE_msg_process_is_enabled()){
    int len = INSTR_DEFAULT_STR_SIZE;
    char str[INSTR_DEFAULT_STR_SIZE];

    container_t process_container = getContainer (instr_process_id(MSG_process_self(), str, len));
    type_t type = getType ("MSG_PROCESS_STATE", process_container->type);
    val_t value = getValueByName ("task_execute", type);
    new_pajePushState (MSG_get_clock(), process_container, type, value);
  }
}

void TRACE_msg_task_execute_end(m_task_t task)
{
  XBT_DEBUG("EXEC,out %p, %lld, %s", task, task->counter, task->category);

  if (TRACE_msg_task_is_enabled()){
    container_t task_container = getContainer (task->name);
    type_t type = getType ("MSG_TASK_STATE", task_container->type);
    new_pajePopState (MSG_get_clock(), task_container, type);
  }

  if (TRACE_msg_process_is_enabled()){
    int len = INSTR_DEFAULT_STR_SIZE;
    char str[INSTR_DEFAULT_STR_SIZE];

    container_t process_container = getContainer (instr_process_id(MSG_process_self(), str, len));
    type_t type = getType ("MSG_PROCESS_STATE", process_container->type);
    new_pajePopState (MSG_get_clock(), process_container, type);
  }
}

/* MSG_task_destroy related functions */
void TRACE_msg_task_destroy(m_task_t task)
{
  XBT_DEBUG("DESTROY %p, %lld, %s", task, task->counter, task->category);

  if (TRACE_msg_task_is_enabled()){
    //that's the end, let's destroy it
    destroyContainer (getContainer(task->name));
  }

  //free category
  xbt_free(task->category);
  task->category = NULL;
  return;
}

/* MSG_task_get related functions */
void TRACE_msg_task_get_start(void)
{
  XBT_DEBUG("GET,in");

  if (TRACE_msg_task_is_enabled()){
    //task not received yet, nothing to do
  }

  if (TRACE_msg_process_is_enabled()){
    int len = INSTR_DEFAULT_STR_SIZE;
    char str[INSTR_DEFAULT_STR_SIZE];

    container_t process_container = getContainer (instr_process_id(MSG_process_self(), str, len));
    type_t type = getType ("MSG_PROCESS_STATE", process_container->type);
    val_t value = getValueByName ("receive", type);
    new_pajePushState (MSG_get_clock(), process_container, type, value);
  }
}

void TRACE_msg_task_get_end(double start_time, m_task_t task)
{
  XBT_DEBUG("GET,out %p, %lld, %s", task, task->counter, task->category);

  if (TRACE_msg_task_is_enabled()){

    //FIXME
    //if (TRACE_msg_volume_is_enabled()){
    //  TRACE_msg_volume_end(task);
    //}

    m_host_t host = MSG_host_self();
    container_t host_container = getContainer(host->name);
    container_t msg = newContainer(task->name, INSTR_MSG_TASK, host_container);
    type_t type = getType (task->category, msg->type);
    new_pajeSetVariable (SIMIX_get_clock(), msg, type, 1);

    type = getType ("MSG_TASK_STATE", msg->type);
    val_t value = getValueByName ("created", type);
    new_pajePushState (MSG_get_clock(), msg, type, value);

    type = getType ("MSG_TASK_LINK", getRootType());
    char key[INSTR_DEFAULT_STR_SIZE];
    snprintf (key, INSTR_DEFAULT_STR_SIZE, "%lld", task->counter);
    new_pajeEndLink (MSG_get_clock(), getRootContainer(), type, msg, "SR", key);
  }

  if (TRACE_msg_process_is_enabled()){
    int len = INSTR_DEFAULT_STR_SIZE;
    char str[INSTR_DEFAULT_STR_SIZE];

    container_t process_container = getContainer (instr_process_id(MSG_process_self(), str, len));
    type_t type = getType ("MSG_PROCESS_STATE", process_container->type);
    new_pajePopState (MSG_get_clock(), process_container, type);

    char key[INSTR_DEFAULT_STR_SIZE];
    snprintf (key, INSTR_DEFAULT_STR_SIZE, "p%lld", task->counter);
    type = getType ("MSG_PROCESS_TASK_LINK", getRootType());
    new_pajeEndLink(MSG_get_clock(), getRootContainer(), type, process_container, "SR", key);
  }
}

/* MSG_task_put related functions */
int TRACE_msg_task_put_start(m_task_t task)
{
  XBT_DEBUG("PUT,in %p, %lld, %s", task, task->counter, task->category);

  if (TRACE_msg_task_is_enabled()){

    container_t msg = getContainer (task->name);
    type_t type = getType ("MSG_TASK_STATE", msg->type);
    new_pajePopState (MSG_get_clock(), msg, type);

    type = getType ("MSG_TASK_LINK", getRootType());
    char key[INSTR_DEFAULT_STR_SIZE];
    snprintf (key, INSTR_DEFAULT_STR_SIZE, "%lld", task->counter);
    new_pajeStartLink(MSG_get_clock(), getRootContainer(), type, msg, "SR", key);

    destroyContainer (msg);

    //FIXME
    //if (TRACE_msg_volume_is_enabled()){
    //  TRACE_msg_volume_start(task);
    //}
  }

  if (TRACE_msg_process_is_enabled()){
    int len = INSTR_DEFAULT_STR_SIZE;
    char str[INSTR_DEFAULT_STR_SIZE];

    container_t process_container = getContainer (instr_process_id(MSG_process_self(), str, len));
    type_t type = getType ("MSG_PROCESS_STATE", process_container->type);
    val_t value = getValueByName ("send", type);
    new_pajePushState (MSG_get_clock(), process_container, type, value);

    char key[INSTR_DEFAULT_STR_SIZE];
    snprintf (key, INSTR_DEFAULT_STR_SIZE, "p%lld", task->counter);
    type = getType ("MSG_PROCESS_TASK_LINK", getRootType());
    new_pajeStartLink(MSG_get_clock(), getRootContainer(), type, process_container, "SR", key);
  }

  return 1;
}

void TRACE_msg_task_put_end(void)
{
  XBT_DEBUG("PUT,out");

  if (TRACE_msg_task_is_enabled()){
    //task no longer exists here
  }

  if (TRACE_msg_process_is_enabled()){
    int len = INSTR_DEFAULT_STR_SIZE;
    char str[INSTR_DEFAULT_STR_SIZE];

    container_t process_container = getContainer (instr_process_id(MSG_process_self(), str, len));
    type_t type = getType ("MSG_PROCESS_STATE", process_container->type);
    new_pajePopState (MSG_get_clock(), process_container, type);
  }
}

#endif /* HAVE_TRACING */
