/**
 *  @file
 *  threadqenqueuepriority.c
 *
 *  @brief enqueue a thread on a priority thread queue
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
 *  4861    | 19/10/2009  | mcoutinho    | IPR 810
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
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

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tqdata.h>


void _Thread_queue_Enqueue_priority(
                                    Thread_queue_Control *the_thread_queue ,
                                    Thread_Control *the_thread
                                    )
{
    /* iterator containing the thread priority  */
    Priority_Control search_priority;

    /* iterator through header chain */
    Thread_Control *search_thread;

    /* interrupt level */
    ISR_Level level;

    /* the chain control */
    Chain_Control *header;

    /* the index of the header */
    uint32_t header_index;

    /* the node iterator */
    Chain_Node *the_node;

    /* next node iterator */
    Chain_Node *next_node;

    /* previous node iterator */
    Chain_Node *previous_node;

    /* search node iterator */
    Chain_Node *search_node;

    /* the priority control */
    Priority_Control priority;

    /* the state control */
    States_Control block_state;

    /* synchronization state */
    Thread_queue_States sync_state;


    /* initialize the thread block chain */
    _Chain_Initialize_empty(&the_thread->Wait.Block2n);

    /* get the thread priority */
    priority = the_thread->current_priority;

    /* get the thread queue index for the thread priority */
    header_index = _Thread_queue_Header_number(priority);

    /* get the header for the thread queue thread priority */
    header = &the_thread_queue->Queues.Priority[ header_index ];

    /* get the state of the thread queue */
    block_state = the_thread_queue->state;

    /* check based on the priority if we should do a reverse search */
    if(_Thread_queue_Is_reverse_search(priority))
    {
        /* reverse search the thread queue */
        goto restart_reverse_search;
    }

    /* forward search */
restart_forward_search:

    /* initialize the search priority with an invalid number */
    search_priority = PRIORITY_MINIMUM - 1;

    /* disable interrupts */
    _ISR_Disable(level);

    /* start the search at the first node */
    search_thread = (Thread_Control *) header->first;

    /* iterate through the chain until the tail is reached */
    while(!_Chain_Is_tail(header , (Chain_Node *) search_thread))
    {
        /* set the search priority */
        search_priority = search_thread->current_priority;

        /* if the thread priority is higher than the search priority */
        if(priority <= search_priority)
        {
            /* then found the thread, stop searching */
            break;
        }

        /* if we should search two threads per cycle */
#if ( CPU_UNROLL_ENQUEUE_PRIORITY == TRUE )

        /* then check second thread in this cycle */

        /* update the search thread to the next thread on the chain */
        search_thread = (Thread_Control *) search_thread->Object.Node.next;

        /* if the thread is the tail */
        if(_Chain_Is_tail(header , (Chain_Node *) search_thread))
        {
            /* then we did not find the thread, stop the search */
            break;
        }

        /* get the priority value of the thread */
        search_priority = search_thread->current_priority;

        /* check the priority value (if lower) */
        if(priority <= search_priority)
        {
            /* then found the thread, stop searching */
            break;
        }

#endif
        /* still did not find the place to put the thread */

        /* allow interrupts to occur here */
        _ISR_Flash(level);

        /* an interrupt could have occurred here
         * and updated the search thread state or the thread queue state */

        /* check if the thread state is still the same as the thread queue state
         * (should only be different if an interrupt occurred) */
        if(!_States_Are_set(search_thread->current_state , block_state))
        {
            /* interrupt has changed state
             * restart reverse search */

            /* leave critical section */
            _ISR_Enable(level);

            /* reset the search priority
             * and restart search */
            goto restart_forward_search;
        }

        /* search the next thread */
        search_thread = (Thread_Control *) search_thread->Object.Node.next;
    }

    /* check if the thread queue state is nothing happened */
    if(the_thread_queue->sync_state != THREAD_QUEUE_NOTHING_HAPPENED)
    {
        /* if something happened, then sycnhronize */
        goto synchronize;
    }

    /* else, the thread queue is synchronized */
    the_thread_queue->sync_state = THREAD_QUEUE_SYNCHRONIZED;

    /* check if the priority is equal */
    if(priority == search_priority)
    {
        /* go to where the priority is equal case */
        goto equal_priority;
    }

    search_node = (Chain_Node *) search_thread;
    previous_node = search_node->previous;
    the_node = (Chain_Node *) the_thread;

    /* insert the node before the search thread */

    /* update the node next field */
    the_node->next = search_node;

    /* update the node previous field */
    the_node->previous = previous_node;

    /* update the previous node next field */
    previous_node->next = the_node;

    /* update the search node previous field */
    search_node->previous = the_node;

    /* update the thread wait queue to the specified thread queue */
    the_thread->Wait.queue = the_thread_queue;

    /* leave critical section */
    _ISR_Enable(level);

    /* and leave */
    return;

    /* reverse search */
