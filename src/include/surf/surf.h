/* Copyright (c) 2004, 2005, 2006, 2007, 2008, 2009, 2010. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#ifndef _SURF_SURF_H
#define _SURF_SURF_H

#include "xbt/swag.h"
#include "xbt/dynar.h"
#include "xbt/dict.h"
#include "xbt/misc.h"
#include "portable.h"
#include "xbt/config.h"
#include "surf/datatypes.h"
#include "xbt/lib.h"

xbt_lib_t host_lib;
int ROUTING_HOST_LEVEL; //Routing level
int	SURF_CPU_LEVEL;		//Surf cpu level
int SURF_WKS_LEVEL;		//Surf workstation level
int SIMIX_HOST_LEVEL;	//Simix level
int	MSG_HOST_LEVEL;		//Msg level
int	SD_HOST_LEVEL;		//Simdag level
int	COORD_HOST_LEVEL;	//Coordinates level

xbt_lib_t link_lib;
int SD_LINK_LEVEL;		//Simdag level
int SURF_LINK_LEVEL;	//Surf level

xbt_lib_t as_router_lib;
int ROUTING_ASR_LEVEL;	//Routing level
int COORD_ASR_LEVEL;	//Coordinates level

SG_BEGIN_DECL()
/* Actions and models are highly connected structures... */
typedef enum {
  SURF_RESOURCE_ON = 1,                   /**< Up & ready        */
  SURF_RESOURCE_OFF = 0                   /**< Down & broken     */
} e_surf_resource_state_t;

typedef enum {
  SURF_LINK_FULLDUPLEX = 2,
  SURF_LINK_SHARED = 1,
  SURF_LINK_FATPIPE = 0
} e_surf_link_sharing_policy_t;

typedef enum {
  SURF_NETWORK_ELEMENT_NULL = 0,        /* NULL */
  SURF_NETWORK_ELEMENT_HOST,    /* host type */
  SURF_NETWORK_ELEMENT_ROUTER,  /* router type */
  SURF_NETWORK_ELEMENT_AS,      /* AS type */
} e_surf_network_element_type_t;

XBT_PUBLIC(e_surf_network_element_type_t) get_network_element_type(const char
                                                              *name);

/** @Brief Specify that we use that action */
XBT_PUBLIC(void) surf_action_ref(surf_action_t action);
/** @brief Creates a new action.
 *
 * @param size The size is the one of the subtype you want to create
 * @param cost initial value
 * @param model to which model we should attach this action
 * @param failed whether we should start this action in failed mode
 */
XBT_PUBLIC(void *) surf_action_new(size_t size, double cost,
                                   surf_model_t model, int failed);

/** \brief Resource model description
 */
typedef struct surf_model_description {
  const char *name;
  const char *description;
  surf_model_t model;
  void (*model_init_preparse) (const char *filename);
  void (*model_init_postparse) (void);
} s_surf_model_description_t, *surf_model_description_t;

XBT_PUBLIC(void) update_model_description(s_surf_model_description_t *
                                          table, const char *name,
                                          surf_model_t model);
XBT_PUBLIC(int) find_model_description(s_surf_model_description_t * table,
                                       const char *name);
XBT_PUBLIC(void) model_help(const char *category,
                            s_surf_model_description_t * table);

/** \brief Action structure
 * \ingroup SURF_actions
 *
 *  Never create s_surf_action_t by yourself ! The actions are created
 *  on the fly when you call execute or communicate on a model.
 *
 *  \see e_surf_action_state_t
 */
typedef struct surf_action {
  s_xbt_swag_hookup_t state_hookup;
  xbt_swag_t state_set;
  double cost;                  /**< cost        */
  double priority;              /**< priority (1.0 by default) */
  double max_duration;          /**< max_duration (may fluctuate until
				   the task is completed) */
  double remains;               /**< How much of that cost remains to
				 * be done in the currently running task */
#ifdef HAVE_LATENCY_BOUND_TRACKING
  int latency_limited;               /**< Set to 1 if is limited by latency, 0 otherwise */
#endif

  double start;                 /**< start time  */
  double finish;                /**< finish time : this is modified during the run
				 * and fluctuates until the task is completed */
  void *data;                   /**< for your convenience */
  int refcount;
  surf_model_t model_type;
#ifdef HAVE_TRACING
  char *category;               /**< tracing category for categorized resource utilization monitoring */
#endif
} s_surf_action_t;

