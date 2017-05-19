/**
 *  @file
 *  threadsetstate.c
 *
 *  @brief set a thread state
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


void _Thread_Set_state(
                       Thread_Control *the_thread ,
                       States_Control state
                       )
{
    /* interrupt level */
    ISR_Level level;

    /* thread ready chain (of the same priority) */
    Chain_Control *ready;


    /* get the thread ready chain (of the same priority) */
    ready = the_thread->ready;

    /* enter critical section */
    _ISR_Disable(level);

    /* if the thread wasn't ready then */
    if(!_States_Is_ready(the_thread->current_state))
    {
        /* just update the state */
        the_thread->current_state = _States_Set(state , the_thread->current_state);

        /* leave critical section */
        _ISR_Enable(level);

        /* and return */
        return;
    }

    /* otherwise, update the state */
    the_thread->current_state = state;

    /* and update the thread ready queue */

    /* if the thread priority chain has only one node */
    if(_Chain_Has_only_one_node(ready))
    {
        /* then remove it */
        _Chain_Initialize_empty(ready);
        _Priority_Remove_from_bit_map(&the_thread->Priority_map);
    }
    else
    {
        /* otherwise just extract it, the thread priority still remains on the
         * priority bit map */
        _Chain_Extract_unprotected(&the_thread->Object.Node);
    }

    /* allow an interrupt to occur */
    _ISR_Flash(level);

    /* if the heir thread has not been updated, then update it now */
    if(_Thread_Is_heir(the_thread))
    {
        /* calculate the heir thread */
        _Thread_Calculate_heir();
    }

    /* decide if a context switch is necessary */
    if(_Thread_Is_executing(the_thread))
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
