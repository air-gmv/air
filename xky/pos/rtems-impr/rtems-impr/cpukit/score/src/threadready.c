/**
 *  @file
 *  threadready.c
 *
 *  @brief readies a thread and make the necessary adjustments to the thread
 *  ready queue
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


void _Thread_Ready(
                   Thread_Control *the_thread
                   )
{
    /* interrupt level */
    ISR_Level level;

    /* heir thread */
    Thread_Control *heir;


    /* enter critical section */
    _ISR_Disable(level);

    /* place the correct state on the thread */
    the_thread->current_state = STATES_READY;

    /* add the thread priority bit map to the general priority bit map */
    _Priority_Add_to_bit_map(&the_thread->Priority_map);

    /* append a node to the chain */
    _Chain_Append_unprotected(the_thread->ready , &the_thread->Object.Node);

    /* allow an interrupt to occur here */
    _ISR_Flash(level);

    /* calculate the highest priority thread on the thread ready list */
    _Thread_Calculate_heir();

    /* get heir thread */
    heir = _Thread_Heir;

    /* check if a context switch is necessary */
    if(!_Thread_Is_executing(heir) && _Thread_Executing->is_preemptible)
    {
        /* context switch is needed */
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
