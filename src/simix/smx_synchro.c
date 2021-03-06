/* Copyright (c) 2007, 2008, 2009, 2010. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include "private.h"
#include "xbt/log.h"


XBT_LOG_NEW_DEFAULT_SUBCATEGORY(simix_synchro, simix,
                                "Logging specific to SIMIX (synchronization)");

static smx_action_t SIMIX_synchro_wait(smx_host_t smx_host, double timeout);
static void SIMIX_synchro_finish(smx_action_t action);
static void _SIMIX_cond_wait(smx_cond_t cond, smx_mutex_t mutex, double timeout,
                             smx_process_t issuer, smx_req_t req);
static void _SIMIX_sem_wait(smx_sem_t sem, double timeout, smx_process_t issuer,
                            smx_req_t req);
static void SIMIX_sem_block_onto(smx_sem_t sem);

/***************************** Synchro action *********************************/

static smx_action_t SIMIX_synchro_wait(smx_host_t smx_host, double timeout)
{
  smx_action_t action;
  action = xbt_mallocator_get(simix_global->action_mallocator);
  action->type = SIMIX_ACTION_SYNCHRO;
  action->name = xbt_strdup("synchro");
  action->synchro.sleep = 
    surf_workstation_model->extension.workstation.sleep(smx_host->host, timeout);

  surf_workstation_model->action_data_set(action->synchro.sleep, action);
  return action;
}

void SIMIX_synchro_stop_waiting(smx_process_t process, smx_req_t req)
{
  switch (req->call) {

    case REQ_MUTEX_LOCK:
      xbt_swag_remove(process, req->mutex_lock.mutex->sleeping);
      break;

    case REQ_COND_WAIT:
      xbt_swag_remove(process, req->cond_wait.cond->sleeping);
      break;

    case REQ_COND_WAIT_TIMEOUT:
      xbt_swag_remove(process, req->cond_wait_timeout.cond->sleeping);
      break;

    case REQ_SEM_ACQUIRE:
      xbt_swag_remove(process, req->sem_acquire.sem->sleeping);
      break;

    case REQ_SEM_ACQUIRE_TIMEOUT:
      xbt_swag_remove(process, req->sem_acquire_timeout.sem->sleeping);
      break;

    default:
      THROW_IMPOSSIBLE;
  }
}

void SIMIX_synchro_destroy(smx_action_t action)
{
  XBT_DEBUG("Destroying synchro %p", action);
  action->synchro.sleep->model_type->action_unref(action->synchro.sleep);
  xbt_free(action->name);
  xbt_mallocator_release(simix_global->action_mallocator, action);
}

void SIMIX_post_synchro(smx_action_t action)
{
  if (surf_workstation_model->action_state_get(action->synchro.sleep) == SURF_ACTION_FAILED)
    action->state = SIMIX_FAILED;
  else if(surf_workstation_model->action_state_get(action->synchro.sleep) == SURF_ACTION_DONE)
    action->state = SIMIX_SRC_TIMEOUT;

  SIMIX_synchro_finish(action);  
}

static void SIMIX_synchro_finish(smx_action_t action)
{
  smx_req_t req = xbt_fifo_shift(action->request_list);

  switch (action->state) {

    case SIMIX_SRC_TIMEOUT:
      TRY {
        THROWF(timeout_error, 0, "Synchro's wait timeout");
      } CATCH(req->issuer->running_ctx->exception) {
        req->issuer->doexception = 1;
      }
      break;

    case SIMIX_FAILED:
      TRY {
        THROWF(host_error, 0, "Host failed");
      } CATCH(req->issuer->running_ctx->exception) {
        req->issuer->doexception = 1;
      }
      break;

    default:
      THROW_IMPOSSIBLE;
      break;
  }

  SIMIX_synchro_stop_waiting(req->issuer, req);
  SIMIX_synchro_destroy(action);
  SIMIX_request_answer(req);
}
/*********************************** Mutex ************************************/

/**
 * \brief Initialize a mutex.
 *
 * Allocs and creates the data for the mutex.
 * \return A mutex
 */
smx_mutex_t SIMIX_mutex_init(void)
{
  s_smx_process_t p;            /* useful to initialize sleeping swag */

  smx_mutex_t mutex = xbt_new0(s_smx_mutex_t, 1);
  mutex->locked = 0;
  mutex->sleeping = xbt_swag_new(xbt_swag_offset(p, synchro_hookup));
  return mutex;
}

/**
 * \brief Handle mutex lock request
 * \param req The request
 */
