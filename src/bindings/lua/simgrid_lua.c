/* SimGrid Lua bindings                                                     */

/* Copyright (c) 2010, the SimGrid team. All right reserved.                */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include <stdio.h>
#include <lua5.1/lauxlib.h>
#include <lua5.1/lualib.h>

#include "msg/msg.h"
#include "xbt.h"

XBT_LOG_NEW_DEFAULT_SUBCATEGORY(lua,bindings,"Lua Bindings");

#define TASK_MODULE_NAME "simgrid.Task"

/* ********************************************************************************* */
/*                            helper functions                                       */
/* ********************************************************************************* */

static void stackDump (lua_State *L) {
  char buff[2048];
  char *p=buff;
  int i;
  int top = lua_gettop(L);
  void *ptr;
  fflush(stdout);
  p+=sprintf(p,"STACK(top=%d): ",top);
  for (i = 1; i <= top; i++) {  /* repeat for each level */
    int t = lua_type(L, i);
    switch (t) {

    case LUA_TSTRING:  /* strings */
      p+=sprintf(p,"`%s'", lua_tostring(L, i));
      break;

    case LUA_TBOOLEAN:  /* booleans */
      p+=sprintf(p,lua_toboolean(L, i) ? "true" : "false");
      break;

    case LUA_TNUMBER:  /* numbers */
      p+=sprintf(p,"%g", lua_tonumber(L, i));
      break;

    default:  /* other values */
      if ((ptr = luaL_checkudata(L,i,TASK_MODULE_NAME))) {
        p+=sprintf(p,"task");
      } else {
        p+=printf(p,"%s", lua_typename(L, t));
      }
      break;

    }
    p+=sprintf(p,"  ");  /* put a separator */
  }
  INFO1("%s",buff);
}

/** @brief ensures that a userdata on the stack is a task and returns the pointer inside the userdata */
static m_task_t checkTask (lua_State *L,int index) {
  m_task_t *pi,tk;
  luaL_checktype(L,index,LUA_TUSERDATA);
  pi = (m_task_t*)luaL_checkudata(L,index,TASK_MODULE_NAME);
  if(pi == NULL)
    luaL_typerror(L,index,TASK_MODULE_NAME);
  tk = *pi;
  if(!tk)
    luaL_error(L,"null Task");
  return  tk;
}

/** @brief leaves a new userdata on top of the stack, sets its metatable, and sets the Task pointer inside the userdata */
static m_task_t *pushTask (lua_State *L,m_task_t tk) {
  m_task_t *pi = (m_task_t*)lua_newuserdata(L,sizeof(m_task_t));
  *pi=tk;
  luaL_getmetatable(L,TASK_MODULE_NAME);
  lua_setmetatable(L,-2);
  return pi;

}


/* ********************************************************************************* */
/*                           wrapper functions                                       */
/* ********************************************************************************* */

static int Task_new(lua_State* L) {
  const char *name=luaL_checkstring(L,1);
  int comp_size = luaL_checkint(L,2);
  int msg_size = luaL_checkint(L,3);
  // FIXME: data shouldn't be NULL I guess
  pushTask(L,MSG_task_create(name,comp_size,msg_size,NULL));
  INFO0("Created task");
  return 1;
}

static int Task_get_name(lua_State *L) {
  m_task_t tk = checkTask(L,1);
  lua_pushstring(L,MSG_task_get_name(tk));
  return 1;
}

static int Task_computation_duration(lua_State *L){
  m_task_t tk = checkTask(L,1);
  lua_pushnumber(L,MSG_task_get_compute_duration (tk));
  return 1;
}

static int Task_execute(lua_State *L){
  m_task_t tk = checkTask(L,1);
  int res = MSG_task_execute(tk);
  lua_pushnumber(L,res);
  return 1;
}

static int Task_destroy(lua_State *L) {
  m_task_t tk = checkTask(L,1);
  int res = MSG_task_destroy(tk);
  lua_pushnumber(L,res);
  return 1;
}
static int Task_send(lua_State *L)  {
  m_task_t tk = checkTask(L,1);
  const char *mailbox = luaL_checkstring(L,2);
  stackDump(L);fflush(stdout);fflush(stderr);
  int res = MSG_task_send(tk,mailbox);
  res++;//FIXME: check it instead of avoiding the warning
  stackDump(L);
  return 0;
}
static int Task_recv(lua_State *L)  {
  m_task_t tk = NULL;
  //stackDump(L);
  const char *mailbox = luaL_checkstring(L,1);
  int res = MSG_task_receive(&tk,mailbox);
  res++;//FIXME: check it instead of avoiding the warning
  INFO1("Task Name : >>>%s",MSG_task_get_name(tk));
  pushTask(L,tk);
  //  stackDump(L);
  return 1;
}

static const luaL_reg Task_methods[] = {
    {"new",   Task_new},
    {"name",  Task_get_name},
    {"computation_duration",  Task_computation_duration},
    {"execute", Task_execute},
    {"destroy", Task_destroy},
    {"send",    Task_send},
    {"recv",    Task_recv},
    {0,0}
};
static int Task_gc(lua_State *L) {
  m_task_t tk=checkTask(L,1);
  if (tk) MSG_task_destroy(tk);
  //printf("GoodBye Task(%p)\n",lua_touserdata(L,1));
  return 0;
}

