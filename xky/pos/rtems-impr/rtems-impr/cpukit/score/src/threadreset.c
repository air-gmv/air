/**
 *  @file
 *  threadreset.c
 *
 *  @brief reset a thread to its initial state.
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
 *  3773    | 13/07/2009  | mcoutinho    | IPR 553
 *  3912    | 21/07/2009  | mcoutinho    | IPR 99
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


void _Thread_Reset(
                   Thread_Control *the_thread ,
                   void *pointer_argument ,
                   uint32_t numeric_argument
                   )
{
    /* free all mutexes hold by the thread */
    _Thread_Free_mutexes(the_thread);

    /* reset the resource count (thread does not hold any resource) */
    the_thread->resource_count = 0;

    /* reset suspend count (thread is not suspended) */
    the_thread->suspend_count = 0;

    /* reset the preemption mode */
    the_thread->is_preemptible = the_thread->Start.is_preemptible;

    /* reset the budget algorithm */
    the_thread->budget_algorithm = the_thread->Start.budget_algorithm;

    /* reset the budget callout function */
    the_thread->budget_callout = the_thread->Start.budget_callout;

    /* set the start information pointer argument */
    the_thread->Start.pointer_argument = pointer_argument;

    /* set the start information numberic argument */
    the_thread->Start.numeric_argument = numeric_argument;

    /* extract the thread from the thread queue */
    if(!_Thread_queue_Extract_with_proxy(the_thread))
    {
        /* check if the watchdog is active */
        if(_Watchdog_Is_active(&the_thread->Timer))
        {
            /*if it is, then remove it */
            (void) _Watchdog_Remove(&the_thread->Timer);
        }
    }

    /* check if the thread has changed its priority */
    if(the_thread->current_priority != the_thread->Start.initial_priority)
    {
        /* it has, so has to reset the priority to the initial value */
        the_thread->real_priority = the_thread->Start.initial_priority;

        /* set the initial thread priority */
        _Thread_Set_priority(the_thread , the_thread->Start.initial_priority);
    }
}

/**  
 *  @}
 */

/**
 *  @}
 */
