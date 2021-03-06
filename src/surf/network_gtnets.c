/* Copyright (c) 2007, 2008, 2009, 2010. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include "network_gtnets_private.h"
#include "gtnets/gtnets_interface.h"
#include "xbt/str.h"

static double time_to_next_flow_completion = -1;

XBT_LOG_NEW_DEFAULT_SUBCATEGORY(surf_network_gtnets, surf,
                                "Logging specific to the SURF network GTNetS module");

extern routing_global_t global_routing;

double sg_gtnets_jitter = 0.0;
int sg_gtnets_jitter_seed = 10;

static void link_new(char *name, double bw, double lat, xbt_dict_t props)
{
  static int link_count = -1;
  network_link_GTNETS_t gtnets_link;

  if (xbt_lib_get_or_null(link_lib, name, SURF_LINK_LEVEL)) {
    return;
  }

  XBT_DEBUG("Scanning link name %s", name);


  gtnets_link = xbt_new0(s_network_link_GTNETS_t, 1);
  gtnets_link->generic_resource.name = name;
  gtnets_link->generic_resource.properties = props;
  gtnets_link->bw_current = bw;
  gtnets_link->lat_current = lat;

  link_count++;

  XBT_DEBUG("Adding new link, linkid %d, name %s, latency %g, bandwidth %g",
           link_count, name, lat, bw);

  if (gtnets_add_link(link_count, bw, lat)) {
	  xbt_die("Cannot create GTNetS link");
  }
  gtnets_link->id = link_count;

  xbt_lib_set(link_lib, name, SURF_LINK_LEVEL, gtnets_link);
}

static void route_new(int src_id, int dst_id, xbt_dynar_t links,
                      int nb_link)
{
  network_link_GTNETS_t link;
  unsigned int cursor;
  int i = 0;
  int *gtnets_links;

  XBT_IN("(src_id=%d, dst_id=%d, links=%p, nb_link=%d)",
          src_id, dst_id, links, nb_link);

  /* Build the list of gtnets link IDs */
  gtnets_links = (int *) calloc(nb_link, sizeof(int));
  i = 0;
  xbt_dynar_foreach(links, cursor, link) {
    gtnets_links[i++] = link->id;
  }

  if (gtnets_add_route(src_id, dst_id, gtnets_links, nb_link)) {
    xbt_die("Cannot create GTNetS route");
  }
  XBT_OUT();
}

static void route_onehop_new(int src_id, int dst_id,
                             network_link_GTNETS_t link)
{
  if (gtnets_add_onehop_route(src_id, dst_id, link->id)) {
    xbt_die("Cannot create GTNetS route");
  }
}

/* Parse the XML for a network link */
static void parse_link_init(void)
{
  char *name;
  double bw;
  double lat;
  e_surf_resource_state_t state;
  name = xbt_strdup(A_surfxml_link_id);
  surf_parse_get_double(&bw, A_surfxml_link_bandwidth);
  surf_parse_get_double(&lat, A_surfxml_link_latency);
  state = SURF_RESOURCE_ON;
  XBT_DEBUG("link_gtnets");
  tmgr_trace_t bw_trace;
  tmgr_trace_t state_trace;
  tmgr_trace_t lat_trace;

  bw_trace = tmgr_trace_new(A_surfxml_link_bandwidth_file);
  lat_trace = tmgr_trace_new(A_surfxml_link_latency_file);
  state_trace = tmgr_trace_new(A_surfxml_link_state_file);

  if (bw_trace)
    XBT_INFO
        ("The GTNetS network model doesn't support bandwidth state traces");
  if (lat_trace)
    XBT_INFO("The GTNetS network model doesn't support latency state traces");
  if (state_trace)
    XBT_INFO("The GTNetS network model doesn't support link state traces");

  if (A_surfxml_link_sharing_policy == A_surfxml_link_sharing_policy_FULLDUPLEX)
  {
	  link_new(bprintf("%s_UP",name), bw, lat, current_property_set);
	  link_new(bprintf("%s_DOWN",name), bw, lat, current_property_set);

  }
  else  link_new(name, bw, lat, current_property_set);
  current_property_set = NULL;
}

