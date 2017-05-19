/**
 *  @file
 *  threadq.inl
 *
 *  @brief file contains all of the inlined routines associated with
 *  the manipulation of thread queues.
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
 *  4790    | 09/10/2009  | mcoutinho    | IPR 99
 *  4790    | 09/10/2009  | mcoutinho    | IPR 553
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  7253    | 12/04/2010  | mcoutinho    | IPR 2266
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreThreadQ Thread Queue Handler
 * @{
 */

#ifndef _RTEMS_SCORE_THREADQ_INL
#define _RTEMS_SCORE_THREADQ_INL

#include <rtems/score/thread.h>
#include <rtems/score/coremutex.h>
#include <rtems/system.h>


/**
 *  @brief process Thread Queue Timeout
 *  
 *  This is a shared helper routine which makes it easier to have multiple
 *  object class specific timeout routines.
 *  
 *  @param[in] the_thread is the thread to extract
 *  
 *  @note Assume Dispatching is disabled.
 */
RTEMS_INLINE_ROUTINE void _Thread_queue_Process_timeout(
                                                        Thread_Control *the_thread
                                                        )
{
   /* thread queue */
   Thread_queue_Control *the_thread_queue = the_thread->Wait.queue;

   /* mutex the thread is blocked on (if any) */
   CORE_mutex_Control *mutex;

   /* new priority if the mutex is inheritance */
   Priority_Control new_priority;


   /* If the_thread_queue is not synchronized, then it is either
    * "nothing happened", "timeout", or "satisfied".   If the_thread
    * is the executing thread, then it is in the process of blocking
    * and it is the thread which is responsible for the synchronization
    * process.
    *
    * If it is not satisfied, then it is "nothing happened" and
    * this is the "timeout" transition.  After a request is satisfied,
    * a timeout is not allowed to occur */

   /* check if the thread queue state is not synchronized and the thread is not
    * executing at the moment */
   if(the_thread_queue->sync_state != THREAD_QUEUE_SYNCHRONIZED &&
      _Thread_Is_executing(the_thread))
   {
      /* if the thread queue state is not satisfied */
      if(the_thread_queue->sync_state != THREAD_QUEUE_SATISFIED)
      {
         /* then there was a timeout */
         the_thread_queue->sync_state = THREAD_QUEUE_TIMEOUT;
      }
   }
   else
   {
      /* the thread queue is syncronized or the thread is executing */

      /* set the thread return code to match the thread queue timeout status */
      the_thread->Wait.return_code = the_thread->Wait.queue->timeout_status;

      /* extract the thread from the thread queue */
      _Thread_queue_Extract(the_thread->Wait.queue , the_thread);
   }

   /* get the mutex the thread is waiting on (if any) */
   mutex = (CORE_mutex_Control *) the_thread->Wait.mutex_blocked;

   /* if the thread is blocking on a mutex with inherit protocol*/
   if(mutex != NULL && _CORE_mutex_Is_inherit_priority(&mutex->Attributes))
   {
      /* get the muxtex holder thread */
      Thread_Control *holder = mutex->holder;

      /* extract the thread node from the chain */
      _Chain_Extract(&the_thread->Wait.node);

      /* reset the node */
      the_thread->Wait.node.next = NULL;
      the_thread->Wait.node.previous = NULL;

      /* find out the new priority and change it*/

      /* if there are more threads waiting on the mutex */
      if(!_Chain_Is_empty(&holder->Wait.threads_blocked))
      {
         /* the new priority of the holder is the last position of the holder
          * thread list */

         /* dont need to disable interrupts since this function is called
          * inside a clock ISR and semaphores with inherance or priority ceiling
          * protocols cannot be released inside ISRs */

         /* get the wait info of the last thread waiting on the mutex */
         Thread_Wait_information *wait_info = (Thread_Wait_information *)
             _Chain_Tail(&holder->Wait.threads_blocked)->previous;

         /* the new priority of the holder will be the priority of the last
          * thread on the chain */
         new_priority = wait_info->thread->current_priority;
      }
      else
      {
         /* if there are no more threads waiting */

         /* then the new prioririty is the original holder thread priority */
         new_priority = holder->real_priority;
      }

      /* check if there is need to change the holder thread priority */
      if(new_priority != holder->current_priority)
      {
         /* change the priority of the holder thread */
         _Thread_Change_priority(holder , new_priority , TRUE);
      }
   }
}

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