typedef struct surf_action_lmm {
  s_surf_action_t generic_action;
  lmm_variable_t variable;
  int suspended;
} s_surf_action_lmm_t, *surf_action_lmm_t;

/** \brief Action states
 *  \ingroup SURF_actions
 *
 *  Action states.
 *
 *  \see surf_action_t, surf_action_state_t
 */
typedef enum {
  SURF_ACTION_READY = 0,        /**< Ready        */
  SURF_ACTION_RUNNING,          /**< Running      */
  SURF_ACTION_FAILED,           /**< Task Failure */
  SURF_ACTION_DONE,             /**< Completed    */
  SURF_ACTION_TO_FREE,          /**< Action to free in next cleanup */
  SURF_ACTION_NOT_IN_THE_SYSTEM
                                /**< Not in the system anymore. Why did you ask ? */
} e_surf_action_state_t;

/** \brief Action state sets
 *  \ingroup SURF_actions
 *
 *  This structure contains some sets of actions.
 *  It provides a fast access to the actions in each state.
 *
 *  \see surf_action_t, e_surf_action_state_t
 */
typedef struct surf_action_state {
  xbt_swag_t ready_action_set;
                                 /**< Actions in state SURF_ACTION_READY */
  xbt_swag_t running_action_set;
                                 /**< Actions in state SURF_ACTION_RUNNING */
  xbt_swag_t failed_action_set;
                                 /**< Actions in state SURF_ACTION_FAILED */
  xbt_swag_t done_action_set;
                                 /**< Actions in state SURF_ACTION_DONE */
} s_surf_action_state_t, *surf_action_state_t;

/***************************/
/* Generic model object */
/***************************/
typedef struct s_routing_global s_routing_global_t, *routing_global_t;
XBT_PUBLIC_DATA(routing_global_t) global_routing;


/** \brief Private data available on all models
 *  \ingroup SURF_models
 */
typedef struct surf_model_private *surf_model_private_t;

     /* Cpu model */

     /** \brief CPU model extension public
      *  \ingroup SURF_models
      *
      *  Public functions specific to the CPU model.
      */
typedef struct surf_cpu_model_extension_public {
  surf_action_t(*execute) (void *cpu, double size);
  surf_action_t(*sleep) (void *cpu, double duration);
  e_surf_resource_state_t(*get_state) (void *cpu);
  double (*get_speed) (void *cpu, double load);
  double (*get_available_speed) (void *cpu);
  void (*create_resource) (char *name, double power_peak,
                           double power_scale,
                           tmgr_trace_t power_trace,
                           int core,
                           e_surf_resource_state_t state_initial,
                           tmgr_trace_t state_trace,
                           xbt_dict_t cpu_properties);
  void (*add_traces) (void);
} s_surf_model_extension_cpu_t;

     /* Network model */

     /** \brief Network model extension public
      *  \ingroup SURF_models
      *
      *  Public functions specific to the network model
      */
typedef struct surf_network_model_extension_public {
  surf_action_t(*communicate) (const char *src_name,
                               const char *dst_name,
                               double size, double rate);
  xbt_dynar_t(*get_route) (const char *src_name, const char *dst_name);
  double (*get_link_bandwidth) (const void *link);
  double (*get_link_latency) (const void *link);
  int (*link_shared) (const void *link);
  void (*add_traces) (void);
  void (*create_resource) (char *name,
                           double bw_initial,
                           tmgr_trace_t bw_trace,
                           double lat_initial,
                           tmgr_trace_t lat_trace,
                           e_surf_resource_state_t
                           state_initial,
                           tmgr_trace_t state_trace,
                           e_surf_link_sharing_policy_t policy,
                           xbt_dict_t properties);
} s_surf_model_extension_network_t;

     /** \brief Workstation model extension public
      *  \ingroup SURF_models
      *
      *  Public functions specific to the workstation model.
      */
