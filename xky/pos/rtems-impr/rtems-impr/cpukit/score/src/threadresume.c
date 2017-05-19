/**
 *  @file
 *  threadresume.c
 *
 *  @brief resume a thread
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  3541    | 29/06/2009  | mcoutinho    | IPR 534
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


void _Thread_Resume(
                    Thread_Control *the_thread ,
                    boolean force
                    )
{
    /* interrupt level */
    ISR_Level level;

    /* thread current state */
    States_Control current_state;


    /* enter critical section */
    _ISR_Disable(level);

    /* reset the thread suspend count */
    the_thread->suspend_count = 0;

    /* get the thread current state */
    current_state = the_thread->current_state;

    /* check if the thread was suspended */
    if(current_state & STATES_SUSPENDED)
    {
        /* if was suspended, then */

        /* the current thread state is no longer suspended */
        current_state = the_thread->current_state = _States_Clear(STATES_SUSPENDED ,
                                                                  current_state);

        /* if the state is ready (could also be blocked) */
        if(_States_Is_ready(current_state))
        {
            /* then add the priority to the priority bit map */
            _Priority_Add_to_bit_map(&the_thread->Priority_map);

            /* append the thread to the ready list */
            _Chain_Append_unprotected(the_thread->ready , &the_thread->Object.Node);

            /* allow interrupts here */
            _ISR_Flash(level);

            /* check if the thread priority is higher than the heir thread */
            if(the_thread->current_priority < _Thread_Heir->current_priority)
            {
                /* if so, change the heir thread */
                _Thread_Heir = the_thread;

                /* check if the running thread mode allows a context switch */
                if(_Thread_Executing->is_preemptible ||
                   the_thread->current_priority == 0)
                {
                    /* context switch is needed */
                    _Context_Switch_necessary = TRUE;
                }
            }
        }
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
