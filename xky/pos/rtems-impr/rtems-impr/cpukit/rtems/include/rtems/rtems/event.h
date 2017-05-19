/**
 *  @file
 *  event.h
 *
 *  @brief contains the information pertaining to the Event
 *  Manager.  This manager provides a high performance method of communication
 *  and synchronization.
 *
 *  Directives provided are:
 *\n
 *     + send an event set to a task\n
 *     + receive event condition\n
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  3548    | 29/06/2009  | mcoutinho    | IPR 566
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @defgroup RTEMS_API_EVENT Event Manager
 *
 *  @brief The Event Manager allows the user to send and receive events from threads
 *  to one another. This allows thread synchronization.
 *  @{
 */

#ifndef _RTEMS_RTEMS_EVENT_H
#define _RTEMS_RTEMS_EVENT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/score/object.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/rtems/options.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>
#include <rtems/rtems/eventset.h>

   /**
    *  @brief this constant is passed as the event_in to the
    *  rtems_event_receive directive to determine which events are pending.
    */
#define EVENT_CURRENT  0

   /**
    *  @brief indicate what happened while the event
    *  manager was in the synchronization window.
    */
   typedef enum
   {
      EVENT_SYNC_SYNCHRONIZED ,
      EVENT_SYNC_NOTHING_HAPPENED ,
      EVENT_SYNC_TIMEOUT ,
      EVENT_SYNC_SATISFIED
   } Event_Sync_states;

   /**
    *  @brief initialize Event Manager
    *
    *  Event_Manager_initialization
    *  This routine performs the initialization necessary for this manager.
    */
   void _Event_Manager_initialization(void);

   /**
    *  @brief send an event
    *
    *  This routine implements the rtems_event_send directive.  It sends
    *  event_in to the task specified by ID.  If the task is blocked
    *  waiting to receive events and the posting of event_in satisfies
    *  the task's event condition, then it is unblocked.
    *
    *  @param[in] id the id of the task to send
    *  @param[in] event_in the event to send
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_event_send(
                                      Objects_Id id ,
                                      rtems_event_set event_in
                                      );

   /**
    *  @brief receive an event
    *
    *  This routine implements the rtems_event_receive directive.  This
    *  directive is invoked when the calling task wishes to receive
    *  the event_in event condition.  One of the fields in the option_set
    *  parameter determines whether the receive request is satisfied if
    *  any or all of the events are pending.   If the event condition
    *  is not satisfied immediately, then the task may block with an
    *  optional timeout of TICKS clock ticks or return immediately.
    *  This determination is based on another field in the option_set
    *  parameter.  This directive returns the events received in the
    *  event_out parameter.
    *
    *  @param[in] event_in the event to wait for
    *  @param[in] option_set the option
    *  @param[in] ticks the number of clock ticks to wait (if any)
    *  @param[out] event_out the events received
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_event_receive(
                                         rtems_event_set event_in ,
                                         rtems_option option_set ,
                                         rtems_interval ticks ,
                                         rtems_event_set *event_out
                                         );

   /**
    *  @brief seize an event
    *
    *  This routine attempts to satisfy the requested event condition
    *  for the running thread.
    *  event_out - event set output area filled in
    *  INTERRUPT LATENCY:
    *  available
    *  wait
    *  check sync
    *
    *  @param[in] event_in the event condition to satisfy
    *  @param[in] option_set acquire event options
    *  @param[in] ticks interval to wait
    *  @param[out] event_out pointer to event set output area
    */
   void _Event_Seize(
                     rtems_event_set event_in ,
                     rtems_option option_set ,
                     rtems_interval ticks ,
                     rtems_event_set *event_out
                     );

   /**
    *  @brief surrender an event
    *
    *  This routines remove a thread from the specified threadq.
    *  INTERRUPT LATENCY:
    *  before flash
    *  after flash
    *  check sync
    *
    *  @param[in] the_thread pointer to thread to be dequeued
    */
   void _Event_Surrender(
                         Thread_Control *the_thread
                         );

   /**
    *  @brief announce that an event timeout occurred
    *
    *  This routine is invoked when a task's event receive request
    *  has not been satisfied after the specified timeout interval.
    *  The task represented by ID will be unblocked and its status
    *  code will be set in it's control block to indicate that a timeout
    *  has occurred.
    *
    *  @param[in] id the id of the thread to unblock due to a timeout
    *  @param[in] ignored ignored
    */
   void _Event_Timeout(
                       Objects_Id id ,
                       void *ignored
                       );

   /**
    *  @brief define the synchronization flag used by the
    *  Event Manager to insure that signals sent to the currently
    *  executing thread are received properly.
    */
   extern volatile Event_Sync_states _Event_Sync_state;

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/eventmp.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

/**  
 *  @}
 */

/**
 *  @}
 */
