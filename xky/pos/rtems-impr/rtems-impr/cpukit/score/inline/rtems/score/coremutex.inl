/**
 *  @file
 *  coremutex.inl
 *
 *  @brief contains all of the inlined routines associated
 *  with the CORE mutexes.
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
 *  3904    | 21/07/2009  | mcoutinho    | IPR 99
 *  4546    | 28/09/2009  | mcoutinho    | IPR 778
 *  4548    | 28/09/2009  | mcoutinho    | IPR 618
 *  4880    | 19/10/2009  | mcoutinho    | IPR 617
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  5322    | 02/11/2009  | mcoutinho    | IPR 852
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
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

#ifndef _RTEMS_SCORE_COREMUTEX_INL
#define _RTEMS_SCORE_COREMUTEX_INL

#include <rtems.h>
#include <rtems/system.h>


/**
 *  @brief determine if the Mutex is locked
 *  
 *  This routine returns TRUE if the mutex specified is locked and FALSE
 *  otherwise.
 *  
 *  @param[in] the_mutex is the mutex to check
 *  
 *  @return returns TRUE if the mutex is locked.
 */
RTEMS_INLINE_ROUTINE boolean _CORE_mutex_Is_locked(
                                                   CORE_mutex_Control *the_mutex
                                                   )
{
   /* return true if the mutex is locked */
   return the_mutex->lock == CORE_MUTEX_LOCKED;
}


/**
 *  @brief determine if the mutex is FIFO
 *  
 *  This routine returns TRUE if the mutex's wait discipline is FIFO and FALSE
 *  otherwise.
 *  
 *  @param[in] the_attribute is the attribute set of the mutex
 *  
 *  @return returns TRUE if the mutex is using FIFO blocking order.
 */
RTEMS_INLINE_ROUTINE boolean _CORE_mutex_Is_fifo(
                                                 CORE_mutex_Attributes *the_attribute
                                                 )
{
   /* return true if the mutex attribute discipline is FIFO */
   return the_attribute->discipline == CORE_MUTEX_DISCIPLINES_FIFO;
}


/**
 *  @brief determine if Mutex uses priority inheritance
 *  
 *  This routine returns TRUE if the mutex's wait discipline is
 *  INHERIT_PRIORITY and FALSE otherwise.
 *  
 *  @param[in] the_attribute is the attribute set of the mutex
 *  
 *  @return returns TRUE if the mutex is using priority
 *  inheritance.
 */
RTEMS_INLINE_ROUTINE boolean _CORE_mutex_Is_inherit_priority(
                                                             CORE_mutex_Attributes *the_attribute
                                                             )
{
   /* return true if the mutex attribute discipline is inherit */
   return the_attribute->discipline == CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT;
}


/**
 *  @brief determine if mutex uses priority ceiling
 *  
 *  This routine returns TRUE if the mutex's wait discipline is
 *  PRIORITY_CEILING and FALSE otherwise.
 *  
 *  @param[in] the_attribute is the attribute set of the mutex
 *  
 *  @return returns TRUE if the mutex is using priority
 *  ceiling.
 */
RTEMS_INLINE_ROUTINE boolean _CORE_mutex_Is_priority_ceiling(
                                                             CORE_mutex_Attributes *the_attribute
                                                             )
{
   /* return true if the mutex attribute discipline is ceiling */
   return the_attribute->discipline == CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING;
}


/**
 *  @brief seize mutex
 *  
 *  This routine attempts to receive a unit from the_mutex.
 *  If a unit is available or if the wait flag is FALSE, then the routine
 *  returns.  Otherwise, the calling task is blocked until a unit becomes
 *  available.
 *  
 *  @param[in] the_mutex is the mutex to attempt to lock
 *  @param[in] level_p is the interrupt level holder
 *  
 *  @return This routine returns 0 if "trylock" can resolve whether or not
 *  the mutex is immediately obtained or there was an error attempting to
 *  get it.  It returns 1 to indicate that the caller cannot obtain
 *  the mutex and will have to block to do so.
 */
