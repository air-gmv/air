/**
 *  @file
 *  coremutexsurrender.c
 *
 *  @brief surrender a core mutex
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
 *  611     | 17/11/2008  | mcoutinho    | IPR 64
 *  3907    | 21/07/2009  | mcoutinho    | IPR 99
 *  4548    | 28/09/2009  | mcoutinho    | IPR 618
 *  5098    | 28/10/2009  | mcoutinho    | IPR 553
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  7457    | 20/04/2010  | mcoutinho    | IPR 2269
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


CORE_mutex_Status _CORE_mutex_Surrender(
                                        CORE_mutex_Control *the_mutex ,
                                        Objects_Id id ,
#if defined(RTEMS_MULTIPROCESSING)
    CORE_mutex_API_mp_support_callout api_mutex_mp_support
                                        ,
#endif
    boolean force
                                        )
{
    /* next thread to hold the mutex */
    Thread_Control *the_thread;

    /* current mutex holder thread */
    Thread_Control *holder;

    /* the new thread priority after releasing the mutex */
    Priority_Control new_priority = 0;


    /* get the mutex holder thread */
    holder = the_mutex->holder;

    /* the following code allows a thread (or ISR) other than the thread
     * which acquired the mutex to release that mutex.  This is only
     * allowed when the mutex in question is FIFO or simple Priority
     * discipline.  But Priority Ceiling or Priority Inheritance mutexes
     * must be released by the thread which acquired them */

    /* if force is selected */
    if(force == TRUE)
    {
        /* then can only release iff the holder thread has been told to be
         * restarted/deleted */

        /* check the nest count (cannot be zero) */
        if(the_mutex->nest_count == 0)
        {
            /* raise internal error */
            _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                     TRUE ,
                                     INTERNAL_ERROR_INVALID_MUTEX_NEST_COUNT);
        }

        /* decrease the nest count (we are relasing the mutex) */
        the_mutex->nest_count--;
    }
    else
    {
        /* force is not selected */

        /* check the mutex attributes (only the owner can release it?) */
        if(the_mutex->Attributes.only_owner_release)
        {

            /* then if it is not the holder that is executing then the executing thread
             * cannot surrender the mutex */
            if(!_Thread_Is_executing(holder) ||
               rtems_interrupt_is_in_progress())
            {
                /* return not owner of resource */
                return CORE_MUTEX_STATUS_NOT_OWNER_OF_RESOURCE;
            }
        }

        /* check if the mutex is already unlocked  */
        if(!the_mutex->nest_count)
        {
            /* return success */
            return CORE_MUTEX_STATUS_SUCCESSFUL;
        }

        /* decrease the nest count (we are relasing the mutex) */
        the_mutex->nest_count--;

        /* check the nest count:
         *   if different then zero then just return
         * only when the nest_count is 0 will the mutex be surrendered to
         * another thread */
        if(the_mutex->nest_count != 0)
        {
            /* check the lock nesting behavior */
            switch(the_mutex->Attributes.lock_nesting_behavior)
            {
                    /* nesting acquires */
                case CORE_MUTEX_NESTING_ACQUIRES:

                    /* return success */
                    return CORE_MUTEX_STATUS_SUCCESSFUL;

                    /* no other behavior is allowed */
                default:
                    /* raise internal error */
                    _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                             TRUE ,
                                             INTERNAL_ERROR_INVALID_LOCK_NESTING_BEHAVIOR);
            }
        }
    }

    /* formally release the mutex before possibly transferring it to a
     * blocked thread */

    /* check if the semaphore is inherit or ceiling */
    if(_CORE_mutex_Is_inherit_priority(&the_mutex->Attributes) ||
       _CORE_mutex_Is_priority_ceiling(&the_mutex->Attributes))
    {
        /* decrease the resource count */
        holder->resource_count--;
    }

    /* extract the mutex from the holder thread list of hold mutexes
     * i.e, the holder thread (thread exexuting) no longer holds this mutex */
    _Chain_Extract(&the_mutex->Node);

    /* reset the mutex node */

    /* reset the next field */
    the_mutex->Node.next = NULL;

    /* reset the previous field */
    the_mutex->Node.previous = NULL;

    /* determine the next thread that has the mutex */
    the_thread = _Thread_queue_Dequeue(&the_mutex->Wait_queue);

    /* if the mutex has inheritance priority protocol */
    if(_CORE_mutex_Is_inherit_priority(&the_mutex->Attributes))
    {
        /* if the thread is not NULL */
        if(the_thread != NULL)
        {
            /* remove the next thread to hold the semaphore from the old thread holder
             * blocked list */
            _Chain_Extract(&the_thread->Wait.node);

            /* reset the thread node next */
            the_thread->Wait.node.next = NULL;

            /* reset the thread node previous */
            the_thread->Wait.node.previous = NULL;
        }

        /* if the chain of blocked threads is not empty */
        if(!_Chain_Is_empty(&_Thread_Executing->Wait.threads_blocked))
        {
            /* then the holder thread priority is defined by the last node on the
             * chain of threads blocked */

            /* get the thread info of the last thread on the chain */
            Thread_Wait_information *wait_info = (Thread_Wait_information *)
                _Chain_Tail(&_Thread_Executing->Wait.threads_blocked)->previous;

            /* get the priority of the last thread on the chain */
            new_priority = wait_info->thread->current_priority;
        }
        else
        {
            /* if the chain is empty, restore the initial priority */
            new_priority = holder->real_priority;
        }
    }

    /* if the mutex has ceiling priority protocol */
    if(_CORE_mutex_Is_priority_ceiling(&the_mutex->Attributes))
    {
        /* the priority will be defined by the last node on the chain of
         * mutexes hold by the thread */

        /* if the chain of blocked threads is not empty */
        if(!_Chain_Is_empty(&_Thread_Executing->Wait.mutexes_hold))
        {
            /* then the holder thread priority is defined by the last node on the
             * chain of mutexes hold */

            /* get the last mutex added */
            CORE_mutex_Control *last_mutex_added = (CORE_mutex_Control *)
                _Chain_Tail(&_Thread_Executing->Wait.mutexes_hold)->previous;

            /* set the priority as the ceiling of the last mutex added */
            new_priority = last_mutex_added->Attributes.priority_ceiling;
        }
        else
        {
            /* if the chain is empty, restore the initial priority */
            new_priority = holder->real_priority;
        }
    }

    /* restart the mutex, nobody holds it for now
     * if the_thread exists, then these fields will be updated */

    /* reset the holder thread */
    the_mutex->holder = NULL;

    /* reset the holder thread id */
    the_mutex->holder_id = 0;

    /* reset the nest count */
    the_mutex->nest_count = 0;

    /* update the priority of the old holder thread if the mutex
     * has inheritance or ceiling protocols */

    /* check if the semaphore is inherit or ceiling */
    if(_CORE_mutex_Is_inherit_priority(&the_mutex->Attributes) ||
       _CORE_mutex_Is_priority_ceiling(&the_mutex->Attributes))
    {
        /* if the holder holds no more mutexes */
        if(holder->resource_count == 0)
        {
            /* if the holder needs to change priority */
            if(holder->real_priority != holder->current_priority)
            {
                /* then change the holder thread priority to its original value */
                _Thread_Change_priority(holder , holder->real_priority , TRUE);
            }
        }
        else
        {
            /* if the holder thread has still some mutexes */
            if(new_priority != holder->current_priority && force == FALSE)
            {
                /* then change the holder thread priority to the new priority */
                _Thread_Change_priority(holder , new_priority , TRUE);
            }
        }
    }

    /* now we check if another thread was waiting for this mutex.  If so,
     * transfer the mutex to that thread */

    /* if thread is not NULL */
    if(the_thread != NULL)
    {
        /* then there is a thread waiting for this mutex */

#if defined(RTEMS_MULTIPROCESSING)
        /* if the thread is not local */
        if(!_Objects_Is_local_id(the_thread->Object.id))
        {
            /* then the holder is NULL (holder is remote) */
            the_mutex->holder = NULL;

            /* the holder id is the thread id */
            the_mutex->holder_id = the_thread->Object.id;

            /* nest count is 1 */
            the_mutex->nest_count = 1;

            /* call remote mutex function */
            ( *api_mutex_mp_support )( the_thread , id );
        }
        else
#endif
        {
            /* thread is local */

            /* set the mutex holder as the thread */
            the_mutex->holder = the_thread;

            /* the mutex holder id as the thread id */
            the_mutex->holder_id = the_thread->Object.id;

            /* nest count is 1 */
            the_mutex->nest_count = 1;

            /* the new holder is not blocked on any mutex now */
            the_thread->Wait.mutex_blocked = NULL;

            /* and is now on the list of hold mutexes by the thread */
            _Chain_Append(&the_thread->Wait.mutexes_hold , &the_mutex->Node);

            /* check the mutex atributes discipline */
            switch(the_mutex->Attributes.discipline)
            {
                    /* discipline is FIFO or priority (no inheritance or ceiling) */
                case CORE_MUTEX_DISCIPLINES_FIFO:
                case CORE_MUTEX_DISCIPLINES_PRIORITY:

                    /* just leave */
                    break;

                    /* inheritance discipline */
                case CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT:

                    /* increment the resource count */
                    the_thread->resource_count++;

                    /* and leave */
                    break;

                    /* ceiling discipline */
                case CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING:

                    /* increment the resource count */
                    the_thread->resource_count++;

                    /* if the mutex ceiling is higher than the thread priority */
                    if(the_mutex->Attributes.priority_ceiling < the_thread->current_priority)
                    {
                        /* then change the thread priority to the ceiling value */
                        _Thread_Change_priority(the_thread ,
                                                the_mutex->Attributes.priority_ceiling ,
                                                FALSE);
                    }

                    /* and leave */
                    break;

                    /* only the cases above are defined */
                default:

                    /* raise internal error */
                    _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                             TRUE ,
                                             INTERNAL_ERROR_INVALID_CORE_MUTEX_ATTRIBUTE);
            }
        }
    }
    else
    {
        /* there is no thread blocked on the mutex */

        /* unlock the mutex */
        the_mutex->lock = CORE_MUTEX_UNLOCKED;
    }

    /* return success */
    return CORE_MUTEX_STATUS_SUCCESSFUL;
}

/**  
 *  @}
 */

/**
 *  @}
 */
