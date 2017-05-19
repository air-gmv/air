/**
 *  @file
 *  watchdog.inl
 *
 *  @brief contains the static inline implementation of all inlined
 *  routines in the Watchdog Handler.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  4546    | 28/09/2009  | mcoutinho    | IPR 778
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreWatchdog Watchdog Handler
 *  @{
 */

#ifndef _RTEMS_SCORE_WATCHDOG_INL
#define _RTEMS_SCORE_WATCHDOG_INL

#include <rtems/system.h>


/**
 *  @brief initialize a watchdog
 *  
 *  This routine initializes the specified watchdog.  The watchdog is
 *  made inactive, the watchdog id and handler routine are set to the
 *  specified values.
 *
 *  @param[in] the_watchdog the watchdog to initialize
 *  @param[in] routine the watchdog routine
 *  @param[in] id the watchdog id
 *  @param[in] user_data the user data to invoke the routine with
 *
 *
 */
RTEMS_INLINE_ROUTINE void _Watchdog_Initialize(
                                               Watchdog_Control *the_watchdog ,
                                               Watchdog_Service_routine_entry routine ,
                                               Objects_Id id ,
                                               void *user_data
                                               )
{
   /* initialize the watchdog fields */

   /* the initial state is inactive */
   the_watchdog->state = WATCHDOG_INACTIVE;

   /* set the routine entry point */
   the_watchdog->routine = routine;

   /* set the object identifier */
   the_watchdog->id = id;

   /* set the argument to call the routine with */
   the_watchdog->user_data = user_data;
}


/**
 *  @brief determine if a watchdog is active
 *  
 *  This routine returns TRUE if the watchdog timer is in the ACTIVE
 *  state, and FALSE otherwise.
 *
 *  @param[in] the_watchdog the watchdog to check if it is active
 *
 *  @return returns TRUE if the watchdog is active and FALSE otherwise
 */
RTEMS_INLINE_ROUTINE boolean _Watchdog_Is_active(
                                                 Watchdog_Control *the_watchdog
                                                 )
{
   /* return ture if the watchdog state is active */
   return ( the_watchdog->state == WATCHDOG_ACTIVE );
}


/**
 *  @brief activate a watchdog
 *  
 *  This routine activates THE_WATCHDOG timer which is already
 *  on a watchdog chain.
 *
 *  @param[in] the_watchdog the watchdog to activate
 */
RTEMS_INLINE_ROUTINE void _Watchdog_Activate(
                                             Watchdog_Control *the_watchdog
                                             )
{
   /* activate the watchdog by changing its state */
   the_watchdog->state = WATCHDOG_ACTIVE;
}


/**
 *  @brief deactivate a watchdog
 *  
 *  This routine deactivates THE_WATCHDOG timer which will remain
 *  on a watchdog chain.
 *
 *  @param[in] the_watchdog the watchdog to deactivate
 */
RTEMS_INLINE_ROUTINE void _Watchdog_Deactivate(
                                               Watchdog_Control *the_watchdog
                                               )
{
   /* deactivate the watchdog by changing its state */
   the_watchdog->state = WATCHDOG_REMOVE_IT;
}


/**
 *  @brief announce the passage of a clock tick to the Watchdog Handler
 *  
 *  This routine is invoked at each clock tick to update the ticks
 *  watchdog chain.
 */
RTEMS_INLINE_ROUTINE void _Watchdog_Tickle_ticks(void)
{
   /* announce the passage of a clock tick to the watchdog ticks chain */
   _Watchdog_Tickle(&_Watchdog_Ticks_chain);
}


/**
 *  @brief announce the passage of a second to the Watchdog Handler
 *  
 *  This routine is invoked at each clock tick to update the seconds
 *  watchdog chain.
 */
RTEMS_INLINE_ROUTINE void _Watchdog_Tickle_seconds(void)
{
   /* announce the passage of a second to the watchdog seconds chain */
   _Watchdog_Tickle(&_Watchdog_Seconds_chain);
}