typedef struct surf_workstation_model_extension_public {
  surf_action_t(*execute) (void *workstation, double size);                                /**< Execute a computation amount on a workstation
                    									and create the corresponding action */
  surf_action_t(*sleep) (void *workstation, double duration);                              /**< Make a workstation sleep during a given duration */
  e_surf_resource_state_t(*get_state) (void *workstation);                                      /**< Return the CPU state of a workstation */
  double (*get_speed) (void *workstation, double load);                                    /**< Return the speed of a workstation */
  double (*get_available_speed) (void *workstation);                                       /**< Return tha available speed of a workstation */
   surf_action_t(*communicate) (void *workstation_src,                                     /**< Execute a communication amount between two workstations */
                                void *workstation_dst, double size,
                                double max_rate);
   xbt_dynar_t(*get_route) (void *workstation_src, void *workstation_dst);                 /**< Get the list of links between two ws */

   surf_action_t(*execute_parallel_task) (int workstation_nb,                              /**< Execute a parallel task on several workstations */
                                          void **workstation_list,
                                          double *computation_amount,
                                          double *communication_amount,
                                          double amount, double rate);
  double (*get_link_bandwidth) (const void *link);                                         /**< Return the current bandwidth of a network link */
  double (*get_link_latency) (const void *link);                                           /**< Return the current latency of a network link */
  int (*link_shared) (const void *link);
   xbt_dict_t(*get_properties) (const void *resource);
  void (*link_create_resource) (char *name,
                                double bw_initial,
                                tmgr_trace_t bw_trace,
                                double lat_initial,
                                tmgr_trace_t lat_trace,
                                e_surf_resource_state_t
                                state_initial,
                                tmgr_trace_t state_trace,
                                e_surf_link_sharing_policy_t
                                policy, xbt_dict_t properties);
  void (*cpu_create_resource) (char *name, double power_peak,
                               double power_scale,
                               tmgr_trace_t power_trace,
                               e_surf_resource_state_t state_initial,
                               tmgr_trace_t state_trace,
                               xbt_dict_t cpu_properties);
  void (*add_traces) (void);

} s_surf_model_extension_workstation_t;




/** \brief Model datatype
 *  \ingroup SURF_models
 *
 *  Generic data structure for a model. The workstations,
 *  the CPUs and the network links are examples of models.
 */
typedef struct surf_model {
  const char *name;     /**< Name of this model */
  s_surf_action_state_t states;      /**< Any living action on this model */

   e_surf_action_state_t(*action_state_get) (surf_action_t action);
                                                                       /**< Return the state of an action */
  void (*action_state_set) (surf_action_t action,
                            e_surf_action_state_t state);
                                                                  /**< Change an action state*/

  double (*action_get_start_time) (surf_action_t action);     /**< Return the start time of an action */
  double (*action_get_finish_time) (surf_action_t action);     /**< Return the finish time of an action */
  int (*action_unref) (surf_action_t action);     /**< Specify that we don't use that action anymore */
  void (*action_cancel) (surf_action_t action);     /**< Cancel a running action */
  void (*action_recycle) (surf_action_t action);     /**< Recycle an action */
  void (*action_data_set) (surf_action_t action, void *data);     /**< Set the user data of an action */
  void (*suspend) (surf_action_t action);     /**< Suspend an action */
  void (*resume) (surf_action_t action);     /**< Resume a suspended action */
  int (*is_suspended) (surf_action_t action);     /**< Return whether an action is suspended */
  void (*set_max_duration) (surf_action_t action, double duration);     /**< Set the max duration of an action*/
  void (*set_priority) (surf_action_t action, double priority);     /**< Set the priority of an action */
#ifdef HAVE_TRACING
  void (*set_category) (surf_action_t action, const char *category); /**< Set the category of an action */
#endif
  double (*get_remains) (surf_action_t action);     /**< Get the remains of an action */
#ifdef HAVE_LATENCY_BOUND_TRACKING
  int (*get_latency_limited) (surf_action_t action);     /**< Return 1 if action is limited by latency, 0 otherwise */
#endif

  surf_model_private_t model_private;

  union extension {
    s_surf_model_extension_cpu_t cpu;
    s_surf_model_extension_network_t network;
    s_surf_model_extension_workstation_t workstation;
  } extension;
} s_surf_model_t;

surf_model_t surf_model_init(void);
void surf_model_exit(surf_model_t model);

static inline void *surf_cpu_resource_by_name(const char *name) {
	return xbt_lib_get_or_null(host_lib, name, SURF_CPU_LEVEL);
}
static inline void *surf_workstation_resource_by_name(const char *name){
	return xbt_lib_get_or_null(host_lib, name, SURF_WKS_LEVEL);
}
static inline void *surf_network_resource_by_name(const char *name){
	return xbt_lib_get_or_null(link_lib, name, SURF_LINK_LEVEL);
}

