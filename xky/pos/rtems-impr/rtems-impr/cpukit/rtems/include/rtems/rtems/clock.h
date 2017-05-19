/**
 *  @file
 *  clock.h
 *
 *  @brief contains all the constants and structures associated
 *  with the Clock Manager.  This manager provides facilities to set, obtain,
 *  and continually update the current date and time.
 *
 *  This manager provides directives to:
 *
 *     + set the current date and time
 *     + obtain the current date and time
 *     + set the nanoseconds since last clock tick handler
 *     + announce a clock tick
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
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @defgroup RTEMS_API_CLOCK Clock Manager
 *
 *   @brief The Clock Manager component contains the code to manage time and the
 *  interface with the clock driver (announce the passage of time).
 *  @{
 */

#ifndef _RTEMS_RTEMS_CLOCK_H
#define _RTEMS_RTEMS_CLOCK_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>

   /**
    *  @brief list of things which can be returned by the rtems_clock_get directive.
    */
   typedef enum
   {
      RTEMS_CLOCK_GET_TOD ,
      RTEMS_CLOCK_GET_SECONDS_SINCE_EPOCH ,
      RTEMS_CLOCK_GET_TICKS_SINCE_BOOT ,
      RTEMS_CLOCK_GET_TICKS_PER_SECOND ,
      RTEMS_CLOCK_GET_TIME_VALUE
   } rtems_clock_get_options;

   /**
    *  @brief standard flavor style to return TOD in for a rtems_clock_get option.
    */
   typedef struct
   {
      uint32_t seconds;
      uint32_t microseconds;
   } rtems_clock_time_value;

   /**
    *  @brief type for the nanoseconds since last tick BSP extension.
    */
   typedef Watchdog_Nanoseconds_since_last_tick_routine
   rtems_nanoseconds_extension_routine;

   /**
    *  @brief get system clock
    *
    *  This directive returns the current date and time.  If the time has
    *  not been set by a tm_set then an error is returned.
    *
    *  @param[in] option which value to return
    *  @param[out] time_buffer pointer to output buffer (a time and date structure
    *  or an interval)
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_clock_get(
                                     rtems_clock_get_options option ,
                                     void *time_buffer
                                     );

   /**
    *  @brief set system clock
    *
    *  This directive sets the date and time for this node.
    *
    *  @param[in] time_buffer pointer to the time and date structure
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_clock_set(
                                     rtems_time_of_day *time_buffer
                                     );

   /**
    *  @brief announce the passage of a clock tick
    *
    *  This directive notifies the executve that a tick has occurred.
    *  When the tick occurs the time manager updates and maintains
    *  the calendar time, timeslicing, and any timeout delays.
    *
    *  @return RTEMS_SUCCESSFUL always succeeds
    *
    *  @note This routine only works for leap-years through 2099.
    */
   rtems_status_code rtems_clock_tick(void);
   
   /**
    *  @brief announce the passage of several clock ticks
    *
    *  This directive notifies the executive that several ticks have occurred.
    *  When the tick occurs the time manager updates and maintains
    *  the calendar time, timeslicing, and any timeout delays.
    *
    *  @return RTEMS_SUCCESSFUL always succeeds
    *
    *  @note This routine only works for leap-years through 2099.
	*/
    rtems_status_code rtems_clock_update(long ticks);

   /**
    *  @brief set nanosecond extension routine
    *
    *  This directive sets the BSP provided nanoseconds since last tick
    *  extension.
    *
    *  @param[in] routine pointer to the extension routine
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_clock_set_nanoseconds_extension(
                                                           rtems_nanoseconds_extension_routine routine
                                                           );

   /**
    *  @brief get system uptime
    *
    *  This directive obtains the system uptime.  A timestamp is the seconds
    *  and nanoseconds since boot.
    *
    *  @param[out] uptime pointer to the timestamp uptime filled in
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_clock_get_uptime(
                                            struct timespec *uptime
                                            );

   /**
    *  @brief validate a time of day
    *
    *  This kernel routine checks the validity of a date and time structure.
    *
    *  @param[in] the_tod pointer to a time and date structure
    *
    *  @return TRUE if the date, time, and tick are valid
    *  FALSE if the the_tod is invalid
    *
    *  @note This routine only works for leap-years through 2099.
    */
   boolean _TOD_Validate(
                         rtems_time_of_day *the_tod
                         );

   /**
    *  @brief convert time of day to seconds
    *
    *  This routine returns the seconds from the epoch until the
    *  current date and time.
    *
    *  @param[in] the_tod pointer to the time and date structure
    *
    *  @return returns seconds since epoch until the_tod
    */
   Watchdog_Interval _TOD_To_seconds(
                                     rtems_time_of_day *the_tod
                                     );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

/**  
 *  @}
 */

/**
 *  @}
 */
