/**
 *  @file
 *  ratemontimeout.c
 *
 *  @brief rate monotonic period has timeout
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
 *  579     | 17/11/2008  | mcoutinho    | IPR 64
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  7098    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_RATEMON Rate Monotonic Manager
 *  @{
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/ratemon.h>
#include <rtems/score/thread.h>


void _Rate_monotonic_Timeout(
                             Objects_Id id ,
                             void *ignored
                             )
{
    /* rate monotonic object that timeout */
    Rate_monotonic_Control *the_period;

    /* rate monotonic object that timeout location */
    Objects_Locations location;

    /* task that possibly awake from the rate monotonic object timeout */
    Thread_Control *the_thread;

    /* When we get here, the Timer is already off the chain so we do not
     * have to worry about that -- hence no _Watchdog_Remove() */

    /* get the period */
    the_period = _Rate_monotonic_Get(id , &location);

    /* check the rate monotonic object location */
    switch(location)
    {
#if defined(RTEMS_MULTIPROCESSING)

            /* rate monotonic object is remote */
        case OBJECTS_REMOTE:
            /* impossible */
#endif

            /* rate monotonic object id is invalid */
        case OBJECTS_ERROR:

            /* leave */
            break;

            /* rate monotonic object is local (nominal case) */
        case OBJECTS_LOCAL:

            /* get the owner task of the period */
            the_thread = the_period->owner;

            /* check if the thread was waiting for the period */
            if(_States_Is_waiting_for_period(the_thread->current_state) &&
               the_thread->Wait.id == the_period->Object.id)
            {
                /* thread was waiting */

                /* unblock the thread */
                _Thread_Unblock(the_thread);

                /* and reset the watchdog (insert it again for the next period) */
                _Watchdog_Insert_ticks(&the_period->Timer , the_period->next_length);
            }
                /* else, check of the period state was that the period owner was blocking */
            else if(the_period->state == RATE_MONOTONIC_OWNER_IS_BLOCKING)
            {
                /* if the owner was blocking then set the appropriate state */
                the_period->state = RATE_MONOTONIC_EXPIRED_WHILE_BLOCKING;

                /* and reset the watchdog (insert it again for the next period) */
                _Watchdog_Insert_ticks(&the_period->Timer , the_period->next_length);
            }
            else
            {
                /* the period has expired and the owner wasnt blocking */
                the_period->state = RATE_MONOTONIC_EXPIRED;

                /* so dont reset the watchdog */
            }

            /* unnest dispatch (disabled by _Rate_monotonic_Get) */
            _Thread_Unnest_dispatch();

            /* and leave */
            break;

            /* default clause, invalid object location */
        default:

            /* raise internal error */
            _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                     TRUE ,
                                     INTERNAL_ERROR_INVALID_OBJECT_LOCATION);
    }
}

/**  
 *  @}
 */

/**
 *  @}
 */
