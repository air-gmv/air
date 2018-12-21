#include <math.h>
#include <time.h>

#include <date.h>
#include <Defs_nav.h>
//#include <trace.h>

//*****************************************************************************
//                        DEFINITIONS
//*****************************************************************************
#define FIRST_GAL_PRN_SI         (62)  // First Galileo PRN (internal GSTB-V2 convention)

#define GL_SECS_IN_MIN_SI         60   // Number of seconds in one minute.
#define GL_SECS_IN_DAY_SI      86400   // Number of seconds in one day.
#define GL_SECS_IN_WEEK_SI    604800   // Number of seconds in one week.
#define GL_MINS_IN_HOUR_SI        60   // Number of minutes in one hour.
#define GL_HOURS_IN_DAY_SI        24   // Number of hours in one day.
#define GL_MONTHS_IN_YEAR_SI      12   // Number of months in one year.
#define GL_DAYS_IN_YEAR_SI       365   // Number of days in a not leap year.
#define GL_DAYS_IN_LEAP_YEAR_SI  366   // Number of days in a leap year.

#define CONV_DAYS_IN_WEEK_SI     (7)   // Number of days in one week

// GPS date.
#define CONV_YEAR_1980_SI (1980)

#define GL_FEBRUARY_SI                  2            // February month index.
#define GL_GPS_TIME_START_JUL_DAYS_LD   2444244.5L

#define GL_SUCCESS_T                0      // GPS time of week is out of range.
#define GL_CAL_GPS_TOW_OUT_RANGE_T  2
#define GL_CAL_WRONG_DATE_T         3      // A calendar date is wrong.
#define GL_CAL_WRONG_JDFORMAT_T     4      // A Julian date format has an invalid value.

// Zero epoch of MJD (Modified Julian Date) related to JD (Julian Date)
#define GL_JD2MJD_LD       2400000.5L

// Zero epoch of MJD1950 related to JD (Julian Date), see #GL_JDFormat_t
#define GL_JD2MJD1950_LD   2433282.5L

// Zero epoch of MJD2000 related to JD (Julian Date), see #GL_JDFormat_t
#define GL_JD2MJD2000_LD   2451544.5L