/**
 *  @brief insert a watchdog into the ticks watchdog chain
 *  
 *  This routine inserts THE_WATCHDOG into the ticks watchdog chain
 *  for a time of UNITS ticks.  The INSERT_MODE indicates whether
 *  THE_WATCHDOG is to be activated automatically or later, explicitly
 *  by the caller.
 *
 *  @param[in] the_watchdog the watchdog to insert
 *  @param[in] units the amount of clock ticks
 */
RTEMS_INLINE_ROUTINE void _Watchdog_Insert_ticks(
                                                 Watchdog_Control *the_watchdog ,
                                                 Watchdog_Interval units
                                                 )
{
   /* set the watchdog initial number of units */
   the_watchdog->initial = units;

   /* insert the watchdog on the watchdog clock ticks chain */
   _Watchdog_Insert(&_Watchdog_Ticks_chain , the_watchdog);
}


/**
 *  @brief insert a watchdog into the seconds watchdog chain
 *  
 *  This routine inserts THE_WATCHDOG into the seconds watchdog chain
 *  for a time of UNITS seconds.  The INSERT_MODE indicates whether
 *  THE_WATCHDOG is to be activated automatically or later, explicitly
 *  by the caller.
 *
 *  @param[in] the_watchdog the watchdog to insert
 *  @param[in] units the amount of seconds
 */
RTEMS_INLINE_ROUTINE void _Watchdog_Insert_seconds(
                                                   Watchdog_Control *the_watchdog ,
                                                   Watchdog_Interval units
                                                   )
{
   /* set the watchdog initial number of units */
   the_watchdog->initial = units;

   /* insert the watchdog on the watchdog seconds chain */
   _Watchdog_Insert(&_Watchdog_Seconds_chain , the_watchdog);
}


/**
 *  @brief adjust the watchdog seconds chain forward or backwards by a specified
 *  amount of units
 *  
 *  This routine adjusts the seconds watchdog chain in the forward
 *  or backward DIRECTION for UNITS seconds.  This is invoked when the
 *  current time of day is changed.
 *
 *  @param[in] direction the direction (forward or backward)
 *  @param[in] units the amount of units to adjust the chain
 */
RTEMS_INLINE_ROUTINE void _Watchdog_Adjust_seconds(
                                                   Watchdog_Adjust_directions direction ,
                                                   Watchdog_Interval units
                                                   )
{
   /* adjust the watchdog seconds chain */
   _Watchdog_Adjust(&_Watchdog_Seconds_chain , direction , units);
}

/**
 *  @brief adjust the watchdog ticks chain forward or backwards by a specified
 *  amount of units
 *  
 *  This routine adjusts the ticks watchdog chain in the forward
 *  direction for UNITS ticks (ticks are monotonic). This is invoked when there
 *  a partition context switch
 
 *  @param[in] units the amount of units to adjust the chain
 */
RTEMS_INLINE_ROUTINE void _Watchdog_Adjust_ticks(
                                                 Watchdog_Interval units
                                                 )
{
   /* adjust the watchdog seconds chain */
   _Watchdog_Adjust(&_Watchdog_Ticks_chain , WATCHDOG_FORWARD , units);
}


/**
 *  @brief determine the next watchdog
 *  
 *  This routine returns a pointer to the watchdog timer following
 *  THE_WATCHDOG on the watchdog chain.
 *
 *  @param[in] the_watchdog
 *
 *  @return returns the next watchdog on the chain
 */
RTEMS_INLINE_ROUTINE Watchdog_Control *_Watchdog_Next(
                                                      Watchdog_Control *the_watchdog
                                                      )
{
   /* get the next watchdog */
   return ( (Watchdog_Control *) the_watchdog->Node.next );
}


/**
 *  @brief get the first watchdog of a watchdog chain
 *  
 *  This routine returns a pointer to the first watchdog timer
 *  on the watchdog chain HEADER.
 *
 *  @param[in] header the chain header
 *
 *  @return returns the first watchdog on the chain
 */
RTEMS_INLINE_ROUTINE Watchdog_Control *_Watchdog_First(
                                                       Chain_Control *header
                                                       )
{
   /* get the first watchdog of the chain */
   return ( (Watchdog_Control *) header->first );
}

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
