/**
 *  @file
 *  clocktodvalidate.c
 *
 *  @brief validate a time of day
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
 *  7070    | 09/04/2010  | mcoutinho    | IPR 1931
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

/**
 *  @brief The following array contains the number of days in all months.
 *
 *  The first dimension should be 1 for leap years, and 0 otherwise.
 *  The second dimension should range from 1 to 12 for January to
 *  February, respectively.
 */
const uint32_t _TOD_Days_per_month[ 2 ][ 13 ] = {
    { 0 , 31 , 28 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31 } ,
    { 0 , 31 , 29 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31 }
};


boolean _TOD_Validate(
                      rtems_time_of_day *the_tod
                      )
{
    /* number of days in the specfied month */
    uint32_t days_in_month;


    /* check if the time of day (except for number of days) is within bounderies:
     *   year must be greater than 1970
     *   month cannot be greater than 12
     *   month cannot be 0
     *   hour cannot be greater than 24
     *   minute cannot be greater than 59
     *   second cannot be greater than 59
     *   ticks cannot be greater than the number of clock ticks in one second
     *   tod address cannot be NULL
     *   day cannot be 0 */
    if(( !the_tod ) ||
       ( the_tod->ticks >=
         ( TOD_MICROSECONDS_PER_SECOND / _TOD_Microseconds_per_tick ) ) ||
       ( the_tod->second >= TOD_SECONDS_PER_MINUTE ) ||
       ( the_tod->minute >= TOD_MINUTES_PER_HOUR ) ||
       ( the_tod->hour >= TOD_HOURS_PER_DAY ) ||
       ( the_tod->month == 0 ) ||
       ( the_tod->month > TOD_MONTHS_PER_YEAR ) ||
       ( the_tod->year < TOD_BASE_YEAR ) ||
       ( the_tod->day == 0 ))
    {
        /* tod is incorrect */
        return FALSE;
    }

    /* get the number of days based on the year (can be leap or not) and the month */
    days_in_month = _TOD_Days_per_month[ _Is_Leap_Year(the_tod->year) ][ the_tod->month ];

    /* check if the day is within bounderies */
    if(the_tod->day > days_in_month)
    {
        /* tod is incorrect */
        return FALSE;
    }

    /* if the checks passed then everything was correct */
    return TRUE;
}

/**  
 *  @}
 */

/**
 *  @}
 */
