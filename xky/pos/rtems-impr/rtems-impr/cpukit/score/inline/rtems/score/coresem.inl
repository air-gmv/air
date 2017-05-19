/**
 *  @file
 *  coresem.inl
 *
 *  @brief contains all of the inlined routines associated
 *  with the SuperCore semaphore.
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
 *  @addtogroup ScoreSemaphore Semaphore Handler
 *  @{
 */

#ifndef _RTEMS_SCORE_CORESEM_INL
#define _RTEMS_SCORE_CORESEM_INL

#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/system.h>


/**
 *  @brief determine if semaphore has priority protocol
 *  
 *  This function returns TRUE if the priority attribute is
 *  enabled in the @a attribute_set and FALSE otherwise.
 *  
 *  @param[in] the_attribute is the attribute set to test
 *  
 *  @return TRUE if the priority attribute is enabled
 */
RTEMS_INLINE_ROUTINE boolean _CORE_semaphore_Is_priority(
                                                         CORE_semaphore_Attributes *the_attribute
                                                         )
{
   /* return true if the semaphore discipline priority queueing */
   return ( the_attribute->discipline == CORE_SEMAPHORE_DISCIPLINES_PRIORITY );
}


/**
 *  @brief seize semaphore (with interrupts disabled)
 *  
 *  This routine attempts to receive a unit from the_semaphore.
 *  If a unit is available or if the wait flag is FALSE, then the routine
 *  returns.  Otherwise, the calling task is blocked until a unit becomes
 *  available.
 *  
 *  @param[in] the_semaphore is the semaphore to obtain
 *  @param[in] id is the Id of the owning API level Semaphore object
 *  @param[in] wait is TRUE if the thread is willing to wait
 *  @param[in] timeout is the maximum number of ticks to block
 *  @param[in] level_p is a temporary variable used to contain the ISR
 *  disable level cookie
 *  
 *  @note There is currently no MACRO version of this routine.
 */
RTEMS_INLINE_ROUTINE void _CORE_semaphore_Seize_isr_disable(
                                                            CORE_semaphore_Control *the_semaphore ,
                                                            Objects_Id id ,
                                                            boolean wait ,
                                                            Watchdog_Interval timeout ,
                                                            ISR_Level *level_p
                                                            )
{
   /* executing thread */
   Thread_Control *executing;

   /* interrupt level */
   ISR_Level level = *level_p;

   
   /* interrupts are disabled when you get here */

   /* get the calling thread */
   executing = _Thread_Executing;

   /* set the default return code to success */
   executing->Wait.return_code = CORE_SEMAPHORE_STATUS_SUCCESSFUL;

   /* check if the sempahore count is not zero (sempahore is available) */
   if(the_semaphore->count != 0)
   {
      /* decrease the count */
      the_semaphore->count -= 1;

      /* leave critical section */
      _ISR_Enable(level);

      /* and return (obtained the semaphore) */
      return;
   }

   /* semaphore is not available here */

   /* check if should wait for the semaphore */
   if(!wait)
   {
      /* leave critical section */
      _ISR_Enable(level);

      /* set the return code to unsatisfied */
      executing->Wait.return_code = CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT;

      /* and return (semaphore not available) */
      return;
   }

   /* here the semaphore is not available and the calling thread is going to be
    * blocked */

   /* disable thread dispatch */
   _Thread_Disable_dispatch();

   /* enter the thread queue critical section */
   _Thread_queue_Enter_critical_section(&the_semaphore->Wait_queue);

   /* set the calling thread wait queue to the semaphore wait queue */
   executing->Wait.queue = &the_semaphore->Wait_queue;

   /* set the calling thread wait object to the semaphore identifier */
   executing->Wait.id = id;

   /* leave critical section */
   _ISR_Enable(level);

   /* enqueue the calling thread on the semaphore wait queue */
   _Thread_queue_Enqueue(&the_semaphore->Wait_queue , timeout);

   /* re-enable thread dispacth */
   _Thread_Enable_dispatch();
}

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