void SIMIX_pre_mutex_lock(smx_req_t req)
{
  /* FIXME: check where to validate the arguments */
  smx_action_t sync_act = NULL;
  smx_mutex_t mutex = req->mutex_lock.mutex;
  smx_process_t process = req->issuer;

  if (mutex->locked) {
    /* FIXME: check if the host is active ? */
    /* Somebody using the mutex, use a synchro action to get host failures */
    sync_act = SIMIX_synchro_wait(process->smx_host, -1);
    xbt_fifo_push(sync_act->request_list, req);
    req->issuer->waiting_action = sync_act;
    xbt_swag_insert(req->issuer, mutex->sleeping);   
  } else {
    /* mutex free */
    mutex->locked = 1;
    mutex->owner = req->issuer;
    SIMIX_request_answer(req);
  }
}

/**
 * \brief Tries to lock a mutex.
 *
 * Tries to lock a mutex, return 1 if the mutex is unlocked, else 0.
 * This function does not block and wait for the mutex to be unlocked.
 * \param mutex The mutex
 * \param issuer The process that tries to acquire the mutex
 * \return 1 - mutex free, 0 - mutex used
 */
int SIMIX_mutex_trylock(smx_mutex_t mutex, smx_process_t issuer)
{
  if (mutex->locked)
    return 0;

  mutex->locked = 1;
  mutex->owner = issuer;
  return 1;
}

/**
 * \brief Unlocks a mutex.
 *
 * Unlocks the mutex and gives it to a process waiting for it. 
 * If the unlocker is not the owner of the mutex nothing happens.
 * If there are no process waiting, it sets the mutex as free.
 * \param mutex The mutex
 * \param issuer The process trying to unlock the mutex
 */
void SIMIX_mutex_unlock(smx_mutex_t mutex, smx_process_t issuer)
{
  smx_process_t p;              /*process to wake up */

  /* If the mutex is not owned by the issuer do nothing */
  if (issuer != mutex->owner)
    return;

  if (xbt_swag_size(mutex->sleeping) > 0) {
    p = xbt_swag_extract(mutex->sleeping);
    SIMIX_synchro_destroy(p->waiting_action);
    p->waiting_action = NULL;
    mutex->owner = p;
    SIMIX_request_answer(&p->request);
  } else {
    /* nobody to wake up */
    mutex->locked = 0;
    mutex->owner = NULL;
  }
}

/**
 * \brief Destroys a mutex.
 *
 * Destroys and frees the mutex's memory. 
 * \param mutex A mutex
 */
void SIMIX_mutex_destroy(smx_mutex_t mutex)
{
  if (mutex){
    xbt_swag_free(mutex->sleeping);
    xbt_free(mutex);
  }
}

/********************************* Condition **********************************/

/**
 * \brief Initialize a condition.
 *
 * Allocates and creates the data for the condition.
 * It have to be called before the use of the condition.
 * \return A condition
 */
smx_cond_t SIMIX_cond_init()
{
  s_smx_process_t p;
  smx_cond_t cond = xbt_new0(s_smx_cond_t, 1);
  cond->sleeping = xbt_swag_new(xbt_swag_offset(p, synchro_hookup));
  cond->mutex = NULL;
  return cond;
}

/**
 * \brief Handle condition waiting requests without timeouts
 * \param The request
 */
void SIMIX_pre_cond_wait(smx_req_t req)
{
  smx_process_t issuer = req->issuer;
  smx_cond_t cond = req->cond_wait.cond;
  smx_mutex_t mutex = req->cond_wait.mutex;

  _SIMIX_cond_wait(cond, mutex, -1, issuer, req);
}

/**
 * \brief Handle condition waiting requests with timeouts
 * \param The request
 */
void SIMIX_pre_cond_wait_timeout(smx_req_t req)
{
  smx_process_t issuer = req->issuer;
  smx_cond_t cond = req->cond_wait_timeout.cond;
  smx_mutex_t mutex = req->cond_wait_timeout.mutex;
  double timeout = req->cond_wait_timeout.timeout;

  _SIMIX_cond_wait(cond, mutex, timeout, issuer, req);
}


static void _SIMIX_cond_wait(smx_cond_t cond, smx_mutex_t mutex, double timeout,
                             smx_process_t issuer, smx_req_t req)
{
  smx_action_t sync_act = NULL;

  XBT_DEBUG("Wait condition %p", cond);

  /* If there is a mutex unlock it */
  /* FIXME: what happens if the issuer is not the owner of the mutex? */
  if (mutex != NULL) {
    cond->mutex = mutex;
    SIMIX_mutex_unlock(mutex, issuer);
  }

  sync_act = SIMIX_synchro_wait(issuer->smx_host, timeout);
  xbt_fifo_unshift(sync_act->request_list, req);
  issuer->waiting_action = sync_act;
  xbt_swag_insert(req->issuer, cond->sleeping);   
}

/**
 * \brief Signalizes a condition.
 *
 * Signalizes a condition and wakes up a sleeping process. 
 * If there are no process sleeping, no action is done.
 * \param cond A condition
 */