typedef struct surf_resource {
  surf_model_t model;
  char *name;
  xbt_dict_t properties;
} s_surf_resource_t, *surf_resource_t;



/**
 * Resource which have a metric handled by a maxmin system
 */
typedef struct {
  double scale;
  double peak;
  tmgr_trace_event_t event;
} s_surf_metric_t;

typedef struct surf_resource_lmm {
  s_surf_resource_t generic_resource;
  lmm_constraint_t constraint;
  e_surf_resource_state_t state_current;
  tmgr_trace_event_t state_event;
  s_surf_metric_t power;
} s_surf_resource_lmm_t, *surf_resource_lmm_t;

/**************************************/
/* Implementations of model object */
/**************************************/


/** \brief The CPU model
 *  \ingroup SURF_models
 */
XBT_PUBLIC_DATA(surf_model_t) surf_cpu_model;

/** \brief Initializes the CPU model with the model Cas01
 *  \ingroup SURF_models
 *
 *  This function is called by surf_workstation_model_init_CLM03
 *  so you shouldn't have to call it by yourself.
 *
 *  \see surf_workstation_model_init_CLM03()
 */
XBT_PUBLIC(void) surf_cpu_model_init_Cas01(const char *filename);

/** \brief Initializes the CPU model with trace integration
 *  \ingroup SURF_models
 *
 */
XBT_PUBLIC(void) surf_cpu_model_init_ti(const char *filename);

/** \brief Initializes the CPU model with the model Cas01 Improved. This model uses a heap to order the events, decreasing the time complexity to get the minimum next event.
 *  \ingroup SURF_models
 *
 *  This function is called by surf_workstation_model_init_CLM03
 *  so you shouldn't have to call it by yourself.
 *
 *  \see surf_workstation_model_init_CLM03()
 */
XBT_PUBLIC(void) surf_cpu_model_init_Cas01_im(const char *filename);

/** \brief The list of all available cpu model models
 *  \ingroup SURF_models
 */
XBT_PUBLIC_DATA(s_surf_model_description_t) surf_cpu_model_description[];

XBT_PUBLIC(void) create_workstations(void);

/**\brief create new host bypass the parser
 *
 */


/** \brief The network model
 *  \ingroup SURF_models
 *
 *  When creating a new API on top on SURF, you shouldn't use the
 *  network model unless you know what you are doing. Only the workstation
 *  model should be accessed because depending on the platform model,
 *  the network model can be NULL.
 */
XBT_PUBLIC_DATA(surf_model_t) surf_network_model;

/** \brief Same as network model 'LagrangeVelho', only with different correction factors.
 *  \ingroup SURF_models
 *  \param filename XML platform file name
 *
 * This model is proposed by Pierre-Nicolas Clauss and Martin Quinson and Stéphane Génaud
 * based on the model 'LV08' and different correction factors depending on the communication
 * size (< 1KiB, < 64KiB, >= 64KiB).
 *
 *  \see surf_workstation_model_init_SMPI()
 */
XBT_PUBLIC(void) surf_network_model_init_SMPI(const char *filename);

/** \brief Initializes the platform with the network model 'LagrangeVelho'
 *  \ingroup SURF_models
 *  \param filename XML platform file name
 *
 * This model is proposed by Arnaud Legrand and Pedro Velho based on
 * the results obtained with the GTNets simulator for onelink and
 * dogbone sharing scenarios.
 *
 *  \see surf_workstation_model_init_LegrandVelho()
 */
XBT_PUBLIC(void) surf_network_model_init_LegrandVelho(const char
                                                      *filename);


/** \brief Initializes the platform with the network model 'LV08_im'
 *  \ingroup SURF_models
 *  \param filename XML platform file name
 *
 * This model is adds the lazy management improvement to Legrand and
 * Velho model. This improvement essentially replaces the list of actions
 * inside the simulation kernel by a heap in order to reduce the complexity
 * at each iteration of the simulation kernel.
 *
 *  \see surf_workstation_model_init_LegrandVelho()
 */
XBT_PUBLIC(void) im_surf_network_model_init_LegrandVelho(const char
                                                      *filename);

/** \brief Initializes the platform with the network model 'Constant'
 *  \ingroup SURF_models
 *  \param filename XML platform file name
 *
 *  In this model, the communication time between two network cards is
 *  constant, hence no need for a routing table. This is particularly
 *  usefull when simulating huge distributed algorithms where
 *  scalability is really an issue. This function is called in
 *  conjunction with surf_workstation_model_init_compound.
 *
 *  \see surf_workstation_model_init_compound()
 */
