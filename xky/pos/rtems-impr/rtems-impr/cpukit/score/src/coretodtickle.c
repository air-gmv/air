/**
 *  @file
 *  coretodtickle.c
 *
 *  @brief announce the passage of a clock tick to the  Time Of Day (TOD)
 *  Handler
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
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
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
 *  @addtogroup ScoreTOD Time Of Day (TOD) Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/timespec.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>


void _TOD_Tickle_ticks(void)
{
    /* a clock tick in a timespec structure */
    struct timespec tick;

    /* number of seconds elapsed */
    uint32_t seconds;
    

    /* Convert the tick quantum to a timespec */

    /* convert a clock tick into nanoseconds */
    tick.tv_nsec = _TOD_Microseconds_per_tick * 1000;

    /* the number of seconds is 0 (assumed the clock tick fits in the
     * nanoseconds field */
    tick.tv_sec = 0;

    /* Update the counter of ticks since boot */
    _Watchdog_Ticks_since_boot += 1;

    /* Update the timespec format uptime */
    (void) _Timespec_Add_to(&_TOD_Uptime , &tick);
    /* we do not care how much the uptime changed */

    /* Update the timespec format TOD */
    seconds = _Timespec_Add_to(&_TOD_Now , &tick);

    /* while there are seconds left to ticke */
    while(seconds)
    {
        /* ticle one more second */
        _Watchdog_Tickle_seconds();

        /* decrement the number of seconds to tickle */
        seconds--;
    }
}


/**  
 *  @}
 */

/**
 *  @}
 */