void SIMIX_cond_signal(smx_cond_t cond)
{
  smx_process_t proc = NULL;
  smx_mutex_t mutex = NULL;
  smx_req_t req = NULL;

  XBT_DEBUG("Signal condition %p", cond);

  /* If there are processes waiting for the condition choose one and try 
     to make it acquire the mutex */
  if ((proc = xbt_swag_extract(cond->sleeping))) {

    /* Destroy waiter's synchro action */
    SIMIX_synchro_destroy(proc->waiting_action);
    proc->waiting_action = NULL;

    /* Now transform the cond wait request into a mutex lock one */
    req = &proc->request;
    if(req->call == REQ_COND_WAIT)
      mutex = req->cond_wait.mutex;
    else
      mutex = req->cond_wait_timeout.mutex;

    req->call = REQ_MUTEX_LOCK;
    req->mutex_lock.mutex = mutex;

    SIMIX_pre_mutex_lock(req);
  }
}

/**
 * \brief Broadcasts a condition.
 *
 * Signal ALL processes waiting on a condition.
 * If there are no process waiting, no action is done.
 * \param cond A condition
 */
void SIMIX_cond_broadcast(smx_cond_t cond)
{
  XBT_DEBUG("Broadcast condition %p", cond);

  /* Signal the condition until nobody is waiting on it */
  while (xbt_swag_size(cond->sleeping)) {
    SIMIX_cond_signal(cond);
  }
}

/**
 * \brief Destroys a contidion.
 *
 * Destroys and frees the condition's memory. 
 * \param cond A condition
 */
void SIMIX_cond_destroy(smx_cond_t cond)
{
  XBT_DEBUG("Destroy condition %p", cond);

  if (cond != NULL) {
    xbt_assert(xbt_swag_size(cond->sleeping) == 0,
                "Cannot destroy conditional since someone is still using it");

    xbt_swag_free(cond->sleeping);
    xbt_free(cond);
  }
}

/******************************** Semaphores **********************************/
#define SMX_SEM_NOLIMIT 99999
/** @brief Initialize a semaphore */
smx_sem_t SIMIX_sem_init(unsigned int value)
{
  s_smx_process_t p;

  smx_sem_t sem = xbt_new0(s_smx_sem_t, 1);
  sem->sleeping = xbt_swag_new(xbt_swag_offset(p, synchro_hookup));
  sem->value = value;
  return sem;
}

/** @brief Destroys a semaphore */
void SIMIX_sem_destroy(smx_sem_t sem)
{
  XBT_DEBUG("Destroy semaphore %p", sem);
  if (sem != NULL) {
    xbt_assert(xbt_swag_size(sem->sleeping) == 0,
                "Cannot destroy semaphore since someone is still using it");
    xbt_swag_free(sem->sleeping);
    xbt_free(sem);
  }
}

/** @brief release the semaphore
 *
 * Unlock a process waiting on the semaphore.
 * If no one was blocked, the semaphore capacity is increased by 1.
 */
void SIMIX_sem_release(smx_sem_t sem)
{
  smx_process_t proc;

  XBT_DEBUG("Sem release semaphore %p", sem);
  if ((proc = xbt_swag_extract(sem->sleeping))) {
    proc = xbt_swag_extract(sem->sleeping);
    SIMIX_synchro_destroy(proc->waiting_action);
    proc->waiting_action = NULL;
    SIMIX_request_answer(&proc->request);
  } else if (sem->value < SMX_SEM_NOLIMIT) {
    sem->value++;
  }
}

/** @brief Returns true if acquiring this semaphore would block */
XBT_INLINE int SIMIX_sem_would_block(smx_sem_t sem)
{
  return (sem->value <= 0);
}

/** @brief Returns the current capacity of the semaphore */
int SIMIX_sem_get_capacity(smx_sem_t sem)
{
  return sem->value;
}

static void _SIMIX_sem_wait(smx_sem_t sem, double timeout, smx_process_t issuer,
                            smx_req_t req)
{
  smx_action_t sync_act = NULL;

  XBT_DEBUG("Wait semaphore %p (timeout:%f)", sem, timeout);
  if (sem->value <= 0) {
    sync_act = SIMIX_synchro_wait(issuer->smx_host, timeout);
    xbt_fifo_unshift(sync_act->request_list, req);
    issuer->waiting_action = sync_act;
    xbt_swag_insert(issuer, sem->sleeping);
  } else {
    sem->value--;
    SIMIX_request_answer(req);
  }
}

/**
 * \brief Handle sem acquire requests without timeouts
 */
void SIMIX_pre_sem_acquire(smx_req_t req)
{
  _SIMIX_sem_wait(req->sem_acquire.sem, -1, req->issuer, req);
}

/**
 * \brief Handle sem acquire requests with timeouts
 */
void SIMIX_pre_sem_acquire_timeout(smx_req_t req)
{
  _SIMIX_sem_wait(req->sem_acquire_timeout.sem,
                  req->sem_acquire_timeout.timeout, req->issuer, req);  
}