XBT_PUBLIC(void) surf_network_model_init_Constant(const char *filename);

/** \brief Initializes the platform with the network model CM02
 *  \ingroup SURF_models
 *  \param filename XML platform file name
 *
 *  This function is called by surf_workstation_model_init_CLM03
 *  or by yourself only if you plan using surf_workstation_model_init_compound
 *
 *  \see surf_workstation_model_init_CLM03()
 */
XBT_PUBLIC(void) surf_network_model_init_CM02(const char *filename);

/**
 * brief initialize the the network model bypassing the XML parser
 */
XBT_PUBLIC(void) surf_network_model_init_bypass(const char *id,
                                                double initial_bw,
                                                double initial_lat);

#ifdef HAVE_GTNETS
/** \brief Initializes the platform with the network model GTNETS
 *  \ingroup SURF_models
 *  \param filename XML platform file name
 *
 *  This function is called by surf_workstation_model_init_GTNETS
 *  or by yourself only if you plan using surf_workstation_model_init_compound
 *
 *  \see surf_workstation_model_init_GTNETS()
 */
XBT_PUBLIC(void) surf_network_model_init_GTNETS(const char *filename);
#endif

/** \brief Initializes the platform with the network model Reno
 *  \ingroup SURF_models
 *  \param filename XML platform file name
 *
 *  The problem is related to max( sum( arctan(C * Df * xi) ) ).
 *
 *  Reference:
 *  [LOW03] S. H. Low. A duality model of TCP and queue management algorithms.
 *  IEEE/ACM Transaction on Networking, 11(4):525-536, 2003.
 *
 *  Call this function only if you plan using surf_workstation_model_init_compound.
 *
 */
XBT_PUBLIC(void) surf_network_model_init_Reno(const char *filename);

/** \brief Initializes the platform with the network model Reno2
 *  \ingroup SURF_models
 *  \param filename XML platform file name
 *
 *  The problem is related to max( sum( arctan(C * Df * xi) ) ).
 *
 *  Reference:
 *  [LOW01] S. H. Low. A duality model of TCP and queue management algorithms.
 *  IEEE/ACM Transaction on Networking, 11(4):525-536, 2003.
 *
 *  Call this function only if you plan using surf_workstation_model_init_compound.
 *
 */
XBT_PUBLIC(void) surf_network_model_init_Reno2(const char *filename);

/** \brief Initializes the platform with the network model Vegas
 *  \ingroup SURF_models
 *  \param filename XML platform file name
 *
 *  This problem is related to max( sum( a * Df * ln(xi) ) ) which is equivalent
 *  to the proportional fairness.
 *
 *  Reference:
 *  [LOW03] S. H. Low. A duality model of TCP and queue management algorithms.
 *  IEEE/ACM Transaction on Networking, 11(4):525-536, 2003.
 *
 *  Call this function only if you plan using surf_workstation_model_init_compound.
 *
 */
XBT_PUBLIC(void) surf_network_model_init_Vegas(const char *filename);

/** \brief The list of all available network model models
 *  \ingroup SURF_models
 */
XBT_PUBLIC_DATA(s_surf_model_description_t)
    surf_network_model_description[];


/** \brief The workstation model
 *  \ingroup SURF_models
 *
 *  Note that when you create an API on top of SURF,
 *  the workstation model should be the only one you use
 *  because depending on the platform model, the network model and the CPU model
 *  may not exist.
 */
XBT_PUBLIC_DATA(surf_model_t) surf_workstation_model;

/** \brief Initializes the platform with a compound workstation model
 *  \ingroup SURF_models
 *  \param filename XML platform file name
 *
 *  This function should be called after a cpu_model and a
 *  network_model have been set up.
 *
 */
XBT_PUBLIC(void) surf_workstation_model_init_compound(const char
                                                      *filename);

/** \brief Initializes the platform with the workstation model CLM03
 *  \ingroup SURF_models
 *  \param filename XML platform file name
 *
 *  This platform model seperates the workstation model and the network model.
 *  The workstation model will be initialized with the model CLM03, the network
 *  model with the model CM02 and the CPU model with the model Cas01.
 *  In future releases, some other network models will be implemented and will be
 *  combined with the workstation model CLM03.
 *
 *  \see surf_workstation_model_init_KCCFLN05()
 */
