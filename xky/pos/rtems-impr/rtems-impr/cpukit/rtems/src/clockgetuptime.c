/**
 *  @file
 *  clockgetuptime.c
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
 *  7065    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_CLOCK Clock Manager
 *  @{
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/clock.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>


rtems_status_code rtems_clock_get_uptime(
                                         struct timespec *uptime
                                         )
{
    /* check the uptime address */
    if(!uptime)
    {
        /*if uptime is zero it is not definned at this time*/
        return RTEMS_INVALID_ADDRESS;
    }

    /* get the time of day */
    _TOD_Get_uptime(uptime);

    /* and return success */
    return RTEMS_SUCCESSFUL;
}

/**  
 *  @}
 */

/**
 *  @}
 */
