/**
 *  @file
 *
 *  @brief Implementation of the function mktm_r
 *
 * Original Author:	Adapted from tzcode maintained by Arthur David Olson.
 *
 *  Partial Modifications by Edisoft S.A.
 *
 * Modifications:
 *   Changed to mktm_r and added __tzcalc_limits - 04/10/02, Jeff Johnston
 *   
 *   Fixed bug in mday computations - 08/12/04, Alex Mogilnikov
 *   <alx@intellectronika.ru>
 *   
 *   Fixed bug in __tzcalc_limits - 08/12/04, Alex Mogilnikov
 *   <alx@intellectronika.ru>
 *
 *  $Id: mktm_r.c 8184 2010-06-15 10:55:59Z mcoutinho $
 *
 *  $Change description: created file to contain the mktm_r and _Is_Leap_Year
 *  functions
 *  Author: mcoutinho$
 *
 */

/**
 *  @addtogroup LIBC LibC
 *  @{
 */

/* local header used by libc/time routines */
#include <time.h>


#define SECSPERMIN	60L
#define MINSPERHOUR	60L
#define HOURSPERDAY	24L
#define SECSPERHOUR	(SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY	(SECSPERHOUR * HOURSPERDAY)
#define DAYSPERWEEK	7
#define MONSPERYEAR	12

#define YEAR_BASE	1900
#define EPOCH_YEAR      1970
#define EPOCH_WDAY      4
#define EPOCH_YEARS_SINCE_LEAP 2
#define EPOCH_YEARS_SINCE_CENTURY 70
#define EPOCH_YEARS_SINCE_LEAP_CENTURY 370

typedef int edilib_int32;

struct tm *_mktm_r(const time_t *tim_p , struct tm *res)
{
    /* number of days */
    edilib_int32 days;

    /* number of remainder days */
    edilib_int32 rem;

    /* local time */
    time_t lcltime;

    /* year */
    edilib_int32 y;

    /* flag indicating if is a leap year */
    edilib_int32 yleap;

    /* table containing the number of days on a non-leap and a leap year */
    static const edilib_int32 year_lengths[2] = {
                                            365 ,
                                            366
    };

    /* table containing the days per month on a non-leap and a leap year */
    static const edilib_int32 mon_lengths[2][MONSPERYEAR] = {
        {31 , 28 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31} ,
        {31 , 29 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31}
    };


    /* base decision about std/dst time on current time */
    lcltime = *tim_p;

    /* get the number of days */
    days = ((edilib_int32) lcltime) / SECSPERDAY;

    /* get the remainder of days */
    rem = ((edilib_int32) lcltime) % SECSPERDAY;

    /* while there are days left in the remainder */
    while(rem < 0)
    {
        /* add the number of seconds */
        rem += SECSPERDAY;

        /* and decrement the number of days */
        days--;
    }

    /* compute hour, */
    res->tm_hour = (edilib_int32) (rem / SECSPERHOUR);
    rem %= SECSPERHOUR;

    /* minute */
    res->tm_min = (edilib_int32) (rem / SECSPERMIN);

    /* and second */
    res->tm_sec = (edilib_int32) (rem % SECSPERMIN);

    /* compute day of week */

    /* week day */
    res->tm_wday = (EPOCH_WDAY + days) % DAYSPERWEEK;

    /* if day of week is smaller than 0 */
    if(res->tm_wday < 0)
    {
        /* increment the week day */
        res->tm_wday += DAYSPERWEEK;
    }

    /* compute year & day of year */

    /* initialize the year to the epoch year */
    y = EPOCH_YEAR;

    /* if days are larger than zero */
    if(days >= 0)
    {
        /* iterate the number of days until the year is found
         * in each iteration determine if the year is leap or not
         * and search the year until the number of days is smaller than the
         * number of days in the year */
        for(yleap = _Is_Leap_Year(y);
            days >= year_lengths[yleap];
            yleap = _Is_Leap_Year(y))
        {
            /* did not find the year, check next year */
            y++;

            /* and decrement the number of days */
            days -= year_lengths[yleap];
        }
    }
    else
    {
        /* days are negative, search backwards */

        do
        {
            /* decrement the number of years */
            y--;

            /* check if year is leap year */
            yleap = _Is_Leap_Year(y);

            /* increment the number of days */
            days += year_lengths[yleap];

        }
        while(days < 0); /* check if found the year */
    }

    /* calculate the year starting from the base year */
    res->tm_year = y - YEAR_BASE;

    /* set the number of days */
    res->tm_yday = (edilib_int32) days;

    /* search the month */
    for(res->tm_mon = 0;
        days >= mon_lengths[yleap][res->tm_mon];
        res->tm_mon++)
    {
        /* decrement the number of days of the month until the number of days
         * is smaller than the number of days in the month */
        days -= mon_lengths[yleap][res->tm_mon];
    }

    /* the res->tm_mon (month) is set on the previous loop */

    /* set the day of the month */
    res->tm_mday = days + 1;

    /* daylight saving time is always zero */
    res->tm_isdst = 0;

    /* return the result */
    return res;
}

/**
 *  @}
 */
