/**
 *  @file
 *  coresem.h
 *
 *  @brief contains all the constants and structures associated
 *  with the Counting Semaphore Handler.  A counting semaphore is the
 *  standard Dijkstra binary semaphore used to provide synchronization
 *  and mutual exclusion capabilities.
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
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

#ifndef _RTEMS_SCORE_CORESEM_H
#define _RTEMS_SCORE_CORESEM_H

/**
 *  @defgroup ScoreSemaphore Semaphore Handler
 *
 *   @brief The Semaphore Handler encapsulates functionality which provides the
 *  foundation Semaphore services used in all of the APIs supported by RTEMS.
 *
 *  This core object utilizes standard Dijkstra counting semaphores to provide
 *  synchronization and mutual exclusion capabilities.
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


#if defined(RTEMS_MULTIPROCESSING)
   /**
    *  @brief The following type defines the callout which the API provides
    *  to support global/multiprocessor operations on semaphores.
    */
   typedef void ( *CORE_semaphore_API_mp_support_callout )(
                                                            Thread_Control * ,
                                                            Objects_Id
                                                            );
#endif

   /**
    *  @brief Blocking disciplines for a semaphore.
    */
   typedef enum
   {
      /**
       * @brief This specifies that threads will wait for the semaphore in FIFO order.
       */
      CORE_SEMAPHORE_DISCIPLINES_FIFO ,
      /**
       * @brief This specifies that threads will wait for the semaphore in
       * priority order.
       */
      CORE_SEMAPHORE_DISCIPLINES_PRIORITY
   } CORE_semaphore_Disciplines;

   /**
    *  @brief Core Semaphore handler return statuses.
    */
   typedef enum
   {
      /**
       * @brief This status indicates that the operation completed successfully.
       */
      CORE_SEMAPHORE_STATUS_SUCCESSFUL ,
      /**
       * @brief This status indicates that the calling task did not want to block
       * and the operation was unable to complete immediately because the
       * resource was unavailable.
       */
      CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT ,
      /**
       * @brief This status indicates that the thread was blocked waiting for an
       * operation to complete and the semaphore was deleted.
       */
      CORE_SEMAPHORE_WAS_DELETED ,
      /**
       * @brief This status indicates that the calling task was willing to block
       * but the operation was unable to complete within the time allotted
       * because the resource never became available.
       */
      CORE_SEMAPHORE_TIMEOUT ,
      /**
       * @brief This status indicates that an attempt was made to unlock the semaphore
       * and this would have made its count greater than that allowed.
       */
      CORE_SEMAPHORE_MAXIMUM_COUNT_EXCEEDED ,
      /**
       * @brief This status indicates that the semaphore was not immediately
       * available and the caller passed a bad timeout value to the API
       * routine.  In this case, the API required that the validity check
       * for the timeout occur after the check that the semaphore was immediately
       * available.
       */
      CORE_SEMAPHORE_BAD_TIMEOUT_VALUE
   } CORE_semaphore_Status;

   /**
    *  @brief The following defines the control block used to manage the
    *  attributes of each semaphore.
    */
   typedef struct
   {
      /**
       * @brief This element indicates the maximum count this semaphore may have.
       */
      uint32_t maximum_count;
      /**
       * @brief This field indicates whether threads waiting on the semaphore block in
       * FIFO or priority order.
       */
      CORE_semaphore_Disciplines discipline;
   } CORE_semaphore_Attributes;

   /**
    *  @brief The following defines the control block used to manage each
    *  counting semaphore.
    */
   typedef struct
   {
      /**
       * @brief This field is the Waiting Queue used to manage the set of tasks
       * which are blocked waiting to obtain the semaphore.
       */
      Thread_queue_Control Wait_queue;
      /**
       * @brief This element is the set of attributes which define this instance's
       * behavior.
       */
      CORE_semaphore_Attributes Attributes;
      /**
       * @brief This element contains the current count of this semaphore.
       */
      uint32_t count;
   } CORE_semaphore_Control;

   /**
    *  @brief The following enumerated type is the set of blocking options
    *  available to seize operation.
    */
   typedef enum
   {
      /**
       * @brief This value indicates that the caller does not wish to block.
       */
      CORE_SEMAPHORE_NO_WAIT ,
      /**
       * @brief This value indicates that the caller is willing to block forever.
       */
      CORE_SEMAPHORE_BLOCK_FOREVER ,
      /**
       * @brief This value indicates that the caller is blocking with a timeout.
       */
      CORE_SEMAPHORE_BLOCK_WITH_TIMEOUT ,
      /**
       * @brief This value indicates that the caller wanted to block but passed in
       * a bad timeout value to the API. Unfortunately, this is a weird case
       * where the timeout bad error is required to be generated only if
       * the semaphore is not available.
       */
      CORE_SEMAPHORE_BAD_TIMEOUT
   } Core_semaphore_Blocking_option;

   /**
    *  @brief initialize a core semaphore
    *
    *  CORE_semaphore_Initialize
    *  This function initialize a semaphore and sets the initial value based
    *  on the given count.
    *
    *  @param[in] the_semaphore the semaphore control block to initialize
    *  @param[in] the_semaphore_attributes the attributes specified at create time
    *  @param[in] initial_value semaphore's initial value
    */
   void _CORE_semaphore_Initialize(
                                   CORE_semaphore_Control *the_semaphore ,
                                   CORE_semaphore_Attributes *the_semaphore_attributes ,
                                   uint32_t initial_value
                                   );

   /**
    *  @brief seize a core semaphore
    *
    *  This routine attempts to receive a unit from @a the_semaphore.
    *  If a unit is available or if the wait flag is FALSE, then the routine
    *  returns.  Otherwise, the calling task is blocked until a unit becomes
    *  available.
    *
    *  @param[in] the_semaphore is the semaphore to seize
    *  @param[in] id is the Id of the API level Semaphore object associated
    *  with this instance of a SuperCore Semaphore
    *  @param[in] wait is the blocking mode
    *  @param[in] timeout is the number of ticks the calling thread is willing
    *  to wait if @a wait is TRUE.
    */
   void _CORE_semaphore_Seize(
                              CORE_semaphore_Control *the_semaphore ,
                              Objects_Id id ,
                              Core_semaphore_Blocking_option wait ,
                              Watchdog_Interval timeout
                              );

   /**
    *  @brief surrender a core semaphore
    *
    *  This routine frees a unit to the semaphore.  If a task was blocked waiting
    *  for a unit from this semaphore, then that task will be readied and the unit
    *  given to that task.  Otherwise, the unit will be returned to the semaphore.
    *
    *  @param[in] the_semaphore is the semaphore to surrender
    *  @param[in] id is the Id of the API level Semaphore object associated
    *  with this instance of a SuperCore Semaphore
    *  @param[in] api_semaphore_mp_support is the routine to invoke if the
    *  thread unblocked is remote
    *
    *  @return an indication of whether the routine succeeded or failed
    */
   CORE_semaphore_Status _CORE_semaphore_Surrender(
                                                   CORE_semaphore_Control *the_semaphore ,
                                                   Objects_Id id
#if defined(RTEMS_MULTIPROCESSING)
       ,
                                                   CORE_semaphore_API_mp_support_callout api_semaphore_mp_support
#endif
       );

   /**
    *  @brief flush a core semaphore
    *
    *  This routine assists in the deletion of a semaphore by flushing the
    *  associated wait queue.
    *
    *  @param[in] the_semaphore is the semaphore to flush
    *  @param[in] remote_extract_callout is the routine to invoke if the
    *  thread unblocked is remote
    *  @param[in] status is the status to be returned to the unblocked thread
    */
   void _CORE_semaphore_Flush(
                              CORE_semaphore_Control *the_semaphore ,
#if defined(RTEMS_MULTIPROCESSING)
       Thread_queue_Flush_callout remote_extract_callout ,
#endif
       uint32_t status
                              );

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/coresem.inl>
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
