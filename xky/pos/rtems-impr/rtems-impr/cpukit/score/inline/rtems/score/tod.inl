/**
 *  @file
 *  tod.inl
 *
 *  @brief contains the static inline implementation of the inlined routines
 *  from the Time of Day Handler.
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
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
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

#ifndef _RTEMS_SCORE_TOD_INL
#define _RTEMS_SCORE_TOD_INL

#include <time.h> /* struct timeval */
#include <rtems/score/isr.h>
#include <rtems/system.h>


/**
 *  @brief deactivate the TOD
 *  
 *  This routine deactivates updating of the current time of day.
 */
RTEMS_INLINE_ROUTINE void _TOD_Deactivate(void)
{
   /* @note do we need something now that we are using timespec for TOD */
}


/**
 *  @brief activate TOD
 *  
 *  This routine activates updating of the current time of day.
 */
RTEMS_INLINE_ROUTINE void _TOD_Activate(void)
{
   /*@note do we need something now that we are using timespec for TOD */
}


/**
 *  @brief get TOD timeval
 *  
 *  This routine returns a timeval based upon the internal timespec format TOD.
 *
 *  @param[out] time the address to place the current timeval
 */
RTEMS_INLINE_ROUTINE void _TOD_Get_timeval(
                                           struct timeval *time
                                           )
{
   /* interrupt level */
   ISR_Level level;

   /* current time */
   struct timespec now;


   /* enter critical section */
   _ISR_Disable(level);

   /* get current time */
   _TOD_Get(&now);

   /* leave critical section */
   _ISR_Enable(level);

   /* copy the timespec structure to a timeval structure */

   /* copy the seconds */
   time->tv_sec = now.tv_sec;

   /* copy the nanoseconds to microseconds */
   time->tv_usec = now.tv_nsec / TOD_NANOSECONDS_PER_MICROSECOND;
}

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
