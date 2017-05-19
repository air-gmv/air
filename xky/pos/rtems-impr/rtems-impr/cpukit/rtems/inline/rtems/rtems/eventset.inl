/**
 *  @file
 *  eventset.inl
 *
 *  @brief information pertaining to event sets.
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
 *  @addtogroup RTEMS_API_COMMON Common
 *  @{
 */

#ifndef _RTEMS_RTEMS_EVENTSET_INL
#define _RTEMS_RTEMS_EVENTSET_INL


/**
 *  @brief determine if the event set is empty
 *
 *  @param[in] the_event_set the event set
 *
 *  @return returns TRUE if on events are posted in the event_set,
 *  and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _Event_sets_Is_empty(
                                                  rtems_event_set the_event_set
                                                  )
{
   /* return true if the event set is empty */
   return ( the_event_set == 0 );
}


/**
 *  @brief post the new events into the event set
 *  
 *  This routine posts the given new_events into the event_set
 *  passed in.  The result is returned to the user in event_set.
 *
 *  @param[in] the_new_events the new events set
 *  @param[in] the_event_set the event set
 */
RTEMS_INLINE_ROUTINE void _Event_sets_Post(
                                           rtems_event_set the_new_events ,
                                           rtems_event_set *the_event_set
                                           )
{
   /* interrupt level */
   ISR_Level level;

   /* enter critical section */
   _ISR_Disable(level);

   /* add to the event set the new events */
   *the_event_set |= the_new_events;

   /* leave critical section */
   _ISR_Enable(level);
}


/**
 *  @brief get the event set desired
 *
 *  @param[in] the_event_set the event set
 *  @param[in] the_event_condition the event condition
 *
 *  @return returns the events in event_condition which are
 *  set in event_set.
 */
RTEMS_INLINE_ROUTINE rtems_event_set _Event_sets_Get(
                                                     rtems_event_set the_event_set ,
                                                     rtems_event_set the_event_condition
                                                     )
{
   /* return the event set AND wise with the event condition */
   return ( the_event_set & the_event_condition );
}


/**
 *  @brief clear events desired from the event set
 *  
 *  This function removes the events in mask from the event_set
 *  passed in.  The result is returned to the user in event_set.
 *
 *  @param[in] the_event_set the event set
 *  @param[in] the_mask the event mask
 *
 *  @return returns the event set with the mask applied to it
 */
RTEMS_INLINE_ROUTINE rtems_event_set _Event_sets_Clear(
                                                       rtems_event_set the_event_set ,
                                                       rtems_event_set the_mask
                                                       )
{
   /* return the event set without the events of the mask */
   return ( the_event_set & ~( the_mask ) );
}

#endif
/* end of include file */

/**  
 *  @}
 */

/**
 *  @}
 */
