/* Copyright (c) 2007, 2009, 2010. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include "msg/msg.h"            /* Yeah! If you want to use msg, you need to include msg/msg.h */
#include "xbt/sysdep.h"         /* calloc */

/* Create a log channel to have nice outputs. */
#include "xbt/log.h"
XBT_LOG_NEW_DEFAULT_CATEGORY(msg_test,
                             "Messages specific for this msg example");

typedef enum {
  PORT_22 = 0,
  MAX_CHANNEL
} channel_t;


/** Lazy guy function. This process suspends itself asap.  */
static int lazy_guy(int argc, char *argv[])
{
  XBT_INFO("Nobody's watching me ? Let's go to sleep.");
  MSG_process_suspend(MSG_process_self());
  XBT_INFO("Uuuh ? Did somebody call me ?");
  XBT_INFO("Mmmh, goodbye now.");
  return 0;
}                               /* end_of_lazy_guy */

/** Dream master function. This process creates a lazy_guy process and
    resumes it 10 seconds later. */
static int dream_master(int argc, char *argv[])
{
  m_process_t lazy = NULL;

  XBT_INFO("Let's create a lazy guy.");
  lazy = MSG_process_create("Lazy", lazy_guy, NULL, MSG_host_self());
  XBT_INFO("Let's wait a little bit...");
  MSG_process_sleep(10.0);
  XBT_INFO("Let's wake the lazy guy up! >:) BOOOOOUUUHHH!!!!");
  MSG_process_resume(lazy);
  XBT_INFO("OK, goodbye now.");
  return 0;
}                               /* end_of_dram_master */

/** Test function */
static MSG_error_t test_all(const char *platform_file,
                            const char *application_file)
{
  MSG_error_t res = MSG_OK;

  {                             /*  Simulation setting */
    MSG_set_channel_number(MAX_CHANNEL);
    MSG_create_environment(platform_file);
  }
  {                             /*   Application deployment */
    MSG_function_register("dream_master", dream_master);
    MSG_launch_application(application_file);
  }
  res = MSG_main();

  XBT_INFO("Simulation time %g", MSG_get_clock());
  return res;
}                               /* end_of_test_all */


/** Main function */
int main(int argc, char *argv[])
{
  MSG_error_t res = MSG_OK;

  MSG_global_init(&argc, argv);
  if (argc < 3) {
    XBT_CRITICAL("Usage: %s platform_file deployment_file\n", argv[0]);
    XBT_CRITICAL("example: %s msg_platform.xml msg_deployment_suspend.xml\n",
              argv[0]);
    exit(1);
  }
  test_all(argv[1], argv[2]);
  res = MSG_clean();

  if (res == MSG_OK)
    return 0;
  else
    return 1;
}                               /* end_of_main */
