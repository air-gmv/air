/**
 *  @file
 *  threadsuspend.c
 *
 *  @brief suspend a thread
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


void _Thread_Suspend(
                     Thread_Control *the_thread
                     )
{
    /* interrupt level */
    ISR_Level level;

    /* thread ready chain (of the same priority) */
    Chain_Control *ready;


    /* get the thread chain of threads with the same priority */
    ready = the_thread->ready;

    /* disable interrupts */
    _ISR_Disable(level);

    /* increase the thread suspend count */
    the_thread->suspend_count++;

    /* if the thread was not ready */
    if(!_States_Is_ready(the_thread->current_state))
    {
        /* then just add the thread state with suspended */
        the_thread->current_state = _States_Set(STATES_SUSPENDED ,
                                                the_thread->current_state);

        /* re-enable interrupts and return */
        _ISR_Enable(level);

        /* and leave */
        return;
    }

    /* if the thread was ready, then set the state to suspended */
    the_thread->current_state = STATES_SUSPENDED;

    /* check if there are threads with equal priority */
    if(_Chain_Has_only_one_node(ready))
    {
        /* if there are not, then initialize the chain to empty */
        _Chain_Initialize_empty(ready);

        /* and remove the thread priority from the priority bit map */
        _Priority_Remove_from_bit_map(&the_thread->Priority_map);
    }
    else
    {
        /* if there are, then just extract the thread from the chain */
        _Chain_Extract_unprotected(&the_thread->Object.Node);
    }

    /* allow interrupts to occur here */
    _ISR_Flash(level);

    /* calculate the heir thread */

    /* if the heir was the thread itself */
    if(_Thread_Is_heir(the_thread))
    {
        /* then recalculate the heir (thread has left ready state) */
        _Thread_Calculate_heir();
    }

    /* if the thread executing was the one suspended */
    if(_Thread_Is_executing(the_thread))
    {
        /* the a context switch is necessary */
        _Context_Switch_necessary = TRUE;
    }

    /* re-enable interrupts */
    _ISR_Enable(level);
}

/**  
 *  @}
 */

/**
 *  @}
 */