//*****************************************************************************
//                        STRUCTURES DEFINITION
//*****************************************************************************
// Number of days in each month.
static const int GL_DAYS_IN_MONTH_SI[ GL_MONTHS_IN_YEAR_SI + 1 ] =
                 { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// Julian date format.
// Julian dates (abbreviated JD) are a continuous count of days and fractions
// since noon Universal Time on January 1, 4713 BCE (on the Julian calendar).
// The modified Julian day number MJD is defined as MJD = JD - 2,400,000.5.
// Thus MJD 0 designates the November 17th, 1858, 00:00:00.
// MJD1950 is defined as MJD1950 = JD - 2433282.5.
// Thus MJD1950 0 designates the January 1, 1950, 00:00:00.
// MJD2000 is defined as MJD2000 = JD - 2451544.5.
// Thus MJD2000 0 designates the January 1, 2000, 00:00:00.
typedef enum
{
   GL_JD_E,            // < Julian date.
   GL_MJD_E,           // < Modified Julian date.
   GL_MJD1950_E,       // < MJD1950 format.
   GL_MJD2000_E        // < MJD2000 format.
} GL_JDFormat_t;

//*****************************************************************************
//                     INTERNAL FUNCTION HEADERS
//*****************************************************************************
int      GL_GPSToJD( int week, double tow, GL_JDFormat_t OutFormat, long double * OutJulian);
int      GL_JDToCal( long double julian, GL_JDFormat_t InFormat, int *year, int *month, int *day,
                     int *hour, int *min, double *sec );
int      GL_LeapYear( int year );
int      convertYearDayToNDays( int year, int dayOfYear);
static void GL_integral_div( int num, int den, int * quot, int * rem );
int      GL_GPSToAGPS( int week,   double tow, long double *aGPStime);
int      GL_ChJDFormat( long double InJulian, GL_JDFormat_t InFormat, GL_JDFormat_t OutFormat,
                        long double * OutJulian );
int      GL_CalToJD( int year, int month, int day, int hour, int min, double sec, GL_JDFormat_t OutFormat,
                     long double * OutJulian );

//*****************************************************************************
//                        PUBLIC FUNCTION BODIES
//*****************************************************************************
/**
 * It converts GPS time (week and time of week) to calendar date.
 *
 * @warning If input time of week is out of range (0..GL_SECS_IN_WEEK_SI),
 *  computations are performed with corrected GPS time (week and tow)
 *  and a warning is returned.
 *
 * @param week (I) GPS week
 * @param tow (I) GPS time of week (seconds)
 * @param year (O) Year in calendar date
 * @param month (O) Month in calendar date
 * @param day (O) Day of month in calendar date
 * @param hour (O) Hour of day
 * @param minute (O) Minute of hour
 * @param second (O) Seconds as a double value
 *
 * @return Error_code:
 * - #GL_SUCCESS_T: Successful execution
 * - #GL_CAL_GPS_TOW_OUT_RANGE_T: Input variable tow is out of range
 *                                (0..GL_SECS_IN_WEEK_SI)
 */
int   GL_GPSToCal( int week, double tow, int *year, int *month, int *day,
                              int *hour, int *minute, double *second)
{
  /*
   * This function converts a GPS time to calendar date,
   * using Julian date as an intermediate format.
   */
  int returnVal = EXIT_OK;               /* Default return value */

  /* Julian date */
  long double julian;

  /* Julian date format, MJD is used to minimise loss of precission */
  GL_JDFormat_t julianFormat = GL_MJD_E;

  /* Convert GPS time to Julian date */
  returnVal = GL_GPSToJD( week, tow, julianFormat, & julian);

  /*
   * Convert to calendar date
   * Return error code is ignored because it is known to be GL_SUCCESS_T
   */
  (void) GL_JDToCal( julian, julianFormat, year, month, day, hour, minute, second);

  return (returnVal);
}

//*****************************************************************************
//   Name:         convertDateToWN
//   Purpose:      Get week number from calendar time format.
//   Arguments:
//      I:         NONE.
//      I/0:       NONE.
//      O:         NONE.
//      Global:      NONE
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2009/10/01   -   JJNC   -   Created.
//*****************************************************************************
void convertDateToWN( struct tm *gmtTime, int *wn)
{
   int nDays=0;         // # days since GPS time.

   // Get # days.
   nDays = convertYearDayToNDays( gmtTime->tm_year, gmtTime->tm_yday);

   // Compute week number.
   (*wn) = nDays / CONV_DAYS_IN_WEEK_SI;
}

//*****************************************************************************
//   Name:         convertDateToTOW
//   Purpose:      Get time of week from calendar time format.
//   Arguments:
//      I:         NONE.
//      I/0:       NONE.
//      O:         NONE.
//      Global:      NONE
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2009/10/01   -   JJNC   -   Created.
//*****************************************************************************
void convertDateToTOW( struct tm *gmtTime, double *tow)
{
   int nDays  = 0;         // # days since GPS time.

   // Get # days.
   nDays = convertYearDayToNDays( gmtTime->tm_year, gmtTime->tm_yday);

   // Compute week number.
   gmtTime->tm_wday = nDays % CONV_DAYS_IN_WEEK_SI;


   // Get # hours.
   (*tow) = gmtTime->tm_wday*GL_HOURS_IN_DAY_SI;

   // Get # minutes.
   (*tow) = ((*tow) + gmtTime->tm_hour)*GL_MINS_IN_HOUR_SI;

   // Get # seconds.
   (*tow) = ((*tow) + gmtTime->tm_min)*GL_SECS_IN_MIN_SI;
   (*tow) = (*tow) + gmtTime->tm_sec;
}

/**
 * It checks a calendar date, and corrects it if necessary.
 *
 * @param year (I/O) Year in calendar date
 * @param month (I/O) Month in calendar date
 * @param day (I/O) Day of month in calendar date
 * @param hour (I/O) Hour of day
 * @param min (I/O) Minute of hour
 * @param sec (I/O) Seconds as a double value
 *
 * @return Error_code:
 * - #GL_SUCCESS_T: Successful execution
 * - #GL_CAL_WRONG_DATE_T: Input calnedar date is not correct,
 *                         and it has been corrected
 */
int GL_CheckDate( int *year, int *month, int *day, int *hour, int *min, double *sec )
{
  /*
   * First, the seconds are checked to be in range.
   * If not, they are corrected and minutes are update accordingly.
   * Then minutes and hours are also checked.
   * Later, months are also checked.
   *
   * Finally, days must be checked. If they are negative, the number of days
   * of previous month is added and month is decrement in one.
   * It is iterated until days are positive.
   * If days are greater than the number of days in the month, they are
   * subtracted this number, and month is incremented.
   * It is also iterated until number of days is corect.
   *
   * The default return value is GL_SUCCESS_T.
   * If a correction is needed, it is set to GL_CAL_WRONG_DATE_T.
   */
   int returnVal = EXIT_OK;        /* Default return value */
   int intSec;                     /* Integral parts of seconds */
   double fracSec;                 /* Fractional  parts of seconds */
   int quot;                       /* Auxiliar quotient of integral division */
   int rem;                        /* Auxiliar reminder of integral division */
   int daysInMonth;                /* Number of days in month */

   /*
    * Check seconds ( 0 <= *sec < 60)
    * Only the integral part is checked.
    */

   /* Compute integral and fractional parts of seconds,
    * fractional one must be positive */
   intSec = (int) floor( *sec );
   fracSec = *sec - (double) intSec;

   if ( ( intSec < 0 ) || ( intSec >= GL_SECS_IN_MIN_SI ) )
   {
      returnVal = GL_CAL_WRONG_DATE_T; /* Update return error code */

      /* Compute integral division */
      GL_integral_div( intSec, GL_SECS_IN_MIN_SI, & quot, & rem );

      /* Update data */
      *min += quot;
      *sec = (double) rem + fracSec;
   }

   /*
    * Check minutes ( 0 <= *min < 60)
    */
   if ( ( *min < 0 ) || ( *min >= GL_MINS_IN_HOUR_SI ) )
   {
      returnVal = GL_CAL_WRONG_DATE_T; /* Update return error code */

      /* Compute integral division */
      GL_integral_div( *min, GL_MINS_IN_HOUR_SI, & quot, & rem );

      /* Update data */
      *hour += quot;
      *min = rem;
   }

   /*
    * Check hours ( 0 <= *hour < 24)
     */
   if ( ( *hour < 0 ) || ( *hour >= GL_HOURS_IN_DAY_SI ) )
   {
      returnVal = GL_CAL_WRONG_DATE_T; /* Update return error code */

      /* Compute integral division */
      GL_integral_div( *hour, GL_HOURS_IN_DAY_SI, & quot, & rem );

      /* Update data */
      *day += quot;
      *hour = rem;
   }

   /*
     * Check month ( 0 < month <= 12 )
     * Note
     */
   if ( ( *month <= 0 ) || ( *month > GL_MONTHS_IN_YEAR_SI ) )
   {
      returnVal = GL_CAL_WRONG_DATE_T; /* Update return error code */

      /* Compute integral division */
      GL_integral_div( *month - 1, GL_MONTHS_IN_YEAR_SI, & quot, & rem );

      /* Update data */
      *year += quot;
      *month = rem + 1;
   }

   /*
     * Finally, day is checked, depending on the month and the year
     * A while loop is used, because days in moth are not constant.
     */
    /* Correct day if it is negative */
   while ( *day <= 0 )
   {
      returnVal = GL_CAL_WRONG_DATE_T; /* Update return error code */

      /* Get days in previous month, checking if year is leap */
      if ( ( *month - 1 == GL_FEBRUARY_SI ) &&
         ( GL_LeapYear( *year ) == TRUE ) )
      {
         daysInMonth = GL_DAYS_IN_MONTH_SI[ *month - 1 ] + 1;
      }
      else if ( *month == 1 )
      {
         daysInMonth = GL_DAYS_IN_MONTH_SI[ GL_MONTHS_IN_YEAR_SI ];
      }
      else
      {
         daysInMonth = GL_DAYS_IN_MONTH_SI[ *month - 1 ];
      }

       /* Add number of days in previous month and remove 1 month */
       *day += daysInMonth;
      *month -= 1;

      /* Check if start of year has been reached */
      if ( *month == 0 )
      {
         *month = GL_MONTHS_IN_YEAR_SI;
         *year -= 1;
      }
   }

   /* Correct day if it is larger than allowed */
    /* Get days in month, checking if year is leap */
   if ( ( *month == GL_FEBRUARY_SI ) &&
       ( GL_LeapYear( *year ) == TRUE ) )
   {
      daysInMonth = GL_DAYS_IN_MONTH_SI[ *month ] + 1;
   }
   else
   {
      daysInMonth = GL_DAYS_IN_MONTH_SI[ *month ];
   }

   while ( *day > daysInMonth )
   {
      returnVal = GL_CAL_WRONG_DATE_T; /* Update return error code */

      /* Remove number of days in month and add 1 month */
      *day -= daysInMonth;
      *month += 1;

      /* Check if end of year has been reached */
      if ( *month > GL_MONTHS_IN_YEAR_SI )
      {
         *month = 1;
         *year += 1;
      }

      /* Get days in month, checking if year is leap */
      if ( ( *month == GL_FEBRUARY_SI ) &&
         ( GL_LeapYear( *year ) == TRUE ) )
      {
         daysInMonth = GL_DAYS_IN_MONTH_SI[ *month ] + 1;
      }
      else
      {
         daysInMonth = GL_DAYS_IN_MONTH_SI[ *month ];
      }
   }

   return returnVal;
}

//*****************************************************************************
//                  INTERNAL FUNCTIONS BODIES
//*****************************************************************************
//*****************************************************************************
//   Name:         getTimeStr
//   Purpose:      This function gets the current epoch of execution.
//   Arguments:
//      I:        year            Current year.
//                dayOfYear       Day in current year.
//      I/0:      NONE.
//      O:        # days since 1 January
//      Global:      NONE.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2009/09/28   -   JJNC   -   Created.
//*****************************************************************************
int convertYearDayToNDays( int year, int dayOfYear)
{
   int nDays=0;         // Function return value.
   int i=0;             // Loop index.
   int isLeap=FALSE;    // Leap year flag.

   nDays = 0;
   for (i=CONV_YEAR_1980_SI; i<year; i++)
   {
      // Get leap year flag.
      isLeap = GL_LeapYear(i);

      // Leap year -> Add 366.
      if (isLeap)
      {
         nDays = nDays + GL_DAYS_IN_YEAR_SI + 1;
      }
      // Normal year -> Add 365.
      else
      {
         nDays = nDays + GL_DAYS_IN_YEAR_SI;
      }
    }

   // Add # days.
   nDays = nDays + dayOfYear - 5;

   return(nDays);
}

//*****************************************************************************
//   Name:         getTimeStr
//   Purpose:      This function gets the current epoch of execution.
//   Arguments:
//      I:         pchTimeStr            System time in the DD-month-YY HH:MM format.
//      I/0:       NONE.
//      O:         0 if succes
//                 -1 e.o.c.
//      Global:      NONE.
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/04/21   -   JJNC   -   Created.
//*****************************************************************************
int getTimeStr(char *pchTimeStr)
{
   int    ret = EXIT_OK;                  // Function return value.
   time_t stThisTime=0;                   // Standard time in seconds.
   struct tm *timeStruct;                 // Struct information time.

   // Get actual time (in seconds since 1/1/1971).
   if (time(&stThisTime) == (time_t)-1)
   {
      ret = EXIT_ER;
   }
   else
   {
      // Format time.
      timeStruct = localtime(&stThisTime);

      // Format time.
      if (strftime(pchTimeStr, 20, "%d-%b-%y %H:%M", timeStruct) == 0)
      {
         // Error formatting time.
         ret = EXIT_ER;
      }
   }

   return (ret);
}

//*********************************************************************************
//   Function:      dateToDay
//   Arguments:
//      I:       year               current year.
//               month              current month.
//               day                current day.
//      I/0:     NONE.
//      O:       NONE.
//      Global:      NONE
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/05/14   -   JJNC   -   Created.
//*********************************************************************************
int dateToDay(int year, int month, int day)
{
   int i=0;                     // Loop index.
   int yday=day;               // Return value.
   int monthDays[]={0,31,28,31,30,31,30,31,31,30,31,30,31};// Days per month.

   // Accumulate days per month.
   for(i=0; i<month; i++)
   {
      yday += monthDays[i];
   }

   // Increase day if leap year and current month february.
   if((leapYear(year) == TRUE) && (month > 2))
   {
      yday++;
   }

   return yday;
}

//*********************************************************************************
//   Function:      leapYear
//   Arguments:
//      I:         year               current year.
//      I/0:       NONE.
//      O:         TRUE if year is leap.
//                 FALSE i.o.c.
//      Global:      NONE
//   Files I/O:      NONE.
//   Other I/O:      NONE.
//
//   History:
//  1.0   -   2008/05/14   -   JJNC   -   Created.
//*********************************************************************************
int leapYear(int year)
{
   int leap = FALSE;                  // Function return value.

   // Check leap year conditions.
   if(((year%4 == 0) && (year%100 != 0)) || (year%400 == 0))
   {
      // It is leap year.
      leap= TRUE;
   }

   return leap;
}

/**
 * It converts GPS time (week and time of week) to a Julian date.
 *
 * @sa GL_JDFormat_t
 *
 * @warning If input time of week is out of range (0..GL_SECS_IN_WEEK_SI),
 *  computations are performed with corrected GPS time (week and tow)
 *  and a warning is returned.
 *
 * @param week (I) GPS week
 * @param tow (I) GPS time of week (seconds)
 * @param OutFormat (I) Julian date format of output date
 * @param OutJulian (O) Output julian date with format defined by @c OutFormat
 *
 * @return Error_code:
 * - #GL_SUCCESS_T: Successful execution
 * - #GL_CAL_WRONG_JDFORMAT_T: Variable OutFormat has an invalid value
 * - #GL_CAL_GPS_TOW_OUT_RANGE_T: Input variable tow is out of range
 *                                (0..GL_SECS_IN_WEEK_SI)
 */
int GL_GPSToJD( int week, double tow, GL_JDFormat_t OutFormat, long double * OutJulian)
{
  /*
   * GPS time is first converted to absolute GPS time,
   * it is converted to Julian date and finally
   * it is converted to the required format of Julian date.
   */

  /* Errro code */
  int returnError;

  /* Absolute GPS time */
  long double aGPSTime;

  /* Convert to absolute GPS time */
  returnError = GL_GPSToAGPS( week, tow, & aGPSTime );

  /* Convert absolute GPS time to Julian date */
  *OutJulian = aGPSTime / (long double) GL_SECS_IN_DAY_SI +
    GL_GPS_TIME_START_JUL_DAYS_LD;

  /* Change format of Julian date */
  if ( GL_ChJDFormat( *OutJulian, GL_JD_E, OutFormat, OutJulian ) !=
       GL_SUCCESS_T ) {
    returnError = GL_CAL_WRONG_JDFORMAT_T;
  }

  /* Return the error code from GL_GPSToAGPS function */
  return returnError;
}

//*********************************************************************************
int GL_JDToCal( long double julian, GL_JDFormat_t InFormat, int *year, int *month, int *day,
                                       int *hour, int *min, double *sec )
{
  /*
   * This functions, first converts the input Julian day to MJD2000 format.
   *
   * Afterwards, an approximate year is computed dividing the Julian date
   * by the approximate number of days in a year (365.25).
   *
   * Finally, the date given by approximate year, remining days, and
   * the fractional part of day as seconds is corrected using GL_CheckDate.
   *
   * Operations are performed with long double precission,
   * and later casted to the desired type.
   */

  /* Retuirn error code */
  int returnError = GL_SUCCESS_T;

  /* Auxiliar Julian day */
  long double auxJulian;

  /* Reminder days as a long double value */
  long double remDays;

  /* Cahnge Julian date to MJD2000 */
  returnError = GL_ChJDFormat( julian, InFormat, GL_MJD2000_E, &julian );

  /* Compute approximate year */
  *year = 2000 + (int) floor( (double) ( julian / 365.25L ) );

  /*
   * Compute Julian day of January, 1st, 00:00:00 of the approximate year.
   * Returned error code is ignored because it is known to be correct.
   */
  (void) GL_CalToJD( *year, 1, 1, 0, 0, 0.0, GL_MJD2000_E, & auxJulian );

  /*
   * Compute reminder days as difference of input Julian date and
   * Julian date correspondign to the approximate year.
   */
  remDays = julian - auxJulian;
  *day = (int) floor( (double) remDays );

  /* Compute the reminder fractional part */
  *sec = (double)
    ( ( remDays - (long double) *day ) *
      (long double) GL_SECS_IN_DAY_SI );

  /*
   * Finally, correct calendar date.
   * Output error code is ignored, because date is known to be incorrect,
   * the objective of this call is to correct it.
   *
   * Year is added 2000, because Julian date is referred to MJD2000.
   * Month is initialised to January (1).
   * Day is added 1, because first day of year is day 0 in julian format.
   * Hour and minute are initialised to 0.
   */
  *month = 1;
  *day += 1;
  *hour = 0;
  *min = 0;

  (void) GL_CheckDate( year, month, day, hour, min, sec );

  return returnError;
}

/**
 * It computes the integral division.
 *
 * Outputs are the quotient and the reminder.
 * The quotient is the largest integer lower than <tt> num / den </tt>
 * The reminder is always positive and such that:
 * <tt> num = dem * quot + rem </tt>
 *
 * @warning
 * If denominator is zero, a mathematical exception will occur.
 * It is not checked because it is an internal function, and denominator
 * is known to be different from 0.
 *
 * @param num (I) Numerator
 * @param den (I) Denominator
 * @param quot (O) Quotient
 * @param rem (O) Reminder
 *
 * @return void
 */
static void GL_integral_div( int num, int den, int * quot, int * rem )
{
   /* Compute integral division */
   *quot = num / den;
   *rem = num % den;

   /* Check reminder to be positive */
   if ( *rem < 0 )
   {
      *rem += den;
      *quot -= 1;
   }

   return;
}

/**
 * It converts GPS time (week and time of week) to absolute GPS time
 * (number of days from start).
 *
 * @warning If input time of week is out of range (0..GL_SECS_IN_WEEK_SI),
 *  computations are performed and a warning is returned.
 *
 * @param week (I) GPS week
 * @param tow (I) GPS time of week (seconds)
 * @param aGPStime (O) Absolute GPS time (seconds)
 *
 * @return Error_code:
 * - #GL_SUCCESS_T: Successful execution
 * - #GL_CAL_GPS_TOW_OUT_RANGE_T: Input variable tow is out of range
 *                                (0..GL_SECS_IN_WEEK_SI)
 */
int GL_GPSToAGPS( int week,   double tow, long double *aGPStime )
{
   /*
   * Absolute GPS time is the GPS week multiplied
   * by the number of seconds in a week plus the time of week.
   */
     /* Return error code */
   int returnError = GL_SUCCESS_T;

   /*
   * Check GPS time of week.
   */
   if ( ( tow < 0.0 ) || ( ! ( tow < (double) GL_SECS_IN_WEEK_SI ) ) )
   {
      returnError = GL_CAL_GPS_TOW_OUT_RANGE_T;
   }

   /*
    * Compute output.
    * Operations are performed with long double type
    * to minimise the loose of precission
   */
   *aGPStime = (long double) week * (long double) GL_SECS_IN_WEEK_SI +
               (long double) tow;

   return returnError;
}

/**
 * It converts a calendar date to a Julian one.
 *
 * @sa GL_JDFormat_t
 *
 * @warning If input calendar date is not correct,
 *  computations are performed with corrected date and a warning is returned.
 *
 * @param year (I) Year in calendar date
 * @param month (I) Month in calendar date
 * @param day (I) Day of month in calendar date
 * @param hour (I) Hour of day
 * @param min (I) Minute of hour
 * @param sec (I) Seconds as a double value
 * @param OutFormat (I) Julian date format of output date
 * @param OutJulian (O) Output julian date with format defined by @c OutFormat
 *
 * @return Error_code:
 * - #GL_SUCCESS_T: Successful execution
 * - #GL_CAL_WRONG_JDFORMAT_T: Variable OutFormat has an invalid value
 * - #GL_CAL_WRONG_DATE_T: Calendar date is not correct
 */
int GL_CalToJD( int year, int month, int day, int hour, int min, double sec,
                        GL_JDFormat_t OutFormat, long double * OutJulian )
{
  /*
   * The calendar date is converted to MJD2000, that is,
   * days since January 1, 2000, 00:00:00.
   *
   * First, the 400 year cycles since year 2000 are computed, and the reminder
   * years in current cycle. Note that a negative number of cycles is allowed.
   * In a 400 year cycle there are 97 leap years, and 303 no leap ones.
   * The reminder number of years in a cycle is between 0 and 399.
   *
   * Furthermore, the number of days since year 0 to previous one are computed,
   * taking into account number of leap years (one every four years, excluding
   * years 100, 200 and 300).
   *
   * Finnally, the day of year corresponding to current calendar date is added,
   * plus the fractional part of current day.
   */

  /* Return error code */
  int returnError;

  /* Number of 400 cycles */
  int num400YearCycle;

  /* Reminder number of years in a 400 year cycle */
  int remYears;

  /* Day of year of calendar date */
  int dayOfYear;

  /* First, input calendar date is checked and corrected */
  returnError = GL_CheckDate( & year, & month, & day,
               & hour, & min, & sec );

  /* Compute number of 400 year cycles */
  GL_integral_div( year - 2000, 400, & num400YearCycle, & remYears );

  /*
   * Compute number of days in the 400 year cycles,
   * Computations are done with long double, to avoid loss of precission.
   */
  *OutJulian =
    (long double) num400YearCycle *
    ( 303.0L * (long double) GL_DAYS_IN_YEAR_SI +
       97.0L * (long double) GL_DAYS_IN_LEAP_YEAR_SI );

  /*
   * Compute number of days since year 0 to previous one, taking into account
   * number of leap years (one every four years, excluding years 100, 200 and
   * 300).
   */
  if ( remYears > 0 ) {
    *OutJulian +=
      (long double) GL_DAYS_IN_YEAR_SI *  /*Number of days in a non leap year*/
      (long double) remYears +            /*multiplied by number of years */
      (long double) ( (remYears-1) / 4 )+ /*plus one leap every four years */
      1.0L;                               /*plus one, because year 0 is leap */
  }

  /* Remove extra days, because years 100, 200 nad 300 are not leap ones */
  if ( remYears > 100 ) {
    *OutJulian -= 1.0L;
  }
  if ( remYears > 200 ) {
    *OutJulian -= 1.0L;
  }
  if ( remYears > 300 ) {
    *OutJulian -= 1.0L;
  }

  /*
   * Compute day of year of current calendar date
   * Output error code is ignored because date is known to be correct,
   * it was corrected previously by GL_CheckDate function.
   */
  (void) GL_DateToDay( remYears, month, day, & dayOfYear );

  /* First day of year is day 0 in Julian format, so 1 is subtracted */
  *OutJulian += (long double) ( dayOfYear - 1 );

  /* Add fractional part of day */
  *OutJulian +=
    (long double) sec / (long double) GL_SECS_IN_DAY_SI +
    (long double) min / (long double) ( GL_MINS_IN_HOUR_SI*GL_HOURS_IN_DAY_SI)+
    (long double) hour / (long double) GL_HOURS_IN_DAY_SI;

  /* Finally, convert MJD2000 to output format of Julian date */
  if ( GL_ChJDFormat( *OutJulian, GL_MJD2000_E, OutFormat, OutJulian) !=
       GL_SUCCESS_T ) {
    returnError = GL_CAL_WRONG_JDFORMAT_T;
  }

  return returnError;
}

/**
 * It computes the date of year corresponding to an input calendar date.
 *
 * @warning If input calendar date is not correct,
 *  computations are performed with corrected date and a warning is returned.
 *
 * @param year (I) Year in calendar date (needed to know if it is leap)
 * @param month (I) Month in calendar date
 * @param day (I) Day of month in calendar date
 * @param yday (O) Output day of year
 *
 * @return Error_code:
 * - #GL_SUCCESS_T: Successful execution
 * - #GL_CAL_WRONG_DATE_T: Calendar date is not correct
 */
int GL_DateToDay( int year, int month, int day, int * yday )
{
  /*
   * It computes the date of year corresponding to an input calendar date.
   * Date is checked using GL_CheckDate. The corrected date is used.
   * The error code returned by GL_CheckDate is the final error code.
   */
  /* Returned error code */
  int returnVal;

  /* Index of for loop */
  int i;

  /* Variables needed by GL_CheckDate function,
   * they are meaningless to current function */
  int hour = 0;
  int min = 0;
  double sec = 0.0;

  /*
   * Check date.
   * A dummy value is used for hour, minute and second.
   */
  returnVal = GL_CheckDate( &year, &month, &day, &hour, &min, &sec );

  /* Initialise output */
  *yday = 0;

  /* Accumulate days until previous month */
  for( i = 1; i < month; i++ ) {
    *yday += GL_DAYS_IN_MONTH_SI[i];
  }

  /* Add one day for leap years */
  if ( ( month > GL_FEBRUARY_SI ) &&
       ( GL_LeapYear( year ) == TRUE ) ) {

    *yday += 1;
  }

  /* Finally add input day in current month */
  *yday += day;

  return returnVal;
}

/**
 * Check is a year is leap or not.
 * Leap years are those multiple of 4, but not multiple of 100,
 * and those multiple of 400.
 *
 * @param year (I) Year
 *
 * @return #GL_TRUE_E if year is leap, #GL_FALSE_E otherwise
 */
int GL_LeapYear( int year )
{
   /* Return value */
   int returnVal;

   /* Check if year is leap */
   if ( ( ( year % 4 == 0 ) && ( year % 100 != 0 ) ) ||
       ( year % 400 == 0 ) )
   {
      // Leap year.
      returnVal = TRUE;
   }
   // Not leap year.
   else
   {
      returnVal = FALSE;
   }

   return returnVal;
}

/**
 * It changes a date between different formats of Julian dates.
 *
 * @sa GL_JDFormat_t
 *
 * @param InJulian (I) Input julian date with format defined by @c InFormat
 * @param InFormat (I) Julian date format of input date
 * @param OutFormat (I) Julian date format of output date
 * @param OutJulian (O) Output julian date with format defined by @c OutFormat
 *                      @n It is allowed to be a pointer to the input parameter
 *                      InJulian, which will be overwritten with output value.
 *
 * @return Error_code:
 * - #GL_SUCCESS_T: Successful execution
 * - #GL_CAL_WRONG_JDFORMAT_T: Variables InFormat or OutFormat has wrong values
 */
int GL_ChJDFormat( long double InJulian, GL_JDFormat_t InFormat, GL_JDFormat_t OutFormat,
                                                      long double * OutJulian)
{
  /*
   * This functions changes a date between diferent Julian date formats.
   *
   * Two switch statements are used to determine the input and output format.
   * If any of the formats is invalid, an error is returned.
   *
   * First switch changes the input date to JD format.
   * Second one, changes from JD format to output format.
   */

   int         ret=EXIT_OK;            // Function return value.
   long double TempJulian=0;           // Date in JD format.

   // Convert input date to JD format.
   switch( InFormat )
   {
      case GL_JD_E:
      {
         TempJulian = InJulian;
         break;
      }
      case GL_MJD_E:
      {
         TempJulian = InJulian + GL_JD2MJD_LD;
         break;
      }
      case GL_MJD1950_E:
      {
         TempJulian = InJulian + GL_JD2MJD1950_LD;
         break;
      }
      case GL_MJD2000_E:
      {
         TempJulian = InJulian + GL_JD2MJD2000_LD;
         break;
      }
      default:
      {
         ret =  GL_CAL_WRONG_JDFORMAT_T;
      }
   }

   if (ret == EXIT_OK)
   {
      /* Convert date to output format */
      switch( OutFormat )
      {
         case GL_JD_E:
         {
         *OutJulian = TempJulian;
         break;
         }
         case GL_MJD_E:
         {
            *OutJulian = TempJulian - GL_JD2MJD_LD;
            break;
         }
         case GL_MJD1950_E:
         {
            *OutJulian = TempJulian - GL_JD2MJD1950_LD;
            break;
         }
         case GL_MJD2000_E:
         {
            *OutJulian = TempJulian - GL_JD2MJD2000_LD;
            break;
         }
         default:
         {
            ret = GL_CAL_WRONG_JDFORMAT_T;
         }
      }
   }

   /* At this point both formats are correct, GL_SUCCESS_T is returned */
   return ret;
}

////============================================================================
// FUNCTION    : julianDay
// CLASS NAME  :
// DESCRIPTION : This function compute the julian day
//
// INPUTS      : - year
//               - month
//               - floatday
// RETURN      : - double: julian day
//
////============================================================================
//  HISTORY
////============================================================================
// DATE          VERSION      AUTHOR         REMARKS
//
// 2003           v2.0        MMMJ       Adapted to SKAME from KINEMATIC
////============================================================================
double julianDay( int year,
              int month,
              double floatday){

    const  long IGREG = (15+31L*(10+12L*1582));
    double jul = 0;
    int    jm  = 0,
           jy  = year,
           ja  = 0;

    if (jy < 0)
    {
       ++jy;
    }
    if (month > 2)
    {
       jy=year;
       jm=month+1;
    }
    else
    {
       jy=year-1;
       jm=month+13;
    }

    jul = floor(365.25*jy) + floor(30.6001*jm) + floatday + 1720994.5;

    if (floatday+31L*(month+12L*year) >= IGREG)
    {
       ja=(int)(0.01*jy);
       jul += 2-ja+(int)(0.25*ja);
    }

    return jul;

} // julianDay

////============================================================================
// FUNCTION    : calendarToGps
// CLASS NAME  :
// DESCRIPTION : This function convert calendar to gps time of the week
//
// INPUTS      : - year
//               - month
//               - day
//               - hour
//               - min
//               - sec
// OUTPUTS     : - week
//               - tow
//
////============================================================================
//  HISTORY
////============================================================================
// DATE          VERSION      AUTHOR         REMARKS
//
// 2003           v2.0        MMMJ       Adapted to SKAME from KINEMATIC
////============================================================================
void calendarToGps( int year, int month, int day, int hour, int min, double sec, long *wn, double *tow){

    double jul      = 0,
           floatday = 0;

     floatday = 1.0*day+1.0*hour/24.0;

    jul = julianDay (year, month, floatday);
    *wn = (long)((jul - 2444244.5) / 7.0);
    *tow =((int)(jul-2444244.5)-(*wn)*7.0)*24*3600.0+hour*3600.0+min*60.0+sec;
} // calendarToGps

/******************************************************************

  void TimeOfDay2cal(double ToD, int *hour, int *min, double *sec)

 ******************************************************************/
void TimeOfDay2cal(double ToD, int *hour, int *min, double *sec)
{
	double aux = 0;

	*hour = (int)(ToD/3600.0);
	aux = ToD - 3600*(*hour);

	*min = (int)(aux/60.0);

	*sec = aux - 60*(*min);
}

/****************************************************************************

  int GetMonthNum(char *szMonth)

 ****************************************************************************/
int GetMonthNum(char *szMonth)
{
   int siMonthNum = 0;
   int i;
   unsigned int uiMinMonthStrlen = 3;
   char pszMonths[12][4] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

   if (strlen(szMonth) >= uiMinMonthStrlen)
   {
      for (i=0; i<12; i++)
      {
#ifdef LINUX
    	  if (strncasecmp(szMonth,pszMonths[i],3) == 0)
#else
         if (strnicmp(szMonth,pszMonths[i],3) == 0)
#endif
         {
            siMonthNum = i + 1;
         }
      }
   }

   return siMonthNum;
}
