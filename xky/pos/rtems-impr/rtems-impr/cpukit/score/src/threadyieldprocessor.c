/**
 *  @file
 *  threadyieldprocessor.c
 *
 *  @brief the thread yields the processor to another thread
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


void _Thread_Yield_processor(void)
{
    /* interrupt level */
    ISR_Level level;

    /* executing thread */
    Thread_Control *executing;

    /* thread ready chain (of the same priority) */
    Chain_Control *ready;


    /* get the executing thread */
    executing = _Thread_Executing;

    /* get the thread chain of threads with the same priority */
    ready = executing->ready;

    /* enter critical section */
    _ISR_Disable(level);

    /* if the thread priority chain has more than one node */
    if(!_Chain_Has_only_one_node(ready))
    {
        /* then the next thread on the chain is the heir thread */

        /* extract the execting thread from the chain */
        _Chain_Extract_unprotected(&executing->Object.Node);

        /* and place it at the end of the chain */
        _Chain_Append_unprotected(ready , &executing->Object.Node);

        /* allow an interrupt to occur */
        _ISR_Flash(level);

        /* if the heir thread has not been updated (to a higher priority thread) */
        if(_Thread_Is_heir(executing))
        {
            /* then calculate it has the first thread on the thread ready list */
            _Thread_Heir = (Thread_Control *) ready->first;
        }

        /* context switch is necessary now */
        _Context_Switch_necessary = TRUE;
    }
        /* check if the heir thread has not been updated (to a higher priority thread) */
    else if(!_Thread_Is_heir(executing))
    {
        /* context switch is necessary now */
        _Context_Switch_necessary = TRUE;
    }

    /* else, nothing needs to be done, there are no other threads of the same
     * priority so no need to change the running thread */

    /* leave critical section */
    _ISR_Enable(level);
}

/**  
 *  @}
 */

/**
 *  @}
 */
