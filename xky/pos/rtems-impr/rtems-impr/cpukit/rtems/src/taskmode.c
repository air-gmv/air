/**
 *  @file
 *  taskmode.c
 *
 *  @brief set or get the task mode
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  585     | 17/11/2008  | mcoutinho    | IPR 69
 *  1886    | 13/03/2009  | mcoutinho    | IPR 57
 *  5247    | 30/10/2009  | mcoutinho    | IPR 828
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  7114    | 09/04/2010  | mcoutinho    | IPR 1931
 *  9293    | 03/01/2011  | mcoutinho    | IPR 451
 *  9293    | 03/01/2011  | mcoutinho    | IPR 2814
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_TASK Task Manager
 *  @{
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/modes.h>
#include <rtems/score/object.h>
#include <rtems/score/stack.h>
#include <rtems/score/states.h>
#include <rtems/rtems/tasks.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tod.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>
#include <rtems/score/sysstate.h>
#include <rtems/rtems/intr.h>


rtems_status_code rtems_task_mode(
                                  rtems_mode mode_set ,
                                  rtems_mode mask ,
                                  rtems_mode *previous_mode_set
                                  )
{
    /* task to change the mode */
    Thread_Control *executing;

    /* the task to change the mode API */
    RTEMS_API_Control *api;

    /* the task to change the old mode*/
    rtems_mode old_mode;

    /* interrupt level */
    rtems_interrupt_level level;


    /* check if previous mode set address is valid */
    if(!previous_mode_set)
    {
        /* return invalid address */
        return RTEMS_INVALID_ADDRESS;
    }

    /* get the old mode */

    /* the task to change the mode is the calling task */
    executing = _Thread_Executing;

    /* get the task API */
    api = executing->API_Extensions[ THREAD_API_RTEMS ];

    /* check the preempt mode */
    if(executing->is_preemptible)
    {
        /* old mode is preempt */
        old_mode = RTEMS_PREEMPT;
    }
    else
    {
        /* old mode is no-preempt */
        old_mode = RTEMS_NO_PREEMPT;
    }

    /* check the executing thread budget algorithm */
    if(executing->budget_algorithm == THREAD_CPU_BUDGET_ALGORITHM_NONE)
    {
        /* old mode is no-timeslice */
        old_mode |= RTEMS_NO_TIMESLICE;
    }
    else
    {
        /* old mode is timeslice */
        old_mode |= RTEMS_TIMESLICE;
    }

    /* check the isr level */
    old_mode |= _ISR_Get_level();

    /* set the previous mode */
    *previous_mode_set = old_mode;


    /* set the new mode */

    /* set the new preempt mode */
    if(mask & RTEMS_PREEMPT_MASK)
    {
        /* if modes is preempt */
        if(_Modes_Is_preempt(mode_set))
        {
            /* set the mode to preempt */
            executing->is_preemptible = TRUE;
        }
        else
        {
            /* set the mode to no-preempt */
            executing->is_preemptible = FALSE;
        }
    }

    /* set the new timeslice mode */
    if(mask & RTEMS_TIMESLICE_MASK)
    {
        /* if modes is timeslice */
        if(_Modes_Is_timeslice(mode_set))
        {
            /* set the mode to timeslice */
            executing->budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE;

            /* reset the task budget */
            executing->cpu_time_budget = _Thread_Ticks_per_timeslice;
        }
        else
        {
            /* set the mode to no-timeslice */
            executing->budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_NONE;
        }
    }

    /* set the new interrupt level */
    if(mask & RTEMS_INTERRUPT_MASK)
    {
        /* set the interrupt mode */
        _Modes_Set_interrupt_level(mode_set);
    }

    /* check if the system state is up (could be inside a device driver) */
    if(_System_state_Is_up(_System_state_Current))
    {
        /* enter critical section to protect _Thread_Heir variable */
        rtems_interrupt_disable(level);

        /* evaluate whether a context switch is necessary */
        if(_Thread_Evaluate_mode())
        {
            /* leave critical section */
            rtems_interrupt_enable(level);

            /* dispatch threads */
            _Thread_Dispatch();
        }
        else
        {
            /* leave critical section */
            rtems_interrupt_enable(level);
        }
    }

    /* return success */
    return RTEMS_SUCCESSFUL;
}

/**  
 *  @}
 */

/**
 *  @}
 */
