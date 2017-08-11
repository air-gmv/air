/**
 *  @file
 *  clocktick.c
 *
 *  @brief announce to the system the passage of a clock tick
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
 *  7068    | 09/04/2010  | mcoutinho    | IPR 1931
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
#include <rtems/score/timespec.h>
#include <rtems/score/watchdog.h>


rtems_status_code rtems_clock_tick(void)
{
   /* tickle the time of day */
   _TOD_Tickle_ticks();

   /* tickle the watchdog, may awake tasks */
   _Watchdog_Tickle_ticks();

   /* tickle the timeslice, may change running task */
   _Thread_Tickle_timeslice();

   /* check if context switch is necessary and thread dispacth is enabled */
   if(_Thread_Is_context_switch_necessary() &&
      _Thread_Is_dispatching_enabled())
   {
      /* dispatch the thread */
      _Thread_Dispatch();
   }

   /* return success, nothing should go wrong on this function */
   return RTEMS_SUCCESSFUL;
}

void rtems_time_travel(long ticks)
{
   /* a clock tick in a timespec structure */
   struct timespec tick;

   /* number of seconds elapsed */
   uint32_t seconds;
    

   /* Convert the tick quantum to a timespec */

   /* convert a clock tick into nanoseconds */
   tick.tv_nsec = _TOD_Microseconds_per_tick * 1000 * ticks;

   /* the number of seconds is 0 (assumed the clock tick fits in the
    * nanoseconds field */
   tick.tv_sec = 0;

   /* Update the counter of ticks since boot */
   _Watchdog_Ticks_since_boot += ticks;

   /* Update the timespec format uptime */
   (void) _Timespec_Add_to(&_TOD_Uptime , &tick);

   /* Update the timespec format TOD */
   seconds = _Timespec_Add_to(&_TOD_Now , &tick);
	
   /* adjust ticks chain*/
   _Watchdog_Adjust_ticks(ticks);

   /* If one second has passed (or more)*/
   if(seconds > 0) {
   
		/* adjust seconds chain */
		_Watchdog_Adjust_seconds(WATCHDOG_FORWARD, seconds);
   }

   _Context_Switch_necessary = TRUE;
}

/* FIXME GMVS return something on this function. At least a RTEMS_SUCCESSFUL*/

rtems_status_code rtems_clock_update(long ticks)
{
   if(ticks == 1)
   {
      /* use default rtems_clock_tick */
      rtems_clock_tick();
   
   } else
   {
	  /* Advance in time several ticks */
	  rtems_time_travel(ticks);

   }
}


/**  
 *  @}
 */

/**
 *  @}
 */
