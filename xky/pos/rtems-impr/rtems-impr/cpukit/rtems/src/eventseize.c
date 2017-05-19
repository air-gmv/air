/**
 *  @file
 *  eventseize.c
 *
 *  @brief seize an event
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
 *  4426    | 21/09/2009  | mcoutinho    | IPR 619
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  7073    | 09/04/2010  | mcoutinho    | IPR 1931
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


void _Event_Seize(
                  rtems_event_set event_in ,
                  rtems_option option_set ,
                  rtems_interval ticks ,
                  rtems_event_set *event_out
                  )
{
    /* executing thread */
    Thread_Control *executing;

    /* seized events */
    rtems_event_set seized_events;

    /* pending events */
    rtems_event_set pending_events;

    /* interrupt level */
    ISR_Level level;

    /* executing thread API */
    RTEMS_API_Control *api;

    /* temporary variable to store the Event Manager sync state */
    Event_Sync_states sync_state;


    /* get the executing thread (for performance issues) */
    executing = _Thread_Executing;

    /* set the default return code for the executing thread as success */
    executing->Wait.return_code = RTEMS_SUCCESSFUL;

    /* get the executing thread api */
    api = executing->API_Extensions[ THREAD_API_RTEMS ];

    /* beggining of critical section */
    _ISR_Disable(level);

    /* get the pending events */
    pending_events = api->pending_events;

    /* get the events seized */
    seized_events = _Event_sets_Get(pending_events , event_in);

    /* if the events seized are sufficient not to block the thread */
    if(!_Event_sets_Is_empty(seized_events) &&
       ( seized_events == event_in || _Options_Is_any(option_set) ))
    {
        /* clear the pending events */
        api->pending_events = _Event_sets_Clear(pending_events , seized_events);

        /* leave critical section */
        _ISR_Enable(level);

        /* return the seized eventsd */
        *event_out = seized_events;

        /* and return */
        return;
    }

    /* if we get here, then the thread did not receive the events wanted */

    /* check if the thread should wait for an event */
    if(_Options_Is_no_wait(option_set))
    {
        /* dont wait, so enable interrupts */
        _ISR_Enable(level);

        /* and return unsatisfied */
        executing->Wait.return_code = RTEMS_UNSATISFIED;

        /* with the seized events */
        *event_out = seized_events;

        /* and return */
        return;
    }

    /* if we get here, then we must wait for the events */

    /* set the state to nothing happened */
    _Event_Sync_state = EVENT_SYNC_NOTHING_HAPPENED;

    /* update the thread TCB with the events data */

    /* update the TCB option */
    executing->Wait.option = (uint32_t) option_set;

    /* update the TCB count */
    executing->Wait.count = (uint32_t) event_in;

    /* update the TCB return argument */
    executing->Wait.return_argument = event_out;

    /* leave critical section */
    _ISR_Enable(level);

    /* if there is a timeout */
    if(ticks)
    {
        /* then initialize the watchdog timer */
        _Watchdog_Initialize(&executing->Timer ,
                             _Event_Timeout ,
                             executing->Object.id ,
                             NULL);

        /* and insert the watchdog on the watchdog clock ticks chain */
        _Watchdog_Insert_ticks(&executing->Timer , ticks);
    }

    /* set the thread state to waiting for event */
    _Thread_Set_state(executing , STATES_WAITING_FOR_EVENT);

    /* disable interrupts (entering critical section) */
    _ISR_Disable(level);

    /* get the event manager state */
    sync_state = _Event_Sync_state;

    /* and set the new state to synchronized */
    _Event_Sync_state = EVENT_SYNC_SYNCHRONIZED;

    /* check the old state */
    switch(sync_state)
    {
            /* This cannot happen.  It indicates that this routine did not
             * enter the synchronization states above. */
        case EVENT_SYNC_SYNCHRONIZED:

            /* raise internal error */
            _Internal_error_Occurred(INTERNAL_ERROR_RTEMS_API ,
                                     TRUE ,
                                     INTERNAL_ERROR_INVALID_EVENT_SYNC_STATE);

            /* leave critical section */
            _ISR_Enable(level);

            /* and return */
            return;

        case EVENT_SYNC_NOTHING_HAPPENED:

            /* leave critical section */
            _ISR_Enable(level);

            /* and return */
            return;

            /* there was a timeout */
        case EVENT_SYNC_TIMEOUT:

            /*set the return code to timeout */
            executing->Wait.return_code = RTEMS_TIMEOUT;

            /* leave critical section */
            _ISR_Enable(level);

            /* unblock the thread */
            _Thread_Unblock(executing);

            /* and return */
            return;

            /* the event set pending was received */
        case EVENT_SYNC_SATISFIED:

            /* if there was an active watchdog */
            if(_Watchdog_Is_active(&executing->Timer))
            {
                /* then deactivate it */
                _Watchdog_Deactivate(&executing->Timer);

                /* leave critical section */
                _ISR_Enable(level);

                /* and remove the watchdog */
                (void) _Watchdog_Remove(&executing->Timer);
            }
                /* if there was not an active watchdog */
            else
            {
                /* then just leave the critical section */
                _ISR_Enable(level);
            }

            /* unblock the thread */
            _Thread_Unblock(executing);

            /* and return */
            return;

            /* invalid event synchronization state
             * trigger internal error */
        default:

            /* raise internal error */
            _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                     TRUE ,
                                     INTERNAL_ERROR_INVALID_EVENT_SYNC_STATE);

            /* and return */
    }
}

/**  
 *  @}
 */

/**
 *  @}
 */
