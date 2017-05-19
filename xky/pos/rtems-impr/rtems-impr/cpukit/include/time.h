/**
 *  @file
 *  time.h
 *
 *  @brief Struct and function declarations for dealing with time.
 *
 *  COPYRIGHT (c) 2010.
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  9622    | 23/02/2011  | mcoutinho    | IPR 67
 *  9622    | 23/02/2011  | mcoutinho    | IPR 451
 *  9877    | 18/03/2011  | aconstantino | SPR 2819
 *  $Rev: 9880 $ | $Date: 2011-03-18 19:58:36 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2853
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_COMMON Common
 *  @{
 */

/* if using the newlib, then */
#ifdef RTEMS_USES_NEWLIB
/* include the newlib header file */
#include_next <time.h>
#else

#ifndef _RTEMS_TIME_H_
#define _RTEMS_TIME_H_

#include <stddef.h>
#include <rtems/score/types.h>


/**
 * @brief time in number of seconds
 **/
typedef long int time_t;

/**
 * @brief time of day
 **/
struct tm
{
   /**
    * @brief second
    */
   int tm_sec;

   /**
    * @brief minute
    */
   int tm_min;

   /**
    * @brief hour
    */
   int tm_hour;

   /**
    * @brief day of month
    */
   int tm_mday;

   /**
    * @brief month
    */
   int tm_mon;

   /**
    * @brief year
    */
   int tm_year;

   /**
    * @brief week day
    */
   int tm_wday;

   /**
    * @brief day of year
    */
   int tm_yday;

   /**
    * @brief day light saving
    */
   int tm_isdst;
};


/**
 * @brief A time value that is accurate to the nearest
   microsecond but also has a range of years.
 */
struct timeval
{
   /**
    * @brief seconds
    */
   int tv_sec;

   /**
    * @brief microseconds
    */
   int tv_usec;
};

/**
 * @brief POSIX.1b structure for a time value.
 *
 * This is like a `struct timeval' but
 * has nanoseconds instead of microseconds.
 */
struct timespec
{
   /**
    * @brief seconds
    */
   int tv_sec;

   /**
    * @brief microseconds
    */
   int tv_nsec;
};


/**
 * @brief converts a calender time to a broken-down time
 *  expressed as local time
 *
 *  @param[in] tim_p time in number of seconds
 *  @param[out] res time converted into the struct tm format
 *
 *  @return pointer to a structure containing the broken-down time.
 **/
struct tm *_mktm_r(const time_t *tim_p , struct tm *res);

#endif /* RTEMS_USES_NEWLIB */

#endif /* _RTEMS_TIME_H_ */

/**
 *  @}
 */
