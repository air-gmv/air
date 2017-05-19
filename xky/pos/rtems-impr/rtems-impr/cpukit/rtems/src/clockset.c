/**
 *  @file
 *  clockset.c
 *
 *  @brief set the system clock
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
 *  7066    | 09/04/2010  | mcoutinho    | IPR 1931
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

#include <xky.h>

rtems_status_code rtems_clock_set(
                                  rtems_time_of_day *time_buffer
                                  )
{
    /* new time set in the form of a timespec */
    struct timespec newtime;

    /* check the time buffer address */
    if(!time_buffer)
    {
        /* return invalid address */
        return RTEMS_INVALID_ADDRESS;
    }

    /* if the time buffer is a valid time of day */
    if(_TOD_Validate(time_buffer))
    {
        /* then convert it to the number of seconds */
        newtime.tv_sec = _TOD_To_seconds(time_buffer);

        /* calculate the nanoseconds since last second */
        newtime.tv_nsec = time_buffer->ticks *
            ( _TOD_Microseconds_per_tick * TOD_NANOSECONDS_PER_MICROSECOND );

        /* disable thread dispatch (clock setting can activate sleeping tasks) */
        _Thread_Disable_dispatch();

        /* set the XKY time */
        xky_syscall_set_tod((xky_time_t *)&newtime);

        /* set the time */
        _TOD_Set(&newtime);

        /* re-enable thread dispatch */
        _Thread_Enable_dispatch();

        /* return success */
        return RTEMS_SUCCESSFUL;
    }

    /* return time buffer is not a valid time of day */
    return RTEMS_INVALID_CLOCK;
}

rtems_status_code TIME_SET(
                           rtems_time_of_day *time_buffer
                           )
{
    /* new time set in the form of a timespec */
    struct timespec newtime;

    /* check the time buffer address */
    if(!time_buffer)
    {
        /* return invalid address */
        return RTEMS_INVALID_ADDRESS;
    }

    /* if the time buffer is a valid time of day */
    if(_TOD_Validate(time_buffer))
    {
        /* then convert it to the number of seconds */
        newtime.tv_sec = _TOD_To_seconds(time_buffer);

        /* calculate the nanoseconds since last second */
        newtime.tv_nsec = time_buffer->ticks *
            ( _TOD_Microseconds_per_tick * TOD_NANOSECONDS_PER_MICROSECOND );

        /* disable thread dispatch (clock setting can activate sleeping tasks) */
        _Thread_Disable_dispatch();

        /* set the PMK time */
        xky_syscall_set_tod((xky_time_t *)&newtime);
		
        _TOD_Set(&newtime);

        /* re-enable thread dispatch */
        _Thread_Enable_dispatch();

        /* return success */
        return RTEMS_SUCCESSFUL;
    }

    /* return time buffer is not a valid time of day */
    return RTEMS_INVALID_CLOCK;
}

/**  
 *  @}
 */

/**
 *  @}
 */
