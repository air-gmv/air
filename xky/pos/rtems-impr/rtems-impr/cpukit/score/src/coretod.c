/**
 *  @file
 *  coretod.c
 *
 *  @brief initialize the Time Of Day (TOD) Handler
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
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  8317    | 21/06/2010  | mcoutinho    | IPR 451
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


boolean _TOD_Is_set;


struct timespec _TOD_Now;


struct timespec _TOD_Uptime;


uint32_t _TOD_Microseconds_per_tick;


void _TOD_Handler_initialization(
                                 uint32_t microseconds_per_tick
                                 )
{
    /* initialize the number of microseconds per clock tick */
   _TOD_Microseconds_per_tick = microseconds_per_tick;

   /* initialize POSIX format TOD (timespec) */

   /* initialize the number of seconds */
   _TOD_Now.tv_sec = TOD_SECONDS_1970_THROUGH_1988;

   /* initialize the number of nanoseconds */
   _TOD_Now.tv_nsec = 0;

   /* initialize Uptime (timespec) */

   /* initialize the number of seconds */
   _TOD_Uptime.tv_sec = 0;

   /* initialize the number of nanoseconds */
   _TOD_Uptime.tv_nsec = 0;

   /* TOD has not been set */
   _TOD_Is_set = FALSE;

   /* activate the time of day */
   _TOD_Activate();
}

/**  
 *  @}
 */

/**
 *  @}
 */
