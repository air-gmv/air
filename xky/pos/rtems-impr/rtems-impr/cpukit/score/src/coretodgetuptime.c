/**
 *  @file
 *  coretodgetuptime.c
 *
 *  @brief get the system uptime
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
#include <rtems/score/isr.h>
#include <rtems/score/timespec.h>
#include <rtems/score/tod.h>


void _TOD_Get_uptime(
                     struct timespec *uptime
                     )
{
    /* interrupt level */
    ISR_Level level;

    /* offset in nanoseconds */
    struct timespec offset;

    /* assume uptime checked by caller */

    /* initialize the offset */

    /* initialize the seconds */
    offset.tv_sec = 0;

    /* initialize the nanoseconds */
    offset.tv_nsec = 0;

    /* enter critical section */
    _ISR_Disable(level);

    /* get time */
    *uptime = _TOD_Uptime;

    /* find the nanosecond offset since _TOD_Now if possible */
    if(_Watchdog_Nanoseconds_since_tick_handler)
    {
        /* get the nanosecond offset */
        offset.tv_nsec = ( *_Watchdog_Nanoseconds_since_tick_handler )( );
    }

    /* leave critical section */
    _ISR_Enable(level);

    /* add the nanosecond offset to the time */
    _Timespec_Add_to(uptime , &offset);
}

/**  
 *  @}
 */

/**
 *  @}
 */