XBT_PUBLIC(void) surf_workstation_model_init_CLM03(const char *filename);

/** \brief Initializes the platform with the model KCCFLN05
 *  \ingroup SURF_models
 *  \param filename XML platform file name
 *
 *  With this model, the workstations and the network are handled
 *  together. The network model is roughly the same as in CM02 but
 *  interference between computations and communications can be taken
 *  into account. This platform model is the default one for MSG and
 *  SimDag.
 *
 */
XBT_PUBLIC(void) surf_workstation_model_init_KCCFLN05(const char
                                                      *filename);

/** \brief Initializes the platform with the model KCCFLN05
 *  \ingroup SURF_models
 *  \param filename XML platform file name
 *
 *  With this model, only parallel tasks can be used. Resource sharing
 *  is done by identifying bottlenecks and giving an equal share of
 *  the model to each action.
 *
 */
XBT_PUBLIC(void) surf_workstation_model_init_ptask_L07(const char
                                                       *filename);

/** \brief The list of all available workstation model models
 *  \ingroup SURF_models
 */
XBT_PUBLIC_DATA(s_surf_model_description_t)
    surf_workstation_model_description[];

/** \brief List of initialized models
 *  \ingroup SURF_models
 */
XBT_PUBLIC_DATA(xbt_dynar_t) model_list;

/*******************************************/
/*** SURF Globals **************************/
/*******************************************/
XBT_PUBLIC_DATA(xbt_cfg_t) _surf_cfg_set;

/** \brief Initialize SURF
 *  \ingroup SURF_simulation
 *  \param argc argument number
 *  \param argv arguments
 *
 *  This function has to be called to initialize the common
 *  structures.  Then you will have to create the environment by
 *  calling 
 *  e.g. surf_workstation_model_init_CLM03() or
 *  surf_workstation_model_init_KCCFLN05().
 *
 *  \see surf_workstation_model_init_CLM03(),
 *  surf_workstation_model_init_KCCFLN05(), surf_workstation_model_init_compound(), surf_exit()
 */
XBT_PUBLIC(void) surf_init(int *argc, char **argv);     /* initialize common structures */

/** \brief Initialize the used models.
 *
 * Must be called after the surf_init so that configuration infrastructure is created
 * Must be called before parsing/creating the environment
 * Must not be called within the initialization process so that the use get a chance to change the settings from
 * its code between, say, MSG_init and MSG_create_environment using MSG_config
 */
XBT_PUBLIC(void) surf_config_models_setup(const char *platform_file);

/** \brief create the elements of the models
 *
 * Must be called after parsing the platform file and before using any elements
 */
XBT_PUBLIC(void) surf_config_models_create_elms(void);

/** \brief Finish simulation initialization
 *  \ingroup SURF_simulation
 *
 *  This function must be called before the first call to surf_solve()
 */
XBT_PUBLIC(void) surf_presolve(void);

/** \brief Performs a part of the simulation
 *  \ingroup SURF_simulation
 *  \param max_date Maximum date to update the simulation to, or -1
 *  \return the elapsed time, or -1.0 if no event could be executed
 *
 *  This function execute all possible events, update the action states
 *  and returns the time elapsed.
 *  When you call execute or communicate on a model, the corresponding actions
 *  are not executed immediately but only when you call surf_solve.
 *  Note that the returned elapsed time can be zero.
 */
XBT_PUBLIC(double) surf_solve(double max_date);

/** \brief Return the current time
 *  \ingroup SURF_simulation
 *
 *  Return the current time in millisecond.
 */
XBT_INLINE XBT_PUBLIC(double) surf_get_clock(void);

/** \brief Exit SURF
 *  \ingroup SURF_simulation
 *
 *  Clean everything.
 *
 *  \see surf_init()
 */
XBT_PUBLIC(void) surf_exit(void);

/* Prototypes of the functions that handle the properties */
XBT_PUBLIC_DATA(xbt_dict_t) current_property_set;       /* the prop set for the currently parsed element (also used in SIMIX) */
XBT_PUBLIC_DATA(void) parse_properties(void);

/* surf parse file related (public because called from a test suite) */
XBT_PUBLIC(void) parse_platform_file(const char *file);

/* Stores the sets */
XBT_PUBLIC_DATA(xbt_dict_t) set_list;