RTEMS_INLINE_ROUTINE int _CORE_mutex_Seize_interrupt_trylock(
                                                             CORE_mutex_Control *the_mutex ,
                                                             ISR_Level *level_p
                                                             )
{
   /* thread executing */
   Thread_Control *executing;


   /* interrupts are disabled when you get here */

   /* get the calling thread */
   executing = _Thread_Executing;

   /* set the default return code to success */
   executing->Wait.return_code = CORE_MUTEX_STATUS_SUCCESSFUL;

   /* check if the mutex is locked */
   if(!_CORE_mutex_Is_locked(the_mutex))
   {
      /* mutex was unlocked */

      /* so lock it */
      the_mutex->lock = CORE_MUTEX_LOCKED;

      /* set the holder thread to be the calling thread */
      the_mutex->holder = executing;

      /* set the holder id to be the id of the calling thread */
      the_mutex->holder_id = executing->Object.id;

      /* set the nest count to one (was zero) */
      the_mutex->nest_count = 1;

      /* check if inherit or ceiling protocol is used */
      if(_CORE_mutex_Is_inherit_priority(&the_mutex->Attributes) ||
         _CORE_mutex_Is_priority_ceiling(&the_mutex->Attributes))
      {
         /* increment the calling thread resource count */
         executing->resource_count++;
      }

      /* if the semaphore is not ceiling */
      if(!_CORE_mutex_Is_priority_ceiling(&the_mutex->Attributes))
      {
         /* then can return (nothing else to be done) */
         return 0;
      }

      /* the mutex is priority ceiling (CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING) */

      /* we possibly bump the priority of the current holder -- which
       * happens to be _Thread_Executing */
      {
         /* auxliary variable for the priority ceiling */
         Priority_Control ceiling;

         /* auxiliary variable for the current thread priority */
         Priority_Control current;


         /* get the ceiling priority */
         ceiling = the_mutex->Attributes.priority_ceiling;

         /* get the calling thread current priority */
         current = executing->current_priority;

         /* check if the thread priority is equal to the ceiling */
         if(current == ceiling)
         {
            /* if so, there is no need to change the thread priority so leave */
            return 0;
         }

         /* the current priority is diferent ceiling */

         /* the option "current < ceiling" cannot occur since it is
          * assumed to be checked before this is function is invoked */

         /* thus the only option left is (current > ceiling), so raise
          * the thread priority up to ceiling */

         /* disable thread dispatch */
         _Thread_Disable_dispatch();

         /* change the priority */
         _Thread_Change_priority(the_mutex->holder ,
                                 the_mutex->Attributes.priority_ceiling ,
                                 FALSE);

         /* and re-enable thread dispatch */
         _Thread_Enable_dispatch();

         /* return 0 */
         return 0;
      }
   }

   /*  At this point, we know the mutex is locked (wasn't initialy available) */

   /*  check if the thread executing is the one that locked the mutex */
   if(_Thread_Is_executing(the_mutex->holder))
   {
      /* the thread is the one executing */

      /* let's see if we are allowed to nest access. */
      switch(the_mutex->Attributes.lock_nesting_behavior)
      {
         /* nesting behavior is nesting acquires */
         case CORE_MUTEX_NESTING_ACQUIRES:

            /* when defining this behaviour you allow to the executing mutex
             * holder thread to acquire the mutex */

            /* so there is no need to wait as is allredy locked by the executing
             * thread, just need to increment the nest count */
            the_mutex->nest_count++;

            /* return 0 */
            return 0;

            /* nesting behavior is nesting blocks */
         case CORE_MUTEX_NESTING_BLOCKS:

            /* when defining this behaviour the caller cannot obtain the mutex
             * and will have to block to do so */

            break;

            /* default case, invalid nesting behavior) */
         default:
            /* only the two cases above are defined */

            /* raise internal error */
            _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                     TRUE ,
                                     INTERNAL_ERROR_INVALID_LOCK_NESTING_BEHAVIOR);
      }
   }

   /* The mutex is not available and the caller must deal with the possibility
    * of blocking */
   return 1;
}


/**
 *  @brief seize a mutex. If the mutex is available, returns. If the mutex
 *  is not available is not available, blocks.
 *
 *  This routine attempts to obtain the mutex.  If the mutex is available,
 *  then it will return immediately.  Otherwise, it will invoke the
 *  support routine @a _Core_mutex_Seize_interrupt_blocking.
 *
 *  @param[in] the_mutex is the mutex to attempt to lock
 *  @param[in] id is the Id of the owning API level Semaphore object
 *  @param[in] wait is TRUE if the thread is willing to wait
 *  @param[in] timeout is the maximum number of ticks to block
 *  @param[in] level is a temporary variable used to contain the ISR
 *         disable level cookie
 *
 *  @note If the mutex is called from an interrupt service routine,
 *        with context switching disabled, or before multitasking,
 *        then a fatal error is generated.
 *
 *
 *  The logic on this routine is as follows:\n
 *\n
 *  * If incorrect system state\n
 *      return an error (raise internal error) \n
 *\n
 *  * If is ceiling and ceiling is lower than thread executing priority \n
 *      return an error (on the thread return code of the Wait structure) \n
 *\n
 *  * If mutex is available without any contention or blocking\n
 *      obtain it with interrupts disabled and returned\n
 *\n
 *  * If the caller is willing to wait\n
 *      then they are blocked.\n
 */
