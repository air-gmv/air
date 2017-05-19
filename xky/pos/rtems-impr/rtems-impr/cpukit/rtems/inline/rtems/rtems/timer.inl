/**
 *  @file
 *  timer.inl
 *
 *  @brief static inline implementation of the inlined routines
 *  from the Timer Manager.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-1999.
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
 *  5330    | 02/11/2009  | mcoutinho    | IPR 854
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_TIMER Timer Manager
 *  @{
 */

#ifndef _RTEMS_RTEMS_TIMER_INL
#define _RTEMS_RTEMS_TIMER_INL


/**
 *  @brief allocate a timer
 *  
 *  This function allocates a timer control block from
 *  the inactive chain of free timer control blocks.
 *
 *  @return returns the allocated timer
 */
RTEMS_INLINE_ROUTINE Timer_Control *_Timer_Allocate(void)
{
   /* allocate the timer object information */
   return (Timer_Control *) _Objects_Allocate(&_Timer_Information);
}


/**
 *  @brief free a timer
 *  
 *  This routine frees a timer control block to the
 *  inactive chain of free timer control blocks.
 *
 *  @param[in] the_timer the timer to free
 */
RTEMS_INLINE_ROUTINE void _Timer_Free(
                                      Timer_Control *the_timer
                                      )
{
   /* free the timer object information */
   _Objects_Free(&_Timer_Information , &the_timer->Object);
}


/**
 *  @brief get a timer
 *  
 *  This function maps timer IDs to timer control blocks.
 *  If ID corresponds to a local timer, then it returns
 *  the timer control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  Otherwise, location is set
 *  to OBJECTS_ERROR and the returned value is undefined.
 *
 *  @param[in] id the timer id
 *  @param[out] location the object location
 *
 *  @return returns the timer or NULL if it exists none
 */
RTEMS_INLINE_ROUTINE Timer_Control *_Timer_Get(
                                               Objects_Id id ,
                                               Objects_Locations *location
                                               )
{
   /* get the timer object information */
   return (Timer_Control *) _Objects_Get(&_Timer_Information , id , location);
}


/**
 *  @brief determine if the timer class is dormant
 *
 *  @param[in] the_class the timer class
 *
 *  @return returns TRUE if the class is that of a DORMANT
 *  timer, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Timer_Is_dormant_class(
                                                     Timer_Classes the_class
                                                     )
{
   /* return true if the the timer class is dormant */
   return ( the_class == TIMER_DORMANT );
}


/**
 *  @brief reset the timer ticks of the timer server thread
 */
RTEMS_INLINE_ROUTINE void _Timer_Server_reset_ticks_timer()
{
   /* check if the timer server ticks chain is not empty */
   if(!_Chain_Is_empty(&_Timer_Ticks_chain))
   {
      /* if not empty, then insert the timer server thread watchdog timer */
      _Watchdog_Insert_ticks(&_Timer_Server->Timer ,
                             ( (Watchdog_Control *) _Timer_Ticks_chain.first )->delta_interval);
   }
}


/**
 *  @brief reset the timer seconds of the timer server thread
 */
RTEMS_INLINE_ROUTINE void _Timer_Server_reset_seconds_timer()
{
   /* check if the timer server seconds chain is not empty */

   if(!_Chain_Is_empty(&_Timer_Seconds_chain))
   {
      /* if not empty, then insert the timer server thread watchdog timer */
      _Watchdog_Insert_seconds(&_Timer_Seconds_timer ,
                               ( (Watchdog_Control *) _Timer_Seconds_chain.first )->delta_interval);
   }
}

#endif
/* end of include file */

/**  
 *  @}
 */

/**
 *  @}
 */
