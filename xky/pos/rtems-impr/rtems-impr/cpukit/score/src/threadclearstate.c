/**
 *  @file
 *  threadclearstate.c
 *
 *  @brief clear the state of a thread. the thread ready queue is adjust if
 *  needed.
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


void _Thread_Clear_state(
                         Thread_Control *the_thread ,
                         States_Control state
                         )
{
    /* interrupt lelel */
    ISR_Level level;

    /* thread current state */
    States_Control current_state;


    /* enter critical section */
    _ISR_Disable(level);

    /* get the thread state */
    current_state = the_thread->current_state;

    /* if the thread state contains the state to clear */
    if(current_state & state)
    {
        /* then change the thread state */
        the_thread->current_state = _States_Clear(state , current_state);

        /* get the thread state */
        current_state = the_thread->current_state;

        /* if the thread is now ready */
        if(_States_Is_ready(current_state))
        {
            /* add the thread priority to the priority bit map */
            _Priority_Add_to_bit_map(&the_thread->Priority_map);

            /* append the thread to the ready queue */
            _Chain_Append_unprotected(the_thread->ready , &the_thread->Object.Node);

            /* allow interrrupts here */
            _ISR_Flash(level);

            /* if the thread that was unblocked is more important than the heir,
             * then we have a new heir.  In addition, if the current thread
             * is preemptible or we are waking up one of the "pseudo-ISR" system
             * threads, then we need to do a context switch */
            if(the_thread->current_priority < _Thread_Heir->current_priority)
            {
                /* change the heir thread */
                _Thread_Heir = the_thread;

                /* check if can change the running thread */
                if(_Thread_Executing->is_preemptible ||
                   the_thread->current_priority == 0)
                {
                    /* can change the running thread */
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