/* Create the gtnets topology based on routing strategy */
static void create_gtnets_topology()
{
  int src_id,dst_id;

   XBT_DEBUG("Starting topology generation");
// À refaire plus tard. Il faut prendre la liste des hôtes/routeurs (dans routing)
// À partir de cette liste, on les numérote.
// Ensuite, on peut utiliser les id pour refaire les appels GTNets qui suivent.

   //get the onelinks from the parsed platform
   xbt_dynar_t onelink_routes = global_routing->get_onelink_routes();
   if (!onelink_routes)
     return;

   //save them in trace file
   onelink_t onelink;
   unsigned int iter;
   xbt_dynar_foreach(onelink_routes, iter, onelink) {
     char *src = onelink->src;
     char *dst = onelink->dst;
     void *link = onelink->link_ptr;
     src_id = *((int *) xbt_dict_get_or_null(global_routing->root->to_index,src));
     dst_id = *((int *) xbt_dict_get_or_null(global_routing->root->to_index,dst));

     if(src_id != dst_id){
     XBT_DEBUG("Link (#%p), src (#%s), dst (#%s), src_id = %d, dst_id = %d", link,src,dst, src_id, dst_id);
     XBT_DEBUG("Calling one link route");
        if(global_routing->get_network_element_type(src) == SURF_NETWORK_ELEMENT_ROUTER){
        	gtnets_add_router(src_id);
        }
        if(global_routing->get_network_element_type(dst) == SURF_NETWORK_ELEMENT_ROUTER){
         gtnets_add_router(dst_id);
        }
        route_onehop_new(src_id, dst_id, (network_link_GTNETS_t)(link));
     }
   }

   if (XBT_LOG_ISENABLED(surf_network_gtnets, xbt_log_priority_debug)) {
        gtnets_print_topology();
   }
}

/* Main XML parsing */
static void define_callbacks(const char *file)
{
  /* Figuring out the network links */
  surfxml_add_callback(ETag_surfxml_link_cb_list, &parse_link_init);
  surfxml_add_callback(ETag_surfxml_platform_cb_list,
                       &create_gtnets_topology);
}

static int resource_used(void *resource_id)
{
  xbt_die("The resource_used feature is not implemented in GTNets model");
}

static int action_unref(surf_action_t action)
{
  action->refcount--;
  if (!action->refcount) {
    xbt_swag_remove(action, action->state_set);
#ifdef HAVE_TRACING
    if (action->category)
      xbt_free(action->category);
#endif
    surf_action_free(&action);
    return 1;
  }
  return 0;
}

static void action_cancel(surf_action_t action)
{
  xbt_die("Cannot cancel GTNetS flow");
  return;
}

static void action_recycle(surf_action_t action)
{
  xbt_die("Cannot recycle GTNetS flow");
  return;
}

static double action_get_remains(surf_action_t action)
{
  return action->remains;
}

static void action_state_set(surf_action_t action,
                             e_surf_action_state_t state)
{
  surf_action_state_set(action, state);
}

static double share_resources(double now)
{
  xbt_swag_t running_actions =
      surf_network_model->states.running_action_set;

  //get the first relevant value from the running_actions list
  if (!xbt_swag_size(running_actions))
    return -1.0;

  xbt_assert(time_to_next_flow_completion,
              "Time to next flow completion not initialized!\n");

  XBT_DEBUG("Calling gtnets_get_time_to_next_flow_completion");
  time_to_next_flow_completion = gtnets_get_time_to_next_flow_completion();
  XBT_DEBUG("gtnets_get_time_to_next_flow_completion received %lg",
         time_to_next_flow_completion);

  return time_to_next_flow_completion;
}

static void update_actions_state(double now, double delta)
{
  surf_action_network_GTNETS_t action = NULL;
  xbt_swag_t running_actions =
      surf_network_model->states.running_action_set;

  /* If there are no renning flows, just return */
  if (time_to_next_flow_completion < 0.0) {
    return;
  }

  /* if delta == time_to_next_flow_completion, too. */
  if (time_to_next_flow_completion <= delta) {  /* run until the first flow completes */
    void **metadata;
    int i, num_flows;

    num_flows = 0;

    if (gtnets_run_until_next_flow_completion(&metadata, &num_flows)) {
      xbt_die("Cannot run GTNetS simulation until next flow completion");
    }
    if (num_flows < 1) {
      xbt_die("GTNetS simulation couldn't find a flow that would complete");
    }

    xbt_swag_foreach(action, running_actions) {
      XBT_DEBUG("Action (%p) remains old value: %f", action,
             action->generic_action.remains);
      double sent = gtnets_get_flow_rx(action);

#ifdef HAVE_TRACING
      double trace_sent = sent;
      if (trace_sent == 0) {
        //if sent is equals to 0, means that gtnets sent all the bytes
        trace_sent = action->generic_action.cost;
      }
      // tracing resource utilization
// COMMENTED BY DAVID
//       int src = TRACE_surf_gtnets_get_src (action);
//       int dst = TRACE_surf_gtnets_get_dst (action);
//       if (src != -1 && dst != -1){
//         xbt_dynar_t route = used_routing->get_route(src, dst);
//         network_link_GTNETS_t link;
//         unsigned int i;
//         xbt_dynar_foreach(route, i, link) {
//              TRACE_surf_link_set_utilization (link->generic_resource.name,
//             action->generic_action.data, trace_sent/delta, now-delta, delta);
//         }
//       }
#endif

      XBT_DEBUG("Sent value returned by GTNetS : %f", sent);
      //need to trust this remain value
      if (sent == 0) {
        action->generic_action.remains = 0;
      } else {
        action->generic_action.remains =
            action->generic_action.cost - sent;
      }
      XBT_DEBUG("Action (%p) remains new value: %f", action,
             action->generic_action.remains);
    }

    for (i = 0; i < num_flows; i++) {
      action = (surf_action_network_GTNETS_t) (metadata[i]);

      action->generic_action.finish = now + time_to_next_flow_completion;
#ifdef HAVE_TRACING
      TRACE_surf_gtnets_destroy(action);
#endif
      action_state_set((surf_action_t) action, SURF_ACTION_DONE);
      XBT_DEBUG("----> Action (%p) just terminated", action);
    }


  } else {                      /* run for a given number of seconds */
    if (gtnets_run(delta)) {
      xbt_die("Cannot run GTNetS simulation");
    }
  }

  return;
}