RTEMS_INLINE_ROUTINE void _CORE_mutex_Seize(
                                            CORE_mutex_Control *the_mutex ,
                                            rtems_id id ,
                                            rtems_boolean wait ,
                                            rtems_interval timeout ,
                                            ISR_Level level)
{

   /* check if the system is ok */
   if(_Thread_Dispatch_disable_level &&
      ( wait ) &&
      ( _System_state_Get() >= SYSTEM_STATE_BEGIN_MULTITASKING ))
   {
      /* raise an internal error -> called from wrong environment */
      _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                               FALSE ,
                               INTERNAL_ERROR_WRONG_ENVIRONMENT);
   }

   /* check the thread priority of mutexes with ceiling protocol */
   if(_CORE_mutex_Is_priority_ceiling(&the_mutex->Attributes) &&
      ( the_mutex->Attributes.priority_ceiling ) > _Thread_Executing->current_priority)
   {
      /* mutex ceiling has been violated, cannot seize the mutex */
      _Thread_Executing->Wait.return_code = CORE_MUTEX_STATUS_CEILING_VIOLATED;

      /* leave critical section */
      _ISR_Enable(level);
   }
      /* check if the mutex is available */
   else if(_CORE_mutex_Seize_interrupt_trylock(the_mutex , &level))
   {
      /* if the mutex is not available then */

      /* check if the calling thread should wait for the mutex */
      if(!wait)
      {
         /* does not have to wait */

         /* leave critical section */
         _ISR_Enable(level);

         /* and return the mutex could not be obtained */
         _Thread_Executing->Wait.return_code = CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT;
      }
      else
      {
         /* if the thread wants to wait, then place it on the queue */

         /* state the thread queue is entering critical section */
         _Thread_queue_Enter_critical_section(&( the_mutex )->Wait_queue);

         /* set the calling thread wait queue to be the mutex wait queue */
         _Thread_Executing->Wait.queue = &( the_mutex )->Wait_queue;

         /* set the mutex identifier as the object the thread is waiting on */
         _Thread_Executing->Wait.id = id;

         /* check if inherit protocol is used */
         if(_CORE_mutex_Is_inherit_priority(&( the_mutex )->Attributes))
         {
            /* if so, then set the thread  */
            _Thread_Executing->Wait.mutex_blocked = (ptrCORE_mutex_Control) the_mutex;

            /* the executing thread goes to the chain of blocked threads of the holder thread */
            _Chain_Append(&( the_mutex )->holder->Wait.threads_blocked , &_Thread_Executing->Wait.node);
         }

         /* disable thread dispatch */
         _Thread_Disable_dispatch();

         /* leave critical section */
         _ISR_Enable(level);

         /* seize the mutex with a timeout */
         _CORE_mutex_Seize_interrupt_blocking(the_mutex , timeout);
      }
   }
      /* if the mutex is available then */
   else
   {
      /* if nesting is allowed */
      if(( the_mutex )->Attributes.lock_nesting_behavior == CORE_MUTEX_NESTING_ACQUIRES)
      {
         /* the mutex is available and the nesting is allowed */
         
         /* only append the mutex one time */
         if(( the_mutex )->nest_count == 1)
         {
            /* add the mutex to the list of mutexes hold by the thread */
            _Chain_Append_unprotected(&_Thread_Executing->Wait.mutexes_hold , &( the_mutex )->Node);
         }
      }
      else
      {
         /* the mutex is available but the nesting is not allowed */

         /* add the mutex to the list of mutexes hold by the thread */
         _Chain_Append_unprotected(&_Thread_Executing->Wait.mutexes_hold , &( the_mutex )->Node);
      }

      /* leave critical section */
      _ISR_Enable(level);
   }
}

/**
 * @}
 */

#endif
/* end of include file */

/**  
 *  @}
 */
