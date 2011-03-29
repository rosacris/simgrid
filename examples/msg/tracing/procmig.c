/* 	$Id$	 */

/* Copyright (c) 2009 The SimGrid team. All rights reserved.                */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include "msg/msg.h"            /* core library */
#include "xbt/sysdep.h"         /* calloc */

/* Create a log channel to have nice outputs. */
#include "xbt/log.h"
XBT_LOG_NEW_DEFAULT_CATEGORY(msg_test,
                             "Messages specific for this msg example");

/** The guy we will move from host to host. It move alone and then is moved by policeman back  */
static int emigrant(int argc, char *argv[])
{
  m_task_t task = NULL;
  char *destination = NULL;

  MSG_process_sleep(2);

  while (1){ // I am an eternal emigrant
    MSG_task_receive(&(task), "master_mailbox");
    destination = (char*)MSG_task_get_data (task);
    if (!destination) break; //there is no destination, die
    XBT_INFO("Migrating to %s", destination);
    MSG_process_migrate(MSG_process_self(), MSG_get_host_by_name(destination));
    MSG_process_sleep(2); // I am tired, have to sleep for 2 seconds
    xbt_free (destination);
    MSG_task_destroy (task);
    task = NULL;
  }
  return 0;
}

static int master(int argc, char *argv[])
{
  m_task_t task = NULL;

  // I am the master of emigrant process,
  // I tell it where it must emigrate to.
  xbt_dynar_t destinations = xbt_dynar_new (sizeof(char*), xbt_free);
  xbt_dynar_push_as (destinations, char*, xbt_strdup ("Tremblay"));
  xbt_dynar_push_as (destinations, char*, xbt_strdup ("Jupiter"));
  xbt_dynar_push_as (destinations, char*, xbt_strdup ("Fafard"));
  xbt_dynar_push_as (destinations, char*, xbt_strdup ("Ginette"));
  xbt_dynar_push_as (destinations, char*, xbt_strdup ("Bourassa"));
  xbt_dynar_push_as (destinations, char*, xbt_strdup ("Fafard"));
  xbt_dynar_push_as (destinations, char*, xbt_strdup ("Tremblay"));
  xbt_dynar_push_as (destinations, char*, xbt_strdup ("Ginette"));
  xbt_dynar_push_as (destinations, char*, NULL);

  char *destination;
  unsigned int i;
  xbt_dynar_foreach(destinations, i, destination){
    task = MSG_task_create("task", 0, 0, NULL);
    if (destination){
      MSG_task_set_data(task, xbt_strdup (destination));
    }
    TRACE_msg_set_task_category(task, "migration_order");
    MSG_task_send (task, "master_mailbox");
    task = NULL;
  }
  return 0;
}

/** Main function */
int main(int argc, char *argv[])
{
  MSG_error_t res = MSG_OK;

  /* Argument checking */
  MSG_global_init(&argc, argv);
  if (argc < 3) {
    XBT_CRITICAL("Usage: %s platform_file deployment_file\n", argv[0]);
    XBT_CRITICAL("example: %s msg_platform.xml msg_deployment_suspend.xml\n",
              argv[0]);
    exit(1);
  }

  /* Simulation setting */
  MSG_create_environment(argv[1]);

  TRACE_category ("migration_order");

  /* Application deployment */
  MSG_function_register("emigrant", emigrant);
  MSG_function_register("master", master);
  MSG_launch_application(argv[2]);

  /* Run the simulation */
  res = MSG_main();
  XBT_INFO("Simulation time %g", MSG_get_clock());
  if (res == MSG_OK)
    res = MSG_clean();

  if (res == MSG_OK)
    return 0;
  else
    return 1;
}                               /* end_of_main */
