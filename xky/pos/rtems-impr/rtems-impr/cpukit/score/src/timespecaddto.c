/**
 *  @file
 *  timespecaddto.c
 *
 *  @brief add two timespecs
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
 *  6116    | 18/02/2010  | mcoutinho    | IPR 1818
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  8318    | 21/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup HELPERS Helpers
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/types.h>
#include <rtems/score/timespec.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>


uint32_t _Timespec_Add_to(
                          struct timespec *time ,
                          const struct timespec *add
                          )
{
    /* seconds to add */
    uint32_t seconds = add->tv_sec;

    /* add the basics */
    time->tv_sec += add->tv_sec;
    time->tv_nsec += add->tv_nsec;

    /* Now adjust it so nanoseconds is in range */
    while(time->tv_nsec >= TOD_NANOSECONDS_PER_SECOND)
    {
        /* decrease the nanoseconds by one second */
        time->tv_nsec -= TOD_NANOSECONDS_PER_SECOND;

        /* increase the seconds */
        time->tv_sec++;

        /* increase the seconds */
        seconds++;
    }

    /* return the number of seconds */
    return seconds;
}

/**  
 *  @}
 */

/**
 *  @}
 */
