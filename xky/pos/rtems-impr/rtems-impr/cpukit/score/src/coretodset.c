/**
 *  @file
 *  coretodset.c
 *
 *  @brief set the system time of day
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
 *  4473    | 22/09/2009  | mcoutinho    | IPR 559
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  9293    | 03/01/2011  | mcoutinho    | IPR 451
 *  9293    | 03/01/2011  | mcoutinho    | IPR 2816
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
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>
#include <rtems/rtems/timer.h>
#include <rtems/rtems/intr.h>

void _TOD_Set(
              const struct timespec *time
              )
{
    /* difference between the current time and the time to set */
    uint32_t units;

    /* interrupt level */
    rtems_interrupt_level level;


    /* disable thread dispatch */
    _Thread_Disable_dispatch();

    /* deactivate TOD */
    _TOD_Deactivate();

    /* if the new time is older than the current time */
    if(time->tv_sec < _TOD_Seconds_since_epoch)
    {
        /* calculate the time diference between the current time and the new time */
        units = _TOD_Seconds_since_epoch - time->tv_sec;

        /* set the timer seconds chain backwards if the
         * timer server has been initialized */
        if(_Timer_Server != NULL)
        {
            /* adjust the timer server seconds chain backward */
            _Watchdog_Adjust(&_Timer_Seconds_chain , WATCHDOG_BACKWARD , units);
        }

        /* set the watchdog seconds chain backwards */
        _Watchdog_Adjust_seconds(WATCHDOG_BACKWARD , units);
    }
        /* if the new time is younger than the current time */
    else
    {
        /* calculate the time diference between the new time and the current time */
        units = time->tv_sec - _TOD_Seconds_since_epoch;

        /* set the timer seconds chain forwards if the
         * timer server has been initialized */
        if(_Timer_Server != NULL)
        {
            /* adjust the timer server seconds chain forward */
            _Watchdog_Adjust(&_Timer_Seconds_chain , WATCHDOG_FORWARD , units);
        }

        /* set the watchdog seconds chain forwards */
        _Watchdog_Adjust_seconds(WATCHDOG_FORWARD , units);
    }

    /* enter critical section to protect _TOD_Now variable */
    rtems_interrupt_disable(level);

    /* copy to POSIX format TOD (timespec) */
    _TOD_Now = *time;

    /* leave critical section */
    rtems_interrupt_enable(level);

    /* time is now set */
    _TOD_Is_set = TRUE;

    /* re-enable TOD */
    _TOD_Activate();

    /* re-enable thread dispatch */
    _Thread_Enable_dispatch();
}

void _TOD_Update(
              const struct timespec *time
              )
{
    /* difference between the current time and the time to set */
    uint32_t units;
	

    /* if the new time is older than the current time */
    if(time->tv_sec < _TOD_Seconds_since_epoch)
    {
        /* calculate the time diference between the current time and the new time */
        units = _TOD_Seconds_since_epoch - time->tv_sec;
		
		 /* copy to POSIX format TOD (timespec) */
		_TOD_Now = *time;

        /* set the watchdog seconds chain backwards */
        _Watchdog_Adjust_seconds(WATCHDOG_BACKWARD , units);
    }
      /* If they are equal */  
	else if(time->tv_sec == _TOD_Seconds_since_epoch){
	
	}
		/* if the new time is younger than the current time */
	else
    {
        /* calculate the time diference between the new time and the current time */
        units = time->tv_sec - _TOD_Seconds_since_epoch;
		
		 /* copy to POSIX format TOD (timespec) */
		_TOD_Now = *time;

        /* set the watchdog seconds chain forwards */
        _Watchdog_Adjust_seconds(WATCHDOG_FORWARD , units);
    }

    /* time is now set */
    _TOD_Is_set = TRUE;

}

/**  
 *  @}
 */

/**
 *  @}
 */
