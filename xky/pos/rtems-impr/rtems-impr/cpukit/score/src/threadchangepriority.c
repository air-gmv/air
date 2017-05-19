/**
 *  @file
 *  threadchangepriority.c
 *
 *  @brief change the priority of a thread
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
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
 *  @addtogroup ScoreThread Thread Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/apiext.h>
#include <rtems/score/context.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/states.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>


void _Thread_Change_priority(
                             Thread_Control *the_thread ,
                             Priority_Control new_priority ,
                             boolean prepend_it
                             )
{
    /* interrupt level */
    ISR_Level level;

    /* thread state */
    States_Control state;


    /* set a transient state for the thread so it is pulled off the Ready chains.
     * This will prevent it from being scheduled no matter what happens in an
     * ISR. */
    _Thread_Set_transient(the_thread);

    /* do not bother recomputing all the priority related information if
     * we are not REALLY changing priority */
    if(the_thread->current_priority != new_priority)
    {
        /* if the priorities are different, then change the priority */
        _Thread_Set_priority(the_thread , new_priority);
    }

    /* enter critical section */
    _ISR_Disable(level);

    /* if the thread has more than STATES_TRANSIENT set, then it is blocked,
     * if it is blocked on a thread queue, then we need to requeue it */

    /* get the thread state */
    state = the_thread->current_state;

    /* if the thread state is not transient */
    if(state != STATES_TRANSIENT)
    {
        /* then the thread is also blocked */

        /* clear the thread state transient state */
        the_thread->current_state = _States_Clear(STATES_TRANSIENT , state);

        /* leave critical section */
        _ISR_Enable(level);

        /* if the thread was waiting on a queue */
        if(_States_Is_waiting_on_thread_queue(state))
        {
            /* requeue the thread to update the priority */
            _Thread_queue_Requeue(the_thread->Wait.queue , the_thread);
        }

        /* and leave */
        return;
    }

    /* interrupts are STILL disabled.
     * We now know the thread will be in the READY state when we remove
     * the TRANSIENT state.  So we have to place it on the appropriate
     * Ready Queue with interrupts off */

    /* clear the thread state transient state */
    the_thread->current_state = _States_Clear(STATES_TRANSIENT , state);

    /* thread is now is ready state */

    /* add the priority to the bit map */
    _Priority_Add_to_bit_map(&the_thread->Priority_map);

    /* decide whether to prepend or append to the list of ready tasks on the chain
     * of the new priority */

    /* if prepend */
    if(prepend_it)
    {
        /* then prepend the thread */
        _Chain_Prepend_unprotected(the_thread->ready , &the_thread->Object.Node);
    }
    else
    {
        /* else append the thread */
        _Chain_Append_unprotected(the_thread->ready , &the_thread->Object.Node);
    }

    /* allow interrupts here */
    _ISR_Flash(level);

    /* we altered the set of thread priorities.  So let's figure out
     * who is the heir and if we need to switch to them */
    _Thread_Calculate_heir();

    /* decide if a context switch is necessary */
    if(!_Thread_Is_executing_also_the_heir() &&
       _Thread_Executing->is_preemptible)
    {
        /* context switch is necessary */
        _Context_Switch_necessary = TRUE;
    }

    /* leave critical section */
    _ISR_Enable(level);
}

/**  
 *  @}
 */

/**
 *  @}
 */
