/**
 *  @file
 *  states.h
 *
 *  @brief contains thread execution state information.
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

#ifndef _RTEMS_SCORE_STATES_H
#define _RTEMS_SCORE_STATES_H

/**
 *  @defgroup ScoreStates Thread States Handler
 *
 *  @brief The Thread States Handler encapsulates functionality which relates to the management of
 *  the state bitmap associated with each thread.
 */
/**@{*/

#ifdef __cplusplus
extern "C"
{
#endif

   /**
    *  @brief control block used to manage a thread's state.
    */
   typedef uint32_t States_Control;

   /*
    *  The following constants define the individual states which may be
    *  be used to compose and manipulate a thread's state.
    */

   /**
    * @brief determine the value that corresponds to all states being set.
    */
#define STATES_ALL_SET                         0xfffff

   /**
    * @brief determine the value that corresponds to a task being ready.
    */
#define STATES_READY                           0x00000

   /**
    * @brief determine the value that corresponds to a task being created but not yet started (dormant state)
    */
#define STATES_DORMANT                         0x00001

   /**
    * @brief determine the value that corresponds to a task being suspended.
    */
#define STATES_SUSPENDED                       0x00002

   /**
    * @brief determine the value that corresponds to a task being in an internal state transition.
    */
#define STATES_TRANSIENT                       0x00004

   /**
    * @brief determine the value that corresponds to a task which is waiting for a timeout.
    */
#define STATES_DELAYING                        0x00008

   /**
    * @brief determine the value that corresponds to a task waiting until a specific TOD.
    */
#define STATES_WAITING_FOR_TIME                0x00010

   /**
    * @brief determine the value that corresponds to a task waiting for a variable length buffer.
    */
#define STATES_WAITING_FOR_BUFFER              0x00020

   /**
    * @brief determine the value that corresponds to a task waiting for a fixed size segment.
    */
#define STATES_WAITING_FOR_SEGMENT             0x00040

   /**
    * @brief determine the value that corresponds to a task waiting for a message.
    */
#define STATES_WAITING_FOR_MESSAGE             0x00080

   /**
    * @brief determine the value that corresponds to a task waiting for an event.
    */
#define STATES_WAITING_FOR_EVENT               0x00100

   /**
    * @brief determine the value that corresponds to a task waiting for a semaphore.
    */
#define STATES_WAITING_FOR_SEMAPHORE           0x00200

   /**
    * @brief determine the value that corresponds to a task waiting for a mutex.
    */
#define STATES_WAITING_FOR_MUTEX               0x00400

   /**
    * @brief determine the value that corresponds to a task waiting for a condition variable.
    */
#define STATES_WAITING_FOR_CONDITION_VARIABLE  0x00800

   /**
    * @brief determine the value that corresponds to a task waiting for a join while exiting.
    */
#define STATES_WAITING_FOR_JOIN_AT_EXIT        0x01000

   /**
    * @brief determine the value that corresponds to a task waiting for a reply to an MPCI request.
    */
#define STATES_WAITING_FOR_RPC_REPLY           0x02000

   /**
    * @brief determine the value that corresponds to a task waiting for a period.
    */
#define STATES_WAITING_FOR_PERIOD              0x04000

   /**
    * @brief determine the value that corresponds to a task waiting for a signal.
    */
#define STATES_WAITING_FOR_SIGNAL              0x08000

   /**
    * @brief determine the value that corresponds to a task waiting for a barrier.
    */
#define STATES_WAITING_FOR_BARRIER             0x10000

   /**
    * @brief determine the value that corresponds to a task waiting for a RWLock.
    */
#define STATES_WAITING_FOR_RWLOCK              0x20000

   /**
    * @brief determine the value that corresponds to a task which is in an interruptible
    * blocking state.
    */
#define STATES_INTERRUPTIBLE_BY_SIGNAL         0x10000000

   /**
    * @brief determine the value that corresponds to a task waiting for a local object operation.
    */
#define STATES_LOCALLY_BLOCKED ( STATES_WAITING_FOR_BUFFER             | \
                                 STATES_WAITING_FOR_SEGMENT            | \
                                 STATES_WAITING_FOR_MESSAGE            | \
                                 STATES_WAITING_FOR_SEMAPHORE          | \
                                 STATES_WAITING_FOR_MUTEX              | \
                                 STATES_WAITING_FOR_CONDITION_VARIABLE | \
                                 STATES_WAITING_FOR_JOIN_AT_EXIT       | \
                                 STATES_WAITING_FOR_SIGNAL             | \
                                 STATES_WAITING_FOR_BARRIER            | \
                                 STATES_WAITING_FOR_RWLOCK             ) 

   /**
    * @brief determine the value that corresponds to a task waiting which is blocked on
    *  a thread queue.
    */
#define STATES_WAITING_ON_THREAD_QUEUE \
                               ( STATES_LOCALLY_BLOCKED         | \
                                 STATES_WAITING_FOR_RPC_REPLY   )

   /**
    * @brief determine the value that corresponds to a task waiting which is blocked.
    */
#define STATES_BLOCKED         ( STATES_DELAYING                | \
                                 STATES_WAITING_FOR_TIME        | \
                                 STATES_WAITING_FOR_PERIOD      | \
                                 STATES_WAITING_FOR_EVENT       | \
                                 STATES_WAITING_ON_THREAD_QUEUE | \
                                 STATES_INTERRUPTIBLE_BY_SIGNAL )

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/states.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */

/**
 *  @}
 */
