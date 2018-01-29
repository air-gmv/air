/**
 * @file
 *
 * @brief Inlined Routines Associated with the SuperCore Semaphore
 *
 * This include file contains all of the inlined routines associated
 * with the SuperCore semaphore.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CORESEMIMPL_H
#define _RTEMS_SCORE_CORESEMIMPL_H

#include <rtems/score/coresem.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/status.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreSemaphore
 */
/**@{**/

/**
 *  @brief Initialize the semaphore based on the parameters passed.
 *
 *  This package is the implementation of the CORE Semaphore Handler.
 *  This core object utilizes standard Dijkstra counting semaphores to provide
 *  synchronization and mutual exclusion capabilities.
 *
 *  This routine initializes the semaphore based on the parameters passed.
 *
 *  @param[in] the_semaphore is the semaphore to initialize
 *  @param[in] initial_value is the initial count of the semaphore
 */
void _CORE_semaphore_Initialize(
  CORE_semaphore_Control *the_semaphore,
  uint32_t                initial_value
);

RTEMS_INLINE_ROUTINE void _CORE_semaphore_Acquire_critical(
  CORE_semaphore_Control *the_semaphore,
  Thread_queue_Context   *queue_context
)
{
  _Thread_queue_Acquire_critical( &the_semaphore->Wait_queue, queue_context );
}

RTEMS_INLINE_ROUTINE void _CORE_semaphore_Release(
  CORE_semaphore_Control *the_semaphore,
  Thread_queue_Context   *queue_context
)
{
  _Thread_queue_Release( &the_semaphore->Wait_queue, queue_context );
}

RTEMS_INLINE_ROUTINE void _CORE_semaphore_Destroy(
  CORE_semaphore_Control        *the_semaphore,
  const Thread_queue_Operations *operations,
  Thread_queue_Context          *queue_context
)
{
  _Thread_queue_Flush_critical(
    &the_semaphore->Wait_queue.Queue,
    operations,
    _Thread_queue_Flush_status_object_was_deleted,
    queue_context
  );
  _Thread_queue_Destroy( &the_semaphore->Wait_queue );
}

/**
 *  @brief Surrender a unit to a semaphore.
 *
 *  This routine frees a unit to the semaphore.  If a task was blocked waiting
 *  for a unit from this semaphore, then that task will be readied and the unit
 *  given to that task.  Otherwise, the unit will be returned to the semaphore.
 *
 *  @param[in] the_semaphore is the semaphore to surrender
 *  @param[in] operations The thread queue operations.
 *  @param[in] queue_context is a temporary variable used to contain the ISR
 *        disable level cookie
 *
 *  @retval an indication of whether the routine succeeded or failed
 */
RTEMS_INLINE_ROUTINE Status_Control _CORE_semaphore_Surrender(
  CORE_semaphore_Control        *the_semaphore,
  const Thread_queue_Operations *operations,
  uint32_t                       maximum_count,
  Thread_queue_Context          *queue_context
)
{
  Thread_Control *the_thread;
  Status_Control  status;

  status = STATUS_SUCCESSFUL;

  _CORE_semaphore_Acquire_critical( the_semaphore, queue_context );

  the_thread = _Thread_queue_First_locked(
    &the_semaphore->Wait_queue,
    operations
  );
  if ( the_thread != NULL ) {
    _Thread_queue_Extract_critical(
      &the_semaphore->Wait_queue.Queue,
      operations,
      the_thread,
      queue_context
    );
  } else {
    if ( the_semaphore->count < maximum_count )
      the_semaphore->count += 1;
    else
      status = STATUS_MAXIMUM_COUNT_EXCEEDED;

    _CORE_semaphore_Release( the_semaphore, queue_context );
  }

  return status;
}

/**
 * This routine returns the current count associated with the semaphore.
 *
 * @param[in] the_semaphore is the semaphore to obtain the count of
 *
 * @return the current count of this semaphore
 */
RTEMS_INLINE_ROUTINE uint32_t  _CORE_semaphore_Get_count(
  const CORE_semaphore_Control *the_semaphore
)
{
  return the_semaphore->count;
}

/**
 * This routine attempts to receive a unit from the_semaphore.
 * If a unit is available or if the wait flag is false, then the routine
 * returns.  Otherwise, the calling task is blocked until a unit becomes
 * available.
 *
 * @param[in] the_semaphore is the semaphore to obtain
 * @param[in] operations The thread queue operations.
 * @param[in] executing The currently executing thread.
 * @param[in] wait is true if the thread is willing to wait
 * @param[in] queue_context is a temporary variable used to contain the ISR
 *        disable level cookie
 */
RTEMS_INLINE_ROUTINE Status_Control _CORE_semaphore_Seize(
  CORE_semaphore_Control        *the_semaphore,
  const Thread_queue_Operations *operations,
  Thread_Control                *executing,
  bool                           wait,
  Thread_queue_Context          *queue_context
)
{
  _Assert( _ISR_Get_level() != 0 );

  _CORE_semaphore_Acquire_critical( the_semaphore, queue_context );
  if ( the_semaphore->count != 0 ) {
    the_semaphore->count -= 1;
    _CORE_semaphore_Release( the_semaphore, queue_context );
    return STATUS_SUCCESSFUL;
  }

  if ( !wait ) {
    _CORE_semaphore_Release( the_semaphore, queue_context );
    return STATUS_UNSATISFIED;
  }

  _Thread_queue_Context_set_thread_state(
    queue_context,
    STATES_WAITING_FOR_SEMAPHORE
  );
  _Thread_queue_Enqueue(
    &the_semaphore->Wait_queue.Queue,
    operations,
    executing,
    queue_context
  );
  return _Thread_Wait_get_status( executing );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
