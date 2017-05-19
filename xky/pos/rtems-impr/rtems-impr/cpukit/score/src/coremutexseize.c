/**
 *  @file
 *  coremutexseize.c
 *
 *  @brief seize a core mutex
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
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreMutex Mutex Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>


void _CORE_mutex_Seize_interrupt_blocking(
                                          CORE_mutex_Control *the_mutex ,
                                          Watchdog_Interval timeout
                                          )
{
    /* executing thread */
   Thread_Control *executing;

   
   /* get the executing thread */
   executing = _Thread_Executing;

   /* check if the mutex is inheritance */
   if(_CORE_mutex_Is_inherit_priority(&the_mutex->Attributes))
   {
      /* check if the mutex holder priority should change */
      if(the_mutex->holder->current_priority > executing->current_priority)
      {
         /* change the mutex holder priority */
         _Thread_Change_priority(the_mutex->holder ,
                                 executing->current_priority ,
                                 FALSE);
      }
   }

   /* increment the number of threads waiting on this mutex */
   the_mutex->blocked_count++;

   /* enqueue the calling thread on the mutex thread queue */
   _Thread_queue_Enqueue(&the_mutex->Wait_queue , timeout);

   /* enable dispatch */
   _Thread_Enable_dispatch();
}


/**  
 *  @}
 */

/**
 *  @}
 */