static void update_resource_state(void *id,
                                  tmgr_trace_event_t event_type,
                                  double value, double date)
{
  xbt_die("Cannot update model state for GTNetS simulation");
}

/* Max durations are not supported */
static surf_action_t communicate(const char *src_name,
                                 const char *dst_name, double size,
                                 double rate)
{
  int src, dst;

  // Utiliser le dictionnaire définit dans create_gtnets_topology pour initialiser correctement src et dst
  src = dst = -1;
  surf_action_network_GTNETS_t action = NULL;

  src = *((int *) xbt_dict_get_or_null(global_routing->root->to_index,src_name));
  dst = *((int *) xbt_dict_get_or_null(global_routing->root->to_index,dst_name));
  xbt_assert((src >= 0
               && dst >= 0), "Either src or dst have invalid id (id<0)");

  XBT_DEBUG("Setting flow src %d \"%s\", dst %d \"%s\"", src, src_name, dst,
         dst_name);

  xbt_dynar_t links = global_routing->get_route(src_name, dst_name);
  route_new(src, dst, links, xbt_dynar_length(links));

  action =
      surf_action_new(sizeof(s_surf_action_network_GTNETS_t), size,
                      surf_network_model, 0);

  /* Add a flow to the GTNets Simulation, associated to this action */
  if (gtnets_create_flow(src, dst, size, (void *) action) < 0) {
    xbt_die("Not route between host %s and host %s", src_name, dst_name);
  }
#ifdef HAVE_TRACING
  TRACE_surf_gtnets_communicate(action, src, dst);
#endif

  return (surf_action_t) action;
}

/* Suspend a flow() */
static void action_suspend(surf_action_t action)
{
  THROW_UNIMPLEMENTED;
}

/* Resume a flow() */
static void action_resume(surf_action_t action)
{
  THROW_UNIMPLEMENTED;
}

/* Test whether a flow is suspended */
static int action_is_suspended(surf_action_t action)
{
  return 0;
}

static void finalize(void)
{
  gtnets_finalize();
}

static void surf_network_model_init_internal(void)
{
  surf_network_model = surf_model_init();

  surf_network_model->name = "network GTNetS";
  surf_network_model->action_unref = action_unref;
  surf_network_model->action_cancel = action_cancel;
  surf_network_model->action_recycle = action_recycle;
  surf_network_model->action_state_set = action_state_set;
  surf_network_model->get_remains = action_get_remains;

  surf_network_model->model_private->resource_used = resource_used;
  surf_network_model->model_private->share_resources = share_resources;
  surf_network_model->model_private->update_actions_state =
      update_actions_state;
  surf_network_model->model_private->update_resource_state =
      update_resource_state;
  surf_network_model->model_private->finalize = finalize;

  surf_network_model->suspend = action_suspend;
  surf_network_model->resume = action_resume;
  surf_network_model->is_suspended = action_is_suspended;

  surf_network_model->extension.network.communicate = communicate;

  /* Added the initialization for GTNetS interface */
  if (gtnets_initialize(sg_tcp_gamma)) {
    xbt_die("Impossible to initialize GTNetS interface");
  }

  routing_model_create(sizeof(network_link_GTNETS_t), NULL, NULL);
}

#ifdef HAVE_LATENCY_BOUND_TRACKING
static int get_latency_limited(surf_action_t action)
{
  return 0;
}
#endif

#ifdef HAVE_GTNETS
void surf_network_model_init_GTNETS(const char *filename)
{
  if (surf_network_model)
    return;
  surf_network_model_init_internal();
  define_callbacks(filename);
  xbt_dynar_push(model_list, &surf_network_model);

#ifdef HAVE_LATENCY_BOUND_TRACKING
  surf_network_model->get_latency_limited = get_latency_limited;
#endif

  if (sg_gtnets_jitter > 0.0) {
    gtnets_set_jitter(sg_gtnets_jitter);
    gtnets_set_jitter_seed(sg_gtnets_jitter_seed);
  }

  update_model_description(surf_network_model_description,
                           "GTNets", surf_network_model);
}
#endif
