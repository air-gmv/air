/**
 *  @file
 *  coremutex.h
 *
 *  @brief contains all the constants and structures associated
 *  with the Mutex Handler.  A mutex is an enhanced version of the standard
 *  Dijkstra binary semaphore used to provide synchronization and mutual
 *  exclusion capabilities.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  600     | 17/11/2008  | mcoutinho    | IPR 64
 *  3666    | 07/07/2009  | mcoutinho    | IPR 578
 *  3759    | 13/07/2009  | mcoutinho    | IPR 463
 *  3759    | 13/07/2009  | mcoutinho    | IPR 553
 *  3901    | 21/07/2009  | mcoutinho    | IPR 99
 *  4548    | 28/09/2009  | mcoutinho    | IPR 618
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5322    | 02/11/2009  | mcoutinho    | IPR 852
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

#ifndef _RTEMS_SCORE_COREMUTEX_H
#define _RTEMS_SCORE_COREMUTEX_H

/**
 *  @defgroup ScoreMutex Mutex Handler
 *
 *   @brief The Mutex Handler encapsulates functionality which provides the foundation
 *  Mutex services used in all of the APIs supported by RTEMS.
 *
 *  This handler provides synchronization and mutual exclusion capabilities.
 */
/**@{*/

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/priority.h>
#include <rtems/score/watchdog.h>
#include <rtems/score/interr.h>
#include <rtems/score/sysstate.h>


#if defined(RTEMS_MULTIPROCESSING)
   /**
    *  @brief MP Support Callback Prototype
    *
    *  The following type defines the callout which the API provides
    *  to support global/multiprocessor operations on mutexes.
    */
   typedef void ( *CORE_mutex_API_mp_support_callout )(
                                                        Thread_Control * ,
                                                        Objects_Id
                                                        );
#endif

   /**
    *  @brief Blocking Disciplines Enumerated Type
    *
    *  This enumerated type defines the blocking disciplines for a mutex.
    */
   typedef enum
   {
      /**
       * @brief This specifies that threads will wait for the mutex in FIFO order.
       */
      CORE_MUTEX_DISCIPLINES_FIFO ,
      /**
       * @brief This specifies that threads will wait for the mutex in priority
       * order.
       */
      CORE_MUTEX_DISCIPLINES_PRIORITY ,
      /**
       * @brief This specifies that threads will wait for the mutex in priority order.
       * Additionally, the Priority Inheritance Protocol will be in effect.
       */
      CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT ,
      /**
       * @brief This specifies that threads will wait for the mutex in priority order.
       * Additionally, the Priority Ceiling Protocol will be in effect.
       */
      CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING
   } CORE_mutex_Disciplines;

   /**
    *  @brief Mutex method return statuses
    *
    *  This enumerated type defines the possible Mutex handler return statuses.
    */
   typedef enum
   {
      /**
       * @brief This status indicates that the operation completed successfully.
       */
      CORE_MUTEX_STATUS_SUCCESSFUL ,
      /**
       *  @brief This status indicates that the calling task did not want to block
       *  and the operation was unable to complete immediately because the
       *  resource was unavailable.
       */
      CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT ,
      /**
       *  @brief This status indicates that an attempt was made to relock a mutex
       *  for which nesting is not configured.
       */
      CORE_MUTEX_STATUS_NESTING_NOT_ALLOWED ,
      /**
       *  @brief This status indicates that an attempt was made to release a mutex
       *  by a thread other than the thread which locked it.
       */
      CORE_MUTEX_STATUS_NOT_OWNER_OF_RESOURCE ,
      /**
       *  @brief This status indicates that the thread was blocked waiting for an
       *  operation to complete and the mutex was deleted.
       */
      CORE_MUTEX_WAS_DELETED ,
      /**
       *  @brief This status indicates that the calling task was willing to block
       *  but the operation was unable to complete within the time allotted
       *  because the resource never became available.
       */
      CORE_MUTEX_TIMEOUT ,
      /**
       *  @brief This status indicates that a thread of logically greater importance
       *  than the ceiling priority attempted to lock this mutex.
       */
      CORE_MUTEX_STATUS_CEILING_VIOLATED
   } CORE_mutex_Status;

   /**
    *  @brief Mutex Lock Nesting Behavior Enumeration
    *
    *  This enumerated type defines the possible behaviors for
    *  lock nesting.
    */
   typedef enum
   {
      /**
       * @brief This sequence has no blocking or errors:
       *         + lock(m)
       *         + lock(m)
       *         + unlock(m)
       *         + unlock(m)
       */
      CORE_MUTEX_NESTING_ACQUIRES ,
      /**
       * @brief This sequence performs as indicated:
       *        + lock(m)
       *        + lock(m)   - deadlocks or timeouts
       *        + unlock(m) - releases
       */
      CORE_MUTEX_NESTING_BLOCKS
   } CORE_mutex_Nesting_behaviors;

   /**
    *  @brief value of a mutex when it is unlocked.
    */
