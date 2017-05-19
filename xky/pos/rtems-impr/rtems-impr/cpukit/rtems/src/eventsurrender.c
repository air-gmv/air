/**
 *  @file
 *  eventsurrender.c
 *
 *  @brief surrender an event: remove a thread from a thread queue
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  7075    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_EVENT Event Manager
 *  @{
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/event.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/tasks.h>


void _Event_Surrender(
                      Thread_Control *the_thread
                      )
{
    /* interrupt level */
    ISR_Level level;

    /* pending events */
    rtems_event_set pending_events;

    /* event condition  */
    rtems_event_set event_condition;

    /* seized events */
    rtems_event_set seized_events;

    /* option */
    rtems_option option_set;

    /* the thread API */
    RTEMS_API_Control *api;

    
    /* get the thread API */
    api = the_thread->API_Extensions[ THREAD_API_RTEMS ];

    /* get the destination thread event option set */
    option_set = (rtems_option) the_thread->Wait.option;

    /* enter critical section */
    _ISR_Disable(level);

    /* get the pending events for the destination thread */
    pending_events = api->pending_events;

    /* get all the event set the destination thread is waiting on */
    event_condition = (rtems_event_set) the_thread->Wait.count;

    /* get the new events seized */
    seized_events = _Event_sets_Get(pending_events , event_condition);

    /* if there are new events seized */
    if(!_Event_sets_Is_empty(seized_events))
    {
        /* then if the destination thread is waiting for an event
         * and if the new seized events are enough to awake the thread */
        if(_States_Is_waiting_for_event(the_thread->current_state))
        {
            /* and if the new seized events are enough to awake the thread */
            if(seized_events == event_condition || _Options_Is_any(option_set))
            {
                /* set the pending events with the old pending events minus the
                 * seized events */
                api->pending_events = _Event_sets_Clear(pending_events ,
                                                        seized_events);

                /* clear the thread event condition (is not waiting for an event anymore) */
                the_thread->Wait.count = 0;

                /* set the return argument to the seized events */
                *(rtems_event_set *) the_thread->Wait.return_argument = seized_events;

                /* allow interrupts here */
                _ISR_Flash(level);

                /* check if the watchdog is not active */
                if(!_Watchdog_Is_active(&the_thread->Timer))
                {
                    /* if not active, then enable interrupts */
                    _ISR_Enable(level);

                    /* and unblock the thread */
                    _Thread_Unblock(the_thread);
                }
                else
                {
                    /* if active, then deactivate the watchdog */
                    _Watchdog_Deactivate(&the_thread->Timer);

                    /* enable interrupts */
                    _ISR_Enable(level);

                    /* remove the watchdog */
                    (void) _Watchdog_Remove(&the_thread->Timer);

                    /* unblock the thread (was waiting) */
                    _Thread_Unblock(the_thread);
                }

                /* and return */
                return;
            }
        }

        /* check the event synchornization state */
        switch(_Event_Sync_state)
        {

                /* if synchronized or satisfied then nothing to be done */
            case EVENT_SYNC_SYNCHRONIZED:
            case EVENT_SYNC_SATISFIED:
                break;

                /* if nothing happend or timeout */
            case EVENT_SYNC_NOTHING_HAPPENED:
            case EVENT_SYNC_TIMEOUT:

                /* then check if the destination thread is not the executing thread */
                if(!_Thread_Is_executing(the_thread))
                {
                    /* if it is not, then leave */
                    break;
                }

                /* if it is the executing thread, then check if the seized events
                 * are sufficient to unblock it */
                if(seized_events == event_condition || _Options_Is_any(option_set))
                {
                    /* set the pending events with the old pending events minus the
                     * seized events */
                    api->pending_events = _Event_sets_Clear(pending_events ,
                                                            seized_events);

                    /* clear the events (no longer waiting on an event */
                    the_thread->Wait.count = 0;

                    /* set the seized events */
                    *(rtems_event_set *) the_thread->Wait.return_argument = seized_events;

                    /* change the state to satisfied */
                    _Event_Sync_state = EVENT_SYNC_SATISFIED;
                }

                /* and leave */
                break;

                /* this state should be obtained, internal error occurred */
            default:

                /* raise internal error */
                _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                         TRUE ,
                                         INTERNAL_ERROR_INVALID_EVENT_SYNC_STATE);
        }
    }

    /* enable interrupts and leave */
    _ISR_Enable(level);
}

/**  
 *  @}
 */

/**
 *  @}
 */
