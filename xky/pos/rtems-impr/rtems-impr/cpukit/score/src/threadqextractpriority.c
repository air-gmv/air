/**
 *  @file
 *  threadqextractpriority.c
 *
 *  @brief remove a thread from a priority thread queue
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


void _Thread_queue_Extract_priority_helper(
                                           Thread_queue_Control *the_thread_queue ,
                                           Thread_Control *the_thread ,
                                           boolean requeuing
                                           )
{
    /* interrupt level */
    ISR_Level level;

    /* node of the thread */
    Chain_Node *the_node;

    /* next node of the thread */
    Chain_Node *next_node;

    /* previous node of the thread */
    Chain_Node *previous_node;

    /* new first thread */
    Thread_Control *new_first_thread;

    /* new first thread node */
    Chain_Node *new_first_node;

    /* new second thread node */
    Chain_Node *new_second_node;

    /* last node */
    Chain_Node *last_node;


    /* get the thread node (is itself) */
    the_node = (Chain_Node *) the_thread;

    /* enter critical section */
    _ISR_Disable(level);

    /* check if the thread is waiting on a thread queue */
    if(!_States_Is_waiting_on_thread_queue(the_thread->current_state))
    {
        /* is not, so leave critical section */
        _ISR_Enable(level);

        /* and return */
        return;
    }

    /* the thread was actually waiting on a thread queue so let's remove it */

    /* get the next node */
    next_node = the_node->next;

    /* get the previous node */
    previous_node = the_node->previous;

    /* check if the thread wait chain is not empty */
    if(!_Chain_Is_empty(&the_thread->Wait.Block2n))
    {
        /* is not, so have to remove the thread */

        new_first_node = the_thread->Wait.Block2n.first;
        new_first_thread = (Thread_Control *) new_first_node;
        last_node = the_thread->Wait.Block2n.last;
        new_second_node = new_first_node->next;

        previous_node->next = new_first_node;
        next_node->previous = new_first_node;
        new_first_node->next = next_node;
        new_first_node->previous = previous_node;

        /* if the chain has more than one node */
        if(!_Chain_Has_only_one_node(&the_thread->Wait.Block2n))
        {
            /* then, */
            /* > two threads on 2-n */
            new_second_node->previous = _Chain_Head(&new_first_thread->Wait.Block2n);
            new_first_thread->Wait.Block2n.first = new_second_node;

            new_first_thread->Wait.Block2n.last = last_node;
            last_node->next = _Chain_Tail(&new_first_thread->Wait.Block2n);
        }
    }
    /* thread wait chain is empty */
    else
    {
        /* thread wait chain is empty */
        previous_node->next = next_node;
        next_node->previous = previous_node;
    }

    /* if we are not supposed to touch timers or the thread's state, return */
    if(requeuing)
    {
        /* leave critical section and return */
        _ISR_Enable(level);

        /* and return */
        return;
    }

    /* check if there was an active watchdog */
    if(!_Watchdog_Is_active(&the_thread->Timer))
    {
        /* no, then just re-enable interrupts */
        _ISR_Enable(level);
    }
    else
    {
        /* deactivate the watchdog */
        _Watchdog_Deactivate(&the_thread->Timer);

        /* re-enable interrupts */
        _ISR_Enable(level);

        /* and remove the watchdog */
        (void) _Watchdog_Remove(&the_thread->Timer);
    }

    /* unblock the thread */
    _Thread_Unblock(the_thread);

#if defined(RTEMS_MULTIPROCESSING)
    
    /* if thread is not local */
    if(!_Objects_Is_local_id(the_thread->Object.id))
    {
        /* then extract it remotely */
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
