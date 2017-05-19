/**
 *  @file
 *  tqdata.h
 *
 *  @brief contains all the constants and structures
 *  needed to declare a thread queue.
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
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

#ifndef _RTEMS_SCORE_TQDATA_H
#define _RTEMS_SCORE_TQDATA_H

/**
 *  @addtogroup ScoreThreadQ Thread Queue Handler
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/score/chain.h>
#include <rtems/score/priority.h>
#include <rtems/score/states.h>

   /**
    *  @brief disciplines supported by the Thread Queue Handler
    */
   typedef enum
   {
      /**
       * @brief FIFO queue discipline
       */
      THREAD_QUEUE_DISCIPLINE_FIFO ,
      /**
       * @brief priority queue discipline
       **/
      THREAD_QUEUE_DISCIPLINE_PRIORITY
   } Thread_queue_Disciplines;

   /**
    * @brief enumerated types that indicate what happened while the thread
    * queue was in the synchronization window.
    */
   typedef enum
   {
      THREAD_QUEUE_SYNCHRONIZED ,
      THREAD_QUEUE_NOTHING_HAPPENED ,
      THREAD_QUEUE_TIMEOUT ,
      THREAD_QUEUE_SATISFIED
   } Thread_queue_States;

   /**
    *  @brief number of priorirty headers on a priority thread queue
    *
    *  This is one of the constants used to manage the priority queues.
    *
    *  There are four chains used to maintain a priority -- each chain
    *  manages a distinct set of task priorities.  The number of chains
    *  is determined by TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS.
    *  The following set must be consistent.
    *
    *  The set below configures 4 headers -- each contains 64 priorities.
    *  Header x manages priority range (x*64) through ((x*64)+63).  If
    *  the priority is more than half way through the priority range it
    *  is in, then the search is performed from the rear of the chain.
    *  This halves the search time to find the insertion point.
    */
#define TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS 4

   /**
    *  @brief number of data priorities per header on a priority thread queue
    *
    *  This is one of the constants used to manage the priority queues.
    *  @ref TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS for more details.
    */
#define TASK_QUEUE_DATA_PRIORITIES_PER_HEADER      64

   /**
    *  @brief mask for reverse search on a thread queue
    *
    *  This is one of the constants used to manage the priority queues.
    *  @ref TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS for more details.
    */
#define TASK_QUEUE_DATA_REVERSE_SEARCH_MASK        0x20

   /**
    *  @brief manage sets of tasks which are blocked
    *  waiting to acquire a resource.
    */
   typedef struct
   {

      /**
       * @brief data structures used to manage the blocked
       *  set of tasks which varies based upon the discipline.
       */
      union
      {
         /**
          * @brief This is the FIFO discipline list.
          */
         Chain_Control Fifo;

         /**
          * @brief This is the set of lists for priority discipline waiting.
          */
         Chain_Control Priority[TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS];
      } Queues;

      /**
       * @brief This field is used to manage the critical section.
       */
      Thread_queue_States sync_state;

      /**
       * @brief This field indicates the thread queue's blocking discipline.
       */
      Thread_queue_Disciplines discipline;

      /**
       * @brief This indicates the blocking state for threads waiting on this
       * thread queue.
       */
      States_Control state;

      /**
       * @brief This is the status value returned to threads which timeout while
       *  waiting on this thread queue.
       */
      uint32_t timeout_status;
   } Thread_queue_Control;

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/tqdata.inl>
#endif

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif
/* end of include file */

/**
 *  @}
 */
