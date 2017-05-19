/**
 *  @file
 *  tod.h
 *
 *  @brief contains all the constants and structures associated
 *  with the Time of Day Handler.
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
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

#ifndef _RTEMS_SCORE_TOD_H
#define _RTEMS_SCORE_TOD_H

/**
 *  @defgroup ScoreTOD Time Of Day (TOD) Handler
 *
 *  @brief This handler encapsulates functionality used to manage time of day.
 *
 **/

/**
 *  @addtogroup ScoreTOD Time Of Day (TOD) Handler
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/score/object.h>
#include <time.h>

   /**
    *  @brief determine the number of seconds in a minute.
    */
#define TOD_SECONDS_PER_MINUTE (uint32_t)60

   /**
    *  @brief determine the number of minutes per hour.
    */
#define TOD_MINUTES_PER_HOUR   (uint32_t)60

   /**
    *  @brief determine the number of months in a year.
    */
#define TOD_MONTHS_PER_YEAR    (uint32_t)12

   /**
    *  @brief determine the number of days in a non-leap year.
    */
#define TOD_DAYS_PER_YEAR      (uint32_t)365

   /**
    *  @brief determine the number of hours per day.
    */
#define TOD_HOURS_PER_DAY      (uint32_t)24

   /**
    *  @brief determine the number of seconds in a day which does
    *  not include a leap second.
    */
#define TOD_SECONDS_PER_DAY    (uint32_t) (TOD_SECONDS_PER_MINUTE * \
                                TOD_MINUTES_PER_HOUR   * \
                                TOD_HOURS_PER_DAY)

   /**
    *  @brief determine the number of seconds in a non-leap year.
    */
#define TOD_SECONDS_PER_NON_LEAP_YEAR (365 * TOD_SECONDS_PER_DAY)

   /**
    *  @brief determine the number of seconds in a millisecond.
    */
#define TOD_MILLISECONDS_PER_SECOND     (uint32_t)1000

   /**
    *  @brief determine the number of microseconds in a second.
    */
#define TOD_MICROSECONDS_PER_SECOND     (uint32_t)1000000

   /**
    *  @brief determine the number of nanoseconds in a second.
    */
#define TOD_NANOSECONDS_PER_SECOND      (uint32_t)1000000000

   /**
    *  @brief determine the number of nanoseconds in a second.
    */
#define TOD_NANOSECONDS_PER_MICROSECOND (uint32_t)1000

   /**
    *  @brief determine the number of seconds from January 1, 1970 to
    *  January 1, 1988
    *
    * Seconds from January 1, 1970 to January 1, 1988.  Used to account for
    * differences between POSIX API and RTEMS core. The timespec format time
    * is kept in POSIX compliant form.
    */
#define TOD_SECONDS_1970_THROUGH_1988 \
  (((1987 - 1970 + 1)  * TOD_SECONDS_PER_NON_LEAP_YEAR) + \
  (4 * TOD_SECONDS_PER_DAY))



   /**
    * @brief calculate the number of ticks per second.
    */

#define TOD_TICKS_PER_SECOND \
           (TOD_MICROSECONDS_PER_SECOND / _TOD_Microseconds_per_tick)

   /**
    *  @brief determine the earliest year to which an
    *  time of day can be initialized.  This is considered the
    *  epoch.
    */
#define TOD_BASE_YEAR 1988

   /**
    *  @brief determine if the time of day has been set
    *
    *  This is TRUE if the application has set the current
    *  time of day, and FALSE otherwise.
    */
   extern boolean _TOD_Is_set;

   /**
    *  @brief Current Time of Day (Timespec)
    */
   extern struct timespec _TOD_Now;

   /**
    * @brief Current running time
    */
   extern struct timespec _TOD_Uptime;

   /**
    *  @brief Microseconds per Clock Tick
    *
    *  The following contains the number of microseconds per tick.
    */
   extern uint32_t _TOD_Microseconds_per_tick;

   /**
    *  @brief determine the seconds since RTEMS epoch
    *
    *  The following contains the number of seconds from 00:00:00
    *  January 1, TOD_BASE_YEAR until the current time of day.
    */
#define _TOD_Seconds_since_epoch (_TOD_Now.tv_sec)



   /**
    *  @brief initialize the time of day handler.
    *
    *  @param[in] microseconds_per_tick microseconds between clock ticks
    */
   void _TOD_Handler_initialization(
                                    uint32_t microseconds_per_tick
                                    );

   /**
    *  @brief set the current date and time with the specified
    *  new date and time structure.
    *
    *  @param[in] time pointer to the time and date structure
    */
   void _TOD_Set(
                 const struct timespec *time
                 );

   /**
    *  @brief get the current time of day
    *
    *  This routine returns the current time of day with potential accuracy
    *  to the nanosecond.
    *
    *  @param[out] time pointer to the time to be returned
    */
   void _TOD_Get(
                 struct timespec *time
                 );

   /**
    *  @brief get the system uptime
    *
    *  This routine returns the system uptime with potential accuracy
    *  to the nanosecond.
    *
    *  @param[out] uptime is a pointer to the uptime to be returned
    */
   void _TOD_Get_uptime(
                        struct timespec *uptime
                        );

   /**
    *  @brief announce the passage of a clock tick to the TOD Handler
    *
    *  This routine increments the ticks field of the current time of
    *  day at each clock tick.
    */
   void _TOD_Tickle_ticks(void);

   /**
    * @brief determine if y is a leap year
    *
    * @param[in] y year to determine if it is leap
    *
    * @return returns 1 if y is a leap year and 0 otherwise
    */
   int32_t _Is_Leap_Year(int32_t y);

   /**
    *  @brief convert miliseconds to microseconds
    *
    *  This routine converts an interval expressed in milliseconds to microseconds.
    *
    *  @note This must be a macro so it can be used in "static" tables.
    */
#define TOD_MILLISECONDS_TO_MICROSECONDS(_ms) ((uint32_t)(_ms) * 1000L)

   /**
    *  @brief convert microseconds to number of clock ticks
    *
    *  This routine converts an interval expressed in microseconds to ticks.
    *
    *  @note This must be a macro so it can be used in "static" tables.
    */
#define TOD_MICROSECONDS_TO_TICKS(_us) \
    ((_us) / _TOD_Microseconds_per_tick)

   /**
    *  @brief convert miliseconds to clock ticks
    *
    *  This routine converts an interval expressed in milliseconds to ticks.
    *
    *  @note This must be a macro so it can be used in "static" tables.
    */

#define TOD_MILLISECONDS_TO_TICKS(_ms) \
    (TOD_MILLISECONDS_TO_MICROSECONDS(_ms) / _TOD_Microseconds_per_tick)


   /**
    *  @brief calculate the number of clock ticks in a second
    *
    *  This macro returns the number of ticks in a second.
    *
    *  @note If the clock tick value does not multiply evenly into a second
    *        then this number of ticks will be slightly shorter than a second.
    */
#define TOD_TICKS_PER_SECOND \
  (TOD_MICROSECONDS_PER_SECOND / _TOD_Microseconds_per_tick)

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/tod.inl>
#endif

#ifdef __cplusplus
}
#endif



#endif
/* end of include file */

/**
 * @}
 **/

/**  
 *  @}
 */
