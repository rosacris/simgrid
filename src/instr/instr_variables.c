/* Copyright (c) 2010. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
  * under the terms of the license (GNU LGPL) which comes with this package. */

#include "instr/instr_private.h"
#include "surf/surf_private.h"
#include "surf/network_private.h"

#ifdef HAVE_TRACING

extern routing_global_t global_routing;

void TRACE_user_link_variable(double time, const char *src,
                              const char *dst, const char *variable,
                              double value, const char *what)
{
  if (!TRACE_is_active() || !TRACE_platform_is_enabled())
    return;

  char valuestr[100];
  snprintf(valuestr, 100, "%g", value);

  if (strcmp(what, "declare") == 0) {
    pajeDefineVariableType(variable, "LINK", variable);
    return;
  }

  if (!global_routing)
    return;

  xbt_dynar_t route = global_routing->get_route(src, dst);
  unsigned int i;
  void *link_ptr;
  xbt_dynar_foreach(route, i, link_ptr) {
    char resource[100];
    snprintf(resource, 100, "%p", link_ptr);

    if (strcmp(what, "set") == 0) {
      pajeSetVariable(time, variable, resource, valuestr);
    } else if (strcmp(what, "add") == 0) {
      pajeAddVariable(time, variable, resource, valuestr);
    } else if (strcmp(what, "sub") == 0) {
      pajeSubVariable(time, variable, resource, valuestr);
    }
  }
}

void TRACE_user_host_variable(double time, const char *variable,
                              double value, const char *what)
{
  char valuestr[100];
  if (!TRACE_is_active() || !TRACE_platform_is_enabled())
    return;

  snprintf(valuestr, 100, "%g", value);

  if (strcmp(what, "declare") == 0) {
    pajeDefineVariableType(variable, "HOST", variable);
  } else if (strcmp(what, "set") == 0) {
    pajeSetVariable(time, variable, MSG_host_self()->name, valuestr);
  } else if (strcmp(what, "add") == 0) {
    pajeAddVariable(time, variable, MSG_host_self()->name, valuestr);
  } else if (strcmp(what, "sub") == 0) {
    pajeSubVariable(time, variable, MSG_host_self()->name, valuestr);
  }
}

#endif /* HAVE_TRACING */