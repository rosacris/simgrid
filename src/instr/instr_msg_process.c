/* Copyright (c) 2010. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
  * under the terms of the license (GNU LGPL) which comes with this package. */

#include "instr/instr_private.h"

#ifdef HAVE_TRACING

static xbt_dict_t process_containers = NULL;

char *TRACE_process_alias_container(m_process_t process, m_host_t host,
                                    char *output, int len)
{
  if (output) {
    snprintf(output, len, "%p-%p", process, host);
    return output;
  } else {
    return NULL;
  }
}

char *TRACE_process_container(m_process_t process, char *output, int len)
{
  if (output) {
    snprintf(output, len, "%s-%p", MSG_process_get_name(process), process);
    return output;
  } else {
    return NULL;
  }
}

void TRACE_msg_process_alloc(void)
{
  process_containers = xbt_dict_new();
}

void TRACE_msg_process_release(void)
{
  xbt_dict_free(&process_containers);
}

static void TRACE_msg_process_location(m_process_t process)
{
  char name[200], alias[200];
  m_host_t host = NULL;
  if (!(TRACE_msg_process_is_enabled() || TRACE_msg_volume_is_enabled()))
    return;

  host = MSG_process_get_host(process);
  TRACE_process_container(process, name, 200);
  TRACE_process_alias_container(process, host, alias, 200);

  //check if process_alias container is already created
  if (!xbt_dict_get_or_null(process_containers, alias)) {
    pajeCreateContainer(MSG_get_clock(), alias, "PROCESS",
                        MSG_host_get_name(host), name);
    if (TRACE_msg_process_is_enabled())
      pajeSetState(MSG_get_clock(), "category", alias, process->category);
    xbt_dict_set(process_containers, xbt_strdup(alias), xbt_strdup("1"),
                 xbt_free);
  }
}

static void TRACE_msg_process_present(m_process_t process)
{
  char alias[200];
  m_host_t host = NULL;
  if (!TRACE_msg_process_is_enabled())
    return;

  //updating presence state of this process location
  host = MSG_process_get_host(process);
  TRACE_process_alias_container(process, host, alias, 200);
  pajePushState(MSG_get_clock(), "presence", alias, "presence");
}

/*
 * TRACE_msg_set_process_category: tracing interface function
 */
void TRACE_msg_set_process_category(m_process_t process,
                                    const char *category)
{
  char name[200];
  if (!TRACE_is_active())
    return;

  //set process category
  process->category = xbt_strdup(category);

  //create container of type "PROCESS" to indicate location
  TRACE_msg_process_location(process);
  TRACE_msg_process_present(process);

  //create container of type "process" to indicate behavior
  TRACE_process_container(process, name, 200);
  if (TRACE_msg_process_is_enabled())
    pajeCreateContainer(MSG_get_clock(), name, "process", category, name);
  if (TRACE_msg_process_is_enabled())
    pajeSetState(MSG_get_clock(), "process-state", name, "executing");
}

/*
 * Instrumentation functions to trace MSG processes (m_process_t)
 */
void TRACE_msg_process_change_host(m_process_t process, m_host_t old_host,
                                   m_host_t new_host)
{
  char alias[200];
  if (!(TRACE_msg_process_is_enabled() || TRACE_msg_volume_is_enabled()) || !process->category)
    return;

  //disabling presence in old_host (__TRACE_msg_process_not_present)
  TRACE_process_alias_container(process, old_host, alias, 200);
  if (TRACE_msg_process_is_enabled())
    pajePopState(MSG_get_clock(), "presence", alias);

  TRACE_msg_process_location(process);
  TRACE_msg_process_present(process);
}

void TRACE_msg_process_kill(m_process_t process)
{
  char name[200];
  if (!TRACE_msg_process_is_enabled() || !process->category)
    return;

  TRACE_process_container(process, name, 200);
  pajeDestroyContainer(MSG_get_clock(), "process", name);
}

void TRACE_msg_process_suspend(m_process_t process)
{
  char name[200];
  if (!TRACE_msg_process_is_enabled() || !process->category)
    return;

  TRACE_process_container(process, name, 200);
  pajeSetState(MSG_get_clock(), "process-state", name, "suspend");
}

void TRACE_msg_process_resume(m_process_t process)
{
  char name[200];
  if (!TRACE_msg_process_is_enabled() || !process->category)
    return;

  TRACE_process_container(process, name, 200);
  pajeSetState(MSG_get_clock(), "process-state", name, "executing");
}

void TRACE_msg_process_sleep_in(m_process_t process)
{
  char name[200];
  if (!TRACE_msg_process_is_enabled() || !process->category)
    return;

  TRACE_process_container(process, name, 200);
  pajeSetState(MSG_get_clock(), "process-state", name, "sleep");
}

void TRACE_msg_process_sleep_out(m_process_t process)
{
  char name[200];
  if (!TRACE_msg_process_is_enabled() || !process->category)
    return;

  TRACE_process_container(process, name, 200);
  pajeSetState(MSG_get_clock(), "process-state", name, "executing");
}

void TRACE_msg_process_end(m_process_t process)
{
  char name[200], alias[200];
  m_host_t host = NULL;
  if (!!process->category)
    return;

  host = MSG_process_get_host(process);
  TRACE_process_container(process, name, 200);
  TRACE_process_alias_container(process, host, alias, 200);
  if (TRACE_msg_process_is_enabled())
    pajeDestroyContainer(MSG_get_clock(), "process", name);
  if (TRACE_msg_process_is_enabled())
    pajeDestroyContainer(MSG_get_clock(), "PROCESS", alias);
}

#endif /* HAVE_TRACING */