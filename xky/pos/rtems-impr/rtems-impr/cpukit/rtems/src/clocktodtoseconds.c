/**
 *  @file
 *  clocktodtoseconds.c
 *
 *  @brief convert the time of day to number of seconds
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
 *  5934    | 11/01/2010  | mcoutinho    | IPR 1046
 *  7069    | 09/04/2010  | mcoutinho    | IPR 1931
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
#include <rtems/rtems/clock.h>


uint32_t _TOD_To_seconds(
                         rtems_time_of_day *the_tod
                         )
{
    /* set the month to the tod month minus 2 (February) */
   int32_t month = the_tod->month - 2;

   /* get the year */
   int32_t year = the_tod->year;

   /* the result */
   uint32_t result;

   
   /* start the months in March (see Gauss Algorithm) */
   if(month <= 0)
   {
      /* Puts Feb last since it has leap day */
      month += 12;

      /* subtract one year */
      year -= 1;
   }

   /* start with the number of days */
   result = the_tod->day;

   /* add the number of days per month elapsed */
   result += 367 * month / 12;

   /* add the number of leap days elapsed */
   result += year / 4 - year / 100 + year / 400;

   /* calculate number of days since 1970 */
   result += year * 365 - 719499;

   /* have the total number of days, now just calculate the seconds */
   return (((result * 24 + the_tod->hour) /* get hours */
       * 60 + the_tod->minute) /* get minutes */
       * 60 + the_tod->second); /* get seconds */
}

/**  
 *  @}
 */

/**
 *  @}
 */