/* For the trace and trace:connect tag (store their content till the end of the parsing) */
XBT_PUBLIC_DATA(xbt_dict_t) traces_set_list;
XBT_PUBLIC_DATA(xbt_dict_t) trace_connect_list_host_avail;
XBT_PUBLIC_DATA(xbt_dict_t) trace_connect_list_power;
XBT_PUBLIC_DATA(xbt_dict_t) trace_connect_list_link_avail;
XBT_PUBLIC_DATA(xbt_dict_t) trace_connect_list_bandwidth;
XBT_PUBLIC_DATA(xbt_dict_t) trace_connect_list_latency;


XBT_PUBLIC(double) get_cpu_power(const char *power);

/*public interface to create resource bypassing the parser via cpu/network model
 *
 * see surfxml_parse.c
 * */
XBT_PUBLIC(void) surf_host_create_resource(char *name, double power_peak,
                                           double power_scale,
                                           tmgr_trace_t power_trace,
                                           int core,
                                           e_surf_resource_state_t
                                           state_initial,
                                           tmgr_trace_t state_trace,
                                           xbt_dict_t cpu_properties);

/*public interface to create resource bypassing the parser via workstation_ptask_L07 model
 *
 * see surfxml_parse.c
 * */
XBT_PUBLIC(void) surf_wsL07_host_create_resource(char *name,
                                                 double power_peak,
                                                 double power_scale,
                                                 tmgr_trace_t power_trace,
                                                 e_surf_resource_state_t
                                                 state_initial,
                                                 tmgr_trace_t state_trace,
                                                 xbt_dict_t
                                                 cpu_properties);
/**
 * create link resource
 * see surfxml_parse.c
 */
XBT_PUBLIC(void) surf_link_create_resource(char *name,
                                           double bw_initial,
                                           tmgr_trace_t bw_trace,
                                           double lat_initial,
                                           tmgr_trace_t lat_trace,
                                           e_surf_resource_state_t
                                           state_initial,
                                           tmgr_trace_t state_trace,
                                           e_surf_link_sharing_policy_t
                                           policy, xbt_dict_t properties);


XBT_PUBLIC(void) surf_wsL07_link_create_resource(char *name,
                                                 double bw_initial,
                                                 tmgr_trace_t bw_trace,
                                                 double lat_initial,
                                                 tmgr_trace_t lat_trace,
                                                 e_surf_resource_state_t
                                                 state_initial,
                                                 tmgr_trace_t state_trace,
                                                 e_surf_link_sharing_policy_t
                                                 policy,
                                                 xbt_dict_t properties);
/**
 * add route element (link_ctn) bypassing the parser
 *
 * see surfxml_parse.c
 *
 */
XBT_PUBLIC(void) surf_add_route_element(char *link_ctn_id);

/**
 * set route src_id,dest_id, and create a route resource
 *
 * see surf_routing.c && surfxml_parse.c
 */

XBT_PUBLIC(void) surf_set_routes(void);


/**
 * add traces
 * see surfxml_parse.c
 */
XBT_PUBLIC(void) surf_add_host_traces(void);
XBT_PUBLIC(void) surf_add_link_traces(void);
XBT_PUBLIC(void) surf_wsL07_add_traces(void);

/*
 * init AS from lua console
 * see surf_routing.c
 */
XBT_PUBLIC(void) routing_AS_init(const char *id, const char *mode);
XBT_PUBLIC(void) routing_AS_end(const char *id);
// add host to network element list
XBT_PUBLIC(void) routing_add_host(const char *host_id);
//Set a new link on the actual list of link for a route or ASroute
XBT_PUBLIC(void) routing_add_link(const char *link_id);
//Set the endpoints for a route
XBT_PUBLIC(void) routing_set_route(const char *src_id, const char *dst_id);
//Store the route
XBT_PUBLIC(void) routing_store_route(void);

/*
 * interface between surf and lua bindings
 * see surfxml_parse.c
 */
XBT_PUBLIC(void) surf_AS_new(const char *id, const char *mode);
XBT_PUBLIC(void) surf_AS_finalize(const char *id);
XBT_PUBLIC(void) surf_route_add_host(const char *id);
XBT_PUBLIC(void) surf_routing_add_route(const char *src_id,
                                        const char *dest_id,
                                        xbt_dynar_t links_id);

#include "surf/surf_resource.h"
#include "surf/surf_resource_lmm.h"

SG_END_DECL()
#endif                          /* _SURF_SURF_H */
