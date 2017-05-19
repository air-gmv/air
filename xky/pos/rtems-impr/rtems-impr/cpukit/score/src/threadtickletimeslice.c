/**
 *  @file
 *  threadtickletimeslice.c
 *
 *  @brief announce the passage of a clock tick to update the thread timeslice
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
 *  624     | 17/11/2008  | mcoutinho    | IPR 70
 *  1887    | 13/03/2009  | mcoutinho    | IPR 57
 *  3552    | 29/06/2009  | mcoutinho    | IPR 548
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
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


void _Thread_Tickle_timeslice(void)
{
    /* executing thread */
    Thread_Control *executing;


    /* get the executing thread */
    executing = _Thread_Executing;

    /* if the thread is not preemptible */
    if(!executing->is_preemptible)
    {
        /* then just return */
        return;
    }

    /* if the thread is not ready then just return */
    if(!_States_Is_ready(executing->current_state))
    {
        /* then just return */
        return;
    }

    /* tThe cpu budget algorithm determines what happens next */

    /* check the thread budget algorithm */
    switch(executing->budget_algorithm)
    {

            /* no budget algorithm */
        case THREAD_CPU_BUDGET_ALGORITHM_NONE:

            /* just leave */
            break;

            /* reset at timeslice algorithm */
        case THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE:

            /* decrease the time budget and check if it has finished */
            if((int) ( --executing->cpu_time_budget ) <= 0)
            {
                /* reset the timeslice since the budget has finished */
                _Thread_Reset_timeslice();

                /* set the executing thread budget to its original value */
                executing->cpu_time_budget = _Thread_Ticks_per_timeslice;
            }

            /* and leave */
            break;

            /* invalid thread bugdet algorithm */
        default:

            /* raise an internal error */
            _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                     TRUE ,
                                     INTERNAL_ERROR_INVALID_THREAD_BUDGET_ALGORITHM);
    }
}

/**
 *  @}
 */

/**
 *  @}
 */
