/**
 *  @file
 *  clockget.c
 *
 *  @brief get the system clock
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
 *  4844    | 15/10/2009  | mcoutinho    | IPR 402
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  7064    | 09/04/2010  | mcoutinho    | IPR 1931
 *  9293    | 03/01/2011  | mcoutinho    | IPR 2817
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
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
#include <rtems/rtems/intr.h>


rtems_status_code rtems_clock_get(
                                  rtems_clock_get_options option ,
                                  void *time_buffer
                                  )
{
    /* check if timer buffer is valid */
    if(!time_buffer)
    {
        /* return invalid address error */
        return RTEMS_INVALID_ADDRESS;
    }

    /* check the user option */
    switch(option)
    {
            /* check which option the caller selected */
        case RTEMS_CLOCK_GET_TOD:
        {
            /* intermediate time representation variable */
            struct tm time;

            /* current time obtained from the super core */
            struct timeval now;

            /* cast the time buffer to RTEMS internal time representation */
            rtems_time_of_day *tmbuf = (rtems_time_of_day *) time_buffer;


            /* check if the time of day is set */
            if(!_TOD_Is_set)
            {
                return RTEMS_NOT_DEFINED;
            }

            /* Obtain the current time */
            _TOD_Get_timeval(&now);

            /* Split it into a closer format */
            _mktm_r(&now.tv_sec , &time);

            /* Now adjust it to the RTEMS format */

            /* calculate year */
            tmbuf->year = time.tm_year + 1900;

            /* calculate month */
            tmbuf->month = time.tm_mon + 1;

            /* calculate day */
            tmbuf->day = time.tm_mday;

            /* calculate hour */
            tmbuf->hour = time.tm_hour;

            /* calculate minute */
            tmbuf->minute = time.tm_min;

            /* calculate second */
            tmbuf->second = time.tm_sec;

            /* calculate clock ticks elapsed from the number of microseconds */
            tmbuf->ticks = now.tv_usec / _TOD_Microseconds_per_tick;

            /* return success */
            return RTEMS_SUCCESSFUL;
        }

            /* the option is get the number of seconds since epoch */
        case RTEMS_CLOCK_GET_SECONDS_SINCE_EPOCH:
        {
            /* cast the time buffer to RTEMS interval */
            rtems_interval *interval = (rtems_interval *) time_buffer;


            /* if the time of day has not been set */
            if(!_TOD_Is_set)
            {
                /* then the time is not defined */
                return RTEMS_NOT_DEFINED;
            }

            /* otherwise, the internval is the number of seconds since epoch */
            *interval = _TOD_Seconds_since_epoch;

            /* return success */
            return RTEMS_SUCCESSFUL;
        }

            /* the option is get the number of clock ticks since the system boot */
        case RTEMS_CLOCK_GET_TICKS_SINCE_BOOT:
        {
            /* cast the time buffer to RTEMS interval */
            rtems_interval *interval = (rtems_interval *) time_buffer;

            /* interrupt level */
            rtems_interrupt_level level;


            /* enter critical section to protect _Watchdog_Ticks_since_boot variable */
            rtems_interrupt_disable(level);

            /* get the number of clock ticks since the system boot */
            *interval = _Watchdog_Ticks_since_boot;

            /* leave critical section */
            rtems_interrupt_enable(level);

            /* return success */
            return RTEMS_SUCCESSFUL;
        }

            /* the option is get the number of clock ticks per second */
        case RTEMS_CLOCK_GET_TICKS_PER_SECOND:
        {
            /* cast the time buffer to RTEMS interval */
            rtems_interval *interval = (rtems_interval *) time_buffer;


            /* get the number of clock ticks in one second */
            *interval = TOD_MICROSECONDS_PER_SECOND / _TOD_Microseconds_per_tick;

            /* return success */
            return RTEMS_SUCCESSFUL;
        }

            /* the option is get the time in a timeval structure type */
        case RTEMS_CLOCK_GET_TIME_VALUE:
        {
            /* cast the time buffer to timeval structure */
            struct timeval *time = (struct timeval *) time_buffer;

            
            /* if the time of day has not been set */
            if(!_TOD_Is_set)
            {
                /* then the time is not defined */
                return RTEMS_NOT_DEFINED;
            }

            /* otherwise, get the timeval */
            _TOD_Get_timeval(time);

            /* return success */
            return RTEMS_SUCCESSFUL;
        }

            /* the option is invalid */
        default:
        {
            /* invalid option */
            return RTEMS_UNSATISFIED;
        }
    }
}

/**  
 *  @}
 */

/**
 *  @}
 */
