/**
 *  @file
 *  coremutex.c
 *
 *  @brief initialize a core mutex
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
 *  3228    | 15/05/2009  | mcoutinho    | IPR 99
 *  3755    | 13/07/2009  | mcoutinho    | IPR 553
 *  3906    | 21/07/2009  | mcoutinho    | IPR 585
 *  5247    | 30/10/2009  | mcoutinho    | IPR 828
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


void _CORE_mutex_Initialize(
                            CORE_mutex_Control *the_mutex ,
                            CORE_mutex_Attributes *the_mutex_attributes ,
                            uint32_t initial_lock ,
                            ptrSemaphore_Control semaphore
                            )
{
    /* discipline of the mutex */
    Thread_queue_Disciplines discipline;


    /* set the attributes of the mutex */
    the_mutex->Attributes = *the_mutex_attributes;

    /* set the mutex initial lock */
    the_mutex->lock = initial_lock;

    /* set the mutex number of blocked threads */
    the_mutex->blocked_count = 0;

    /* set the mutex classic API semaphore */
    the_mutex->semaphore = semaphore;

    /* initialize the node */

    /* initialize the next field */
    the_mutex->Node.next = NULL;

    /* initialize the previous field */
    the_mutex->Node.previous = NULL;

    /* if the mutex is initially locked */
    if(initial_lock == CORE_MUTEX_LOCKED)
    {
        /* then the holder is the executing thread */

        /* set the nest count to 1 */
        the_mutex->nest_count = 1;

        /* holder is the executing thread */
        the_mutex->holder = _Thread_Executing;

        /* holder id is the executing thread id */
        the_mutex->holder_id = _Thread_Executing->Object.id;

        /* append the mutex to the list of mutexes hold by the executing thread */
        _Chain_Append(&_Thread_Executing->Wait.mutexes_hold , &the_mutex->Node);

        /* if the mutex is priority ceiling or inheritance */
        if(_CORE_mutex_Is_inherit_priority(&the_mutex->Attributes) ||
           _CORE_mutex_Is_priority_ceiling(&the_mutex->Attributes))
        {
            /* then increment the resource count */
            _Thread_Executing->resource_count++;
        }

        /* if it is ceiling mutex */
        if(_CORE_mutex_Is_priority_ceiling(&the_mutex->Attributes))
        {
            /* increase the priority of the thread to the ceiling value */

            /* mutex priority ceiling */
            Priority_Control ceiling;

            /* holder thread current priority */
            Priority_Control current;

            /* get the ceiling priority */
            ceiling = the_mutex->Attributes.priority_ceiling;

            /* get the holder thread current priority */
            current = _Thread_Executing->current_priority;

            /* if the thread priority is lower than the ceiling */
            if(current > ceiling)
            {
                /* increase the priority of the thread */
                _Thread_Change_priority(the_mutex->holder ,
                                        the_mutex->Attributes.priority_ceiling ,
                                        FALSE);
            }
            /* if the thread priority is higher than the ceiling:
             *  this case has been handled before coming here */

            /* if current == ceiling then nothing needs to be done */
        }
    }

    else
    {
        /* the mutex is not initially locked */
        /* then nobody holds the mutex */

        /* the nest count is 0 */
        the_mutex->nest_count = 0;

        /* there is no holder */
        the_mutex->holder = NULL;

        /* there is no holder id */
        the_mutex->holder_id = 0;
    }

    /* check the mutex discipline */
    if(_CORE_mutex_Is_fifo(the_mutex_attributes))
    {
        /* discipline is FIFO */
        discipline = THREAD_QUEUE_DISCIPLINE_FIFO;
    }
    else
    {
        /* discipline is priority */
        discipline = THREAD_QUEUE_DISCIPLINE_PRIORITY;
    }

    /* initialize the mutex thread queue */
    _Thread_queue_Initialize(&the_mutex->Wait_queue ,
                             discipline ,
                             STATES_WAITING_FOR_MUTEX ,
                             CORE_MUTEX_TIMEOUT);
}

/**  
 *  @}
 */

/**
 *  @}
 */