#define CORE_MUTEX_UNLOCKED 1

   /**
    *  @brief value of a mutex when it is locked.
    */
#define CORE_MUTEX_LOCKED   0

   /**
    *  @brief Core Mutex Attributes
    *
    *  The following defines the control block used to manage the
    *  attributes of each mutex.
    */
   typedef struct
   {
      /**
       * @brief This field determines what the behavior of this mutex instance will
       * be when attempting to acquire the mutex when it is already locked.
       */
      CORE_mutex_Nesting_behaviors lock_nesting_behavior;
      /**
       * @brief When this field is TRUE, then only the thread that locked the mutex
       * is allowed to unlock it.
       */
      boolean only_owner_release;
      /**
       * @brief This field indicates whether threads waiting on the mutex block in
       * FIFO or priority order.
       */
      CORE_mutex_Disciplines discipline;
      /**
       * @brief This field contains the ceiling priority to be used if that protocol
       * is selected.
       */
      Priority_Control priority_ceiling;
   } CORE_mutex_Attributes;


   /**
    * @brief Declare a pointer to the stucture Semaphore_Control
    **/
   typedef struct Semaphore_Control *ptrSemaphore_Control;

   /**
    *  @brief Core Mutex Control Structure
    *
    *  The following defines the control block used to manage each mutex.
    */
   typedef struct _CORE_mutex_Control
   {
      /**
       * @brief Node on a threads hold mutexes chain
       **/
      Chain_Node Node;
      /**
       * @brief This field is the Waiting Queue used to manage the set of tasks
       * which are blocked waiting to lock the mutex.
       */
      Thread_queue_Control Wait_queue;
      /**
       * @brief This element is the set of attributes which define this instance's
       * behavior.
       */
      CORE_mutex_Attributes Attributes;
      /**
       * @brief This element contains the current state of the mutex.
       */
      uint32_t lock;
      /**
       * @brief This element contains the number of times the mutex has been acquired
       * nested.  This must be zero (0) before the mutex is actually unlocked.
       */
      uint32_t nest_count;
      /**
       * @brief This is the number of waiting threads.
       */
      uint32_t blocked_count;
      /**
       * @brief This element points to the thread which is currently holding this mutex.
       * The holder is the last thread to successfully lock the mutex and which
       * has not unlocked it.  If the thread is not locked, there is no holder.
       */
      Thread_Control *holder;
      /**
       * @brief This element contains the object Id of the holding thread.
       */
      Objects_Id holder_id;
      /**
       * @brief The Semaphore that contains this mutex
       **/
      ptrSemaphore_Control semaphore;
   } CORE_mutex_Control;

   /**
    *  @brief initialize a core mutex
    *
    *  This routine initializes a mutex at create time and set the control
    *  structure according to the values passed.
    *
    *  @param[in] the_mutex the mutex control block to initialize
    *  @param[in] the_mutex_attributes the mutex attributes specified at create time
    *  @param[in] initial_lock mutex initial lock or unlocked status
    *  @param[in] semaphore the semaphore that create the mutex
    */
   void _CORE_mutex_Initialize(
                               CORE_mutex_Control *the_mutex ,
                               CORE_mutex_Attributes *the_mutex_attributes ,
                               uint32_t initial_lock ,
                               ptrSemaphore_Control semaphore
                               );

