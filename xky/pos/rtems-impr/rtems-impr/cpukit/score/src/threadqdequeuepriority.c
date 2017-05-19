/**
 *  @file
 *  threadqdequeuepriority.c
 *
 *  @brief dequeue a thread from a priority thread queue
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
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
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
 *  @addtogroup ScoreThreadQ Thread Queue Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tqdata.h>


Thread_Control *_Thread_queue_Dequeue_priority(
                                               Thread_queue_Control *the_thread_queue
                                               )
{
    /* iterator through the thread queue data number of headers */
    uint32_t index;

    /* interrupt level */
    ISR_Level level;

    /* thread to dequeue (initialized to NULL just to remove warnings) */
    Thread_Control *the_thread = NULL;

    /* auxiliary variable */
    Thread_Control *new_first_thread;

    /* first node on blocked2n chain */
    Chain_Node *new_first_node;

    /* second node on blocked2n chain */
    Chain_Node *new_second_node;

    /* last node on blocked2n chain */
    Chain_Node *last_node;

    /* next thread after the one to be dequeued */
    Chain_Node *next_node;

    /* previous thread after the one to be dequeued */
    Chain_Node *previous_node;


    /* enter critical section */
    _ISR_Disable(level);

    /* search through the number of thread queue data number of headers */
    /* by priority order */
    for(index = 0;
        index < TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS;
        index++)
    {
        /* if the chain on this priority is empty */
        if(!_Chain_Is_empty(&the_thread_queue->Queues.Priority[ index ]))
        {
            /* then get the highest priority thread on the chain (first on the chain) */
            the_thread = (Thread_Control *)
                the_thread_queue->Queues.Priority[ index ].first;

            /* dequeue the thread from the thread queue */
            goto dequeue;
        }
    }

    /* check the thread queue state */
    switch(the_thread_queue->sync_state)
    {

            /* thread queue is synchronized */
        case THREAD_QUEUE_SYNCHRONIZED:

            /* or thread queue is satisfied */
        case THREAD_QUEUE_SATISFIED:

            /* leave critical section */
            _ISR_Enable(level);

            /* there are no threads */
            return NULL;

            /* thread queue is nothing happened */
        case THREAD_QUEUE_NOTHING_HAPPENED:

            /* or thread queue timeout */
        case THREAD_QUEUE_TIMEOUT:

            /* set the state to satisfied (the executing thread is the one
             * dequeued) */
            the_thread_queue->sync_state = THREAD_QUEUE_SATISFIED;

            /* leave critical section */
            _ISR_Enable(level);

            /* return executing thread */
            return _Thread_Executing;

            /* only the above disciplines are defined */
        default:

            /* raise internal error */
            _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                     TRUE ,
                                     INTERNAL_ERROR_INVALID_THREAD_QUEUE_STATE);

    }

    /* dequeue the thread */
dequeue:
            
    /* reset the thread wait queue */
    the_thread->Wait.queue = NULL;

    /* get first blocked thread */
    new_first_node = the_thread->Wait.Block2n.first;
    new_first_thread = (Thread_Control *) new_first_node;

    /* get next node */
    next_node = the_thread->Object.Node.next;

    /* get previous node */
    previous_node = the_thread->Object.Node.previous;

    /* check if the thread queue is empty */
    if(!_Chain_Is_empty(&the_thread->Wait.Block2n))
    {
        /* is not empty, so */

        /* get last node */
        last_node = the_thread->Wait.Block2n.last;

        /* get the new second node */
        new_second_node = new_first_node->next;

        /* remove the node from the chain */
        previous_node->next = new_first_node;
        next_node->previous = new_first_node;
        new_first_node->next = next_node;
        new_first_node->previous = previous_node;

        /* check if the chain has only one node */
        if(!_Chain_Has_only_one_node(&the_thread->Wait.Block2n))
        {
            /* > two threads on 2-n */
            new_second_node->previous = _Chain_Head(&new_first_thread->Wait.Block2n);

            new_first_thread->Wait.Block2n.first = new_second_node;
            new_first_thread->Wait.Block2n.last = last_node;

            last_node->next = _Chain_Tail(&new_first_thread->Wait.Block2n);
        }
    }
    else
    {
        previous_node->next = next_node;
        next_node->previous = previous_node;
    }

    /* check if there is an active watchdog */
    if(!_Watchdog_Is_active(&the_thread->Timer))
    {
        /* there isnt, so */

        /* leave critical section */
        _ISR_Enable(level);

        /* and unblock the thread */
        _Thread_Unblock(the_thread);
    }
    else
    {
        /* there is an active watchdog, so */

        /* deactivate the thread watchdog */
        _Watchdog_Deactivate(&the_thread->Timer);

        /* leave critical section */
        _ISR_Enable(level);

        /* remove the thread watchdog */
        (void) _Watchdog_Remove(&the_thread->Timer);

        /* unblock the thread */
        _Thread_Unblock(the_thread);
    }

#if defined(RTEMS_MULTIPROCESSING)

    /* if the thread is not local */
    if(!_Objects_Is_local_id(the_thread->Object.id))
    {
        /* then free it remotely */
        _Thread_MP_Free_proxy(the_thread);
    }
#endif

    /* return the thread */
    return ( the_thread );
}

/**  
 *  @}
 */

/**
 *  @}
 */