static int Task_tostring(lua_State *L) {
  lua_pushfstring(L, "Task :%p",lua_touserdata(L,1));
  return 1;
}


static const luaL_reg Task_meta[] = {
    {"__gc",  Task_gc},
    {"__tostring",  Task_tostring},
    {0,0}
};

/*
 * Environment related
 */
static int launch_application(lua_State *L) {
  const char * file = luaL_checkstring(L,1);
  MSG_launch_application(file);
  return 0;
}
#include "simix/simix.h" //FIXME: KILLME when debugging on simix internals become useless
static int create_environment(lua_State *L) {
  const char *file = luaL_checkstring(L,1);
  INFO1("Loading environment file %s",file);
  MSG_create_environment(file);
  smx_host_t *hosts = SIMIX_host_get_table();
  int i;
  for (i=0;i<SIMIX_host_get_number();i++) {
    INFO1("We have an host %s", SIMIX_host_get_name(hosts[i]));
  }

  return 0;
}
static int debug(lua_State *L) {
  const char *str = luaL_checkstring(L,1);
  DEBUG1("%s",str);
  return 0;
}
static int info(lua_State *L) {
  const char *str = luaL_checkstring(L,1);
  INFO1("%s",str);
  return 0;
}
static int run(lua_State *L) {
  MSG_main();
  return 0;
}
static int clean(lua_State *L) {
  MSG_clean();
  return 0;
}
static const luaL_Reg simgrid_funcs[] = {
    { "create_environment", create_environment},
    { "launch_application", launch_application},
    { "debug", debug},
    { "info", info},
    { "run", run},
    { "clean", clean},
    /* short names */
    { "platform", create_environment},
    { "application", launch_application},
    { NULL, NULL }
};

/* ********************************************************************************* */
/*                       module management functions                                 */
/* ********************************************************************************* */

void SIMIX_ctx_lua_factory_set_state(void *state);/* Hack: pass the L to our factory */
extern const char*xbt_ctx_factory_to_use; /*Hack: let msg load directly the right factory */

#define LUA_MAX_ARGS_COUNT 10 /* maximum amount of arguments we can get from lua on command line */

int luaopen_simgrid(lua_State* L); // Fuck gcc: we don't need that prototype
int luaopen_simgrid(lua_State* L) {
  xbt_ctx_factory_to_use = "lua";

  char **argv=malloc(sizeof(char*)*LUA_MAX_ARGS_COUNT);
  int argc=1;
  argv[0] = (char*)"/usr/bin/lua"; /* Lie on the argv[0] so that the stack dumping facilities find the right binary. FIXME: what if lua is not in that location? */
  /* Get the command line arguments from the lua interpreter */
  lua_getglobal(L,"arg");
  xbt_assert1(lua_istable(L,-1),"arg parameter is not a table but a %s",lua_typename(L,-1));
  int done=0;
  while (!done) {
    argc++;
    lua_pushinteger(L,argc-2);
    lua_gettable(L,-2);
    if (lua_isnil(L,-1)) {
      done = 1;
    } else {
      xbt_assert1(lua_isstring(L,-1),"argv[%d] got from lua is no string",argc-1);
      xbt_assert2(argc<LUA_MAX_ARGS_COUNT,
           "Too many arguments, please increase LUA_MAX_ARGS_COUNT in %s before recompiling SimGrid if you insist on having more than %d args on command line",
           __FILE__,LUA_MAX_ARGS_COUNT-1);
      argv[argc-1] = (char*)luaL_checkstring(L,-1);
      lua_pop(L,1);
      INFO1("Got command line argument %s from lua",argv[argc-1]);
    }
  }
  argv[argc--]=NULL;

  /* Initialize the MSG core */
  MSG_global_init(&argc,argv);
  INFO1("Still %d arguments on command line",argc); // FIXME: update the lua's arg table to reflect the changes from SimGrid

  /* register the core C functions to lua */
  luaL_register(L, "simgrid", simgrid_funcs);
  /* register the tasks to lua */
  luaL_openlib(L,TASK_MODULE_NAME,Task_methods,0); //create methods table,add it to the globals
  luaL_newmetatable(L,TASK_MODULE_NAME); //create metatable for Task,add it to the Lua registry
  luaL_openlib(L,0,Task_meta,0);// fill metatable
  lua_pushliteral(L,"__index");
  lua_pushvalue(L,-3);  //dup methods table
  lua_rawset(L,-3); //matatable.__index = methods
  lua_pushliteral(L,"__metatable");
  lua_pushvalue(L,-3);  //dup methods table
  lua_rawset(L,-3); //hide metatable:metatable.__metatable = methods
  lua_pop(L,1);   //drop metatable

  /* Keep the context mechanism informed of our lua world today */
  SIMIX_ctx_lua_factory_set_state(L);

  return 1;
}