#ifndef __RTEMS_APPLICATION__

   /**
    *  @brief Seize Mutex with Quick Success Path
    *
    *  This routine attempts to receive a unit from the_mutex.
    *  If a unit is available or if the wait flag is FALSE, then the routine
    *  returns.  Otherwise, the calling task is blocked until a unit becomes
    *  available.
    *
    *  @param[in] the_mutex is the mutex to attempt to lock
    *  @param[in] level_p is the interrupt level holder
    *
    *  @return This routine returns 0 if "trylock" can resolve whether or not
    *  the mutex is immediately obtained or there was an error attempting to
    *  get it.  It returns 1 to indicate that the caller cannot obtain
    *  the mutex and will have to block to do so.
    *
    *  @note For performance reasons, this routine is implemented as
    *  a macro that uses two support routines.
    */
   RTEMS_INLINE_ROUTINE int _CORE_mutex_Seize_interrupt_trylock(
                                                                CORE_mutex_Control *the_mutex ,
                                                                ISR_Level *level_p
                                                                );

   /**
    *  @brief seize a mutex with a timeout
    *
    *  _CORE_mutex_Seize (interrupt blocking support)
    *  This routine blocks the caller thread after an attempt attempts to obtain
    *  the specified mutex has failed.
    *
    *  @param[in] the_mutex pointer to mutex control block
    *  @param[in] timeout number of ticks to wait (0 means forever)
    */
   void _CORE_mutex_Seize_interrupt_blocking(
                                             CORE_mutex_Control *the_mutex ,
                                             Watchdog_Interval timeout
                                             );


   /**
    *  @brief surrender a mutex
    *
    *  This routine frees a unit to the mutex.  If a task was blocked waiting for
    *  a unit from this mutex, then that task will be readied and the unit
    *  given to that task.  Otherwise, the unit will be returned to the mutex.
    *
    *  @param[in] the_mutex is the mutex to surrender
    *  @param[in] id is the id of the RTEMS Object associated with this mutex
    *  @param[in] api_mutex_mp_support is the routine that will be called when
    *  unblocking a remote mutex
    *  @param[in] force if force is set to true, then the mutex will be surrendered
    *  regardless of the owner
    *
    *  @return an indication of whether the routine succeeded or failed
    */
   CORE_mutex_Status _CORE_mutex_Surrender(
                                           CORE_mutex_Control *the_mutex ,
                                           Objects_Id id ,
#if defined(RTEMS_MULTIPROCESSING)
       CORE_mutex_API_mp_support_callout api_mutex_mp_support ,
#endif
       boolean force
                                           );

   /**
    *  @brief flush all threads waiting on a mutex
    *
    *  This routine assists in the deletion of a mutex by flushing the associated
    *  wait queue.
    *
    *  @param[in] the_mutex is the mutex to flush
    *  @param[in] remote_extract_callout is the routine to invoke when a remote
    *  thread is extracted
    *  @param[in] status is the status value which each unblocked thread will
    *  return to its caller.
    */
   void _CORE_mutex_Flush(
                          CORE_mutex_Control *the_mutex ,
#if defined(RTEMS_MULTIPROCESSING)
       Thread_queue_Flush_callout remote_extract_callout ,
#endif
       uint32_t status
                          );

#include <rtems/score/coremutex.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/*  end of include file */

/**  
 *  @}
 */
