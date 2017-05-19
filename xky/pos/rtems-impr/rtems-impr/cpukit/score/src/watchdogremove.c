/**
 *  @file
 *  watchdogremove.c
 *
 *  @brief remove a watchdog
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
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreWatchdog Watchdog Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/watchdog.h>


Watchdog_States _Watchdog_Remove(
                                 Watchdog_Control *the_watchdog
                                 )
{
    /* interrupt level */
    ISR_Level level;

    /* previous watchdog state */
    Watchdog_States previous_state;

    /* next watchdog */
    Watchdog_Control *next_watchdog;


    /* enter critical section */
    _ISR_Disable(level);

    /* get current watchdog state */
    previous_state = the_watchdog->state;

    /* check the watchdog state */
    switch(previous_state)
    {
            /* inactive, nothing to be done */
        case WATCHDOG_INACTIVE:

            /* just leave */
            break;

            /* being inserted */
        case WATCHDOG_BEING_INSERTED:

            /* it is not actually on the chain so just change the state and
             * the insert operation we interrupted will be aborted */
            the_watchdog->state = WATCHDOG_INACTIVE;

            /* and leave */
            break;

            /* active */
        case WATCHDOG_ACTIVE:

            /* remove it */
        case WATCHDOG_REMOVE_IT:

            /* update the watchdog state */
            the_watchdog->state = WATCHDOG_INACTIVE;

            /* get the next watchdog */
            next_watchdog = _Watchdog_Next(the_watchdog);

            /* if there is a next watchdog then */
            if(_Watchdog_Next(next_watchdog))
            {
                /* increase the interval of the next wathdog by the removed watchdog interval */
                next_watchdog->delta_interval += the_watchdog->delta_interval;
            }

            /* if the watchdog sync count is not 0 */
            if(_Watchdog_Sync_count)
            {
                /* then set the watchdog sync state to the interrupt nest level */
                _Watchdog_Sync_level = _ISR_Nest_level;
            }

            /* extract the watchdog from the chain */
            _Chain_Extract_unprotected(&the_watchdog->Node);

            /* and leave */
            break;

            /* invalid watchdog state */
        default:

            /* raise internal error */
            _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                     TRUE ,
                                     INTERNAL_ERROR_INVALID_WATCHDOG_STATE);

    }

    /* set the watchdog stop time to the current time */
    the_watchdog->stop_time = _Watchdog_Ticks_since_boot;

    /* leave critical section */
    _ISR_Enable(level);

    /* return the previous state */
    return ( previous_state );
}

/**  
 *  @}
 */

/**
 *  @}
 */