restart_reverse_search:

    /* initialize the search priority with an invalid number */
    search_priority = PRIORITY_MAXIMUM + 1;

    /* enter critical section */
    _ISR_Disable(level);

    /* start the search at the last node */
    search_thread = (Thread_Control *) header->last;

    /* iterate through the chain until the head is reached */
    while(!_Chain_Is_head(header , (Chain_Node *) search_thread))
    {
        /* get the search thread priority */
        search_priority = search_thread->current_priority;

        /* if the thread priority is smaller than the search thread priority */
        if(priority >= search_priority)
        {
            /* then we found the thread, stop the search */
            break;
        }

        /* if we should search two threads per cycle */
#if ( CPU_UNROLL_ENQUEUE_PRIORITY == TRUE )

        /* then check second thread in this cycle */

        /* search next thread on the chain */
        search_thread = (Thread_Control *) search_thread->Object.Node.previous;

        /* if the search thread is the head of the chain */
        if(_Chain_Is_head(header , (Chain_Node *) search_thread))
        {
            /* then we did not find the thread, stop the search */
            break;
        }

        /* still not the head, check next node    */

        /* get the priority value of the search thread */
        search_priority = search_thread->current_priority;

        /* if the search thread priority is lower than the thread priority */
        if(priority >= search_priority)
        {
            /* then we found the thread, stop the search */
            break;
        }
#endif

        /* allow interrupts here */
        _ISR_Flash(level);

        /* an interrupt could have occurred here
         * and updated the search thread state or the thread queue state */

        /* check if the thread state is still the same as the thread queue state
         * (should only be different if an interrupt occurred) */
        if(!_States_Are_set(search_thread->current_state , block_state))
        {
            /* leave critical section */
            _ISR_Enable(level);

            /* restart reverse search */
            goto restart_reverse_search;
        }

        /* state did not change, continue search */

        /* get the previous thread (next iteration on the cycle) */
        search_thread = (Thread_Control *) search_thread->Object.Node.previous;
    }

    /* thread queue state was supposed to be nothing happened 
     * if it is not, then an interrupt has occurred */

    /* check the thread queue synchornization state */
    if(the_thread_queue->sync_state != THREAD_QUEUE_NOTHING_HAPPENED)
    {
        /* re-synchronize the thread queue */
        goto synchronize;
    }

    the_thread_queue->sync_state = THREAD_QUEUE_SYNCHRONIZED;

    /* check if the priority is equal */
    if(priority == search_priority)
    {
        /* go to where the priority is equal case */
        goto equal_priority;
    }

    /* insert the node after the search thread node */

    /* get the search thread node */
    search_node = (Chain_Node *) search_thread;

    /* get the node after the search thread node */
    next_node = search_node->next;

    /* get the thread node */
    the_node = (Chain_Node *) the_thread;

    /* update the node next field */
    the_node->next = next_node;

    /* update the node previous field */
    the_node->previous = search_node;

    /* update the search node node next field */
    search_node->next = the_node;

    /* update the search node previous field */
    next_node->previous = the_node;

    /* update the thread to insert thread queue */
    the_thread->Wait.queue = the_thread_queue;

    /* re-enable interrupts and leave */
    _ISR_Enable(level);

    /* and leave */
    return;

    /* equal priority */
equal_priority:

    /* add at end of priority group */

    /* the search thread is the last node on the chain */

    /* get the search thread node */
    search_node = _Chain_Tail(&search_thread->Wait.Block2n);

    /* get the search thread node previous node */
    previous_node = search_node->previous;

    /* get the thread node */
    the_node = (Chain_Node *) the_thread;

    /* insert the node before the tail */

    /* update the node next field */
    the_node->next = search_node;

    /* update the node previous field */
    the_node->previous = previous_node;

    /* update the previous node next field */
    previous_node->next = the_node;

    /* update the search node previous field */
    search_node->previous = the_node;

    /* update the thread to insert thread queue */
    the_thread->Wait.queue = the_thread_queue;

    /* re-enable interrupts and leave */
    _ISR_Enable(level);

    /* and leave */
    return;

    /* synchronize */
synchronize:

    /* get the thread queue synchronization state */
    sync_state = the_thread_queue->sync_state;

    /* and the set the state to synchronized */
    the_thread_queue->sync_state = THREAD_QUEUE_SYNCHRONIZED;

    /* check the old thread queue sync state */
    switch(sync_state)
    {
            /* thread queue timeout occured */
        case THREAD_QUEUE_TIMEOUT:
            /* thread did not get the resource it was blocking on */

            /* set the thread return code to the timeout status */
            the_thread->Wait.return_code = the_thread->Wait.queue->timeout_status;

            /* reset the thread wait queue (thread no longer waiting) */
            the_thread->Wait.queue = NULL;

            /* leave critical section */
            _ISR_Enable(level);

            /* and leave */
            break;

            /* thread queue is now satisfied */
        case THREAD_QUEUE_SATISFIED:
            /* thread got the resource it was blocking on */

            /* if there is an active watchdog */
            if(_Watchdog_Is_active(&the_thread->Timer))
            {
                /* then deactivate it */
                _Watchdog_Deactivate(&the_thread->Timer);

                /* reset the thread wait queue (thread no longer waiting) */
                the_thread->Wait.queue = NULL;

                /* re-enable interrupts */
                _ISR_Enable(level);

                /* and remove it */
                (void) _Watchdog_Remove(&the_thread->Timer);
            }
            else
            {
                /* re-enable interrupts */
                _ISR_Enable(level);
            }

            /* and leave */
            break;

            /* other state */
        default:

            /* and leave */
            break;

    }


    /* unblock the thread */
    _Thread_Unblock(the_thread);

#if defined(RTEMS_MULTIPROCESSING)
    /* global objects with thread queue's should not be operated on from an
     * ISR.  But the sync code still must allow short timeouts to be processed
     * correctly */
    if(!_Objects_Is_local_id(the_thread->Object.id))
    {
        _Thread_MP_Free_proxy(the_thread);
    }
#endif
}

/**  
 *  @}
 */

/**
 *  @}
 */
