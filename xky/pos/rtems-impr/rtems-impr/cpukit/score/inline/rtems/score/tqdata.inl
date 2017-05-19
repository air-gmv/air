/**
 *  @file
 *  tqdata.inl
 *
 *  @brief contains the static inline implementation of the inlined
 *  routines needed to support the Thread Queue Data.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreThreadQ Thread Queue Handler
 *  @{
 */

#ifndef _RTEMS_SCORE_TQDATA_INL
#define _RTEMS_SCORE_TQDATA_INL

#include <rtems/system.h>


/**
 *  @brief get the priority header number
 *  
 *  This function returns the index of the priority chain on which
 *  a thread of the_priority should be placed.
 *
 *  @param[in] the_priority the priority to obtain the header number
 *
 *  @return returns the priority header number
 */
RTEMS_INLINE_ROUTINE uint32_t _Thread_queue_Header_number(
                                                          Priority_Control the_priority
                                                          )
{
   /* header number is the higher part of the priority */
   return (the_priority / TASK_QUEUE_DATA_PRIORITIES_PER_HEADER );
}


/**
 *  @brief determine if RTEMS should search the thread queue data in reverse
 *  order
 *
 *  @param[in] the_priority the priority find if is to reverse search or not
 *
 *  @return returns different than 0 if the_priority indicates that the
 *  enqueue search should start at the front of this priority
 *  group chain and 0 otherwise
 */
RTEMS_INLINE_ROUTINE boolean _Thread_queue_Is_reverse_search(
                                                             Priority_Control the_priority
                                                             )
{
   /* use reverse search if the priority is larger than 32 */
   return ( the_priority & TASK_QUEUE_DATA_REVERSE_SEARCH_MASK );
}


/**
 *  @brief announce entering in thread queue critical section
 *  
 *  This routine is invoked to indicate that the specified thread queue is
 *  entering a critical section.
 *
 *  @param[in] the_thread_queue the thread queue to announce is in critical
 *  section
 */
RTEMS_INLINE_ROUTINE void _Thread_queue_Enter_critical_section(
                                                               Thread_queue_Control *the_thread_queue
                                                               )
{
   /* set the thread queue sync state to nothing happened */
   the_thread_queue->sync_state = THREAD_QUEUE_NOTHING_HAPPENED;
}

/**
 *  @}
 */

#endif
/* end of include file */

/**  
 *  @}
 */
