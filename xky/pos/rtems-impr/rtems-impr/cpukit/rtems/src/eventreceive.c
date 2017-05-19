/**
 *  @file
 *  eventreceive.c
 *
 *  @brief receive an event
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
 *  3595    | 02/07/2009  | mcoutinho    | IPR 97
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  7072    | 09/04/2010  | mcoutinho    | IPR 1931
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
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


rtems_status_code rtems_event_receive(
                                      rtems_event_set event_in ,
                                      rtems_option option_set ,
                                      rtems_interval ticks ,
                                      rtems_event_set *event_out
                                      )
{
    /* thread API control */
    RTEMS_API_Control *api;

    /* return value of this function */
    rtems_status_code result;


    /* check if the event out address is valid*/
    if(!event_out)
    {
        /* if NULL, then return invalid address */
        result = RTEMS_INVALID_ADDRESS;
    }
    else
    {
        /* get the api control of the executing thread */
        api = _Thread_Executing->API_Extensions[ THREAD_API_RTEMS ];

        /* check if the event in set is empty */
        if(_Event_sets_Is_empty(event_in))
        {
            /* event in is empty, then return any pending events */
            *event_out = api->pending_events;

            /* return success */
            result = RTEMS_SUCCESSFUL;
        }
        else
        {
            /* event in is not empty, need to disable dispatch */
            _Thread_Disable_dispatch();

            /* seize the event (may envolve blocking */
            _Event_Seize(event_in , option_set , ticks , event_out);

            /* re-enable thread-dispatch */
            _Thread_Enable_dispatch();

            /* get the result  */
            result = (_Thread_Executing->Wait.return_code);
        }
    }

    /* return the result */
    return result;
}

/**  
 *  @}
 */

/**
 *  @}
 */
