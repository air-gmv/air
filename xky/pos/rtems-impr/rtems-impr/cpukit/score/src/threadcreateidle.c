/**
 *  @file
 *  threadcreateidle.c
 *
 *  @brief create an idle thread
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
 *  4429    | 21/09/2009  | mcoutinho    | IPR 685
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

/* idle thread name */
const char *_Thread_Idle_name = "IDLE";


void _Thread_Create_idle(void)
{
    /* idle thread entry point */
    void *idle;

    /* idle thread stack size */
    uint32_t idle_task_stack_size;


    /* the entire workspace is zeroed during its initialization.  Thus, all
     * fields not explicitly assigned were explicitly zeroed by
     * _Workspace_Initialization */

    /* allocate space for the idle thread */
    _Thread_Idle = _Thread_Internal_allocate();

    /* initialize the idle thread */

    /* if the CPU has the idle thread entry point */
#if (CPU_PROVIDES_IDLE_THREAD_BODY == TRUE)

    /* idle thread body is the CPU/BSP specific body */
    idle = (void *) _CPU_Thread_Idle_body;

#else

    /* idle thread body is the RTEMS default */
    idle = (void *) _Thread_Idle_body;

#endif

    /* or, if the CPU table contains a idle thread, then override it */
    if(_CPU_Table.idle_task)
    {
        /* override the idle thread entry point */
        idle = _CPU_Table.idle_task;
    }

    /* the idle thread stack corresponds to the size on the CPU table */
    idle_task_stack_size = _CPU_Table.idle_task_stack_size;

    /* check the stack and place at least the minimum size */
    if(idle_task_stack_size < STACK_MINIMUM_SIZE)
    {
        /* set the stack size to the minimum allowed */
        idle_task_stack_size = STACK_MINIMUM_SIZE;
    }

    /* initialize the idle thread */
    _Thread_Initialize(&_Thread_Internal_information ,
                       _Thread_Idle ,
                       idle_task_stack_size ,
                       CPU_IDLE_TASK_IS_FP ,
                       PRIORITY_MAXIMUM ,
                       TRUE , /* preemptable */
                       THREAD_CPU_BUDGET_ALGORITHM_NONE ,
                       NULL , /* no budget algorithm callout */
                       0 , /* all interrupts enabled */
                       (Objects_Name) _Thread_Idle_name);

    /* WARNING!!! This is necessary to "kick" start the system and
     *            MUST be done before _Thread_Start is invoked */

    /* set the executing thread */
    _Thread_Executing = _Thread_Idle;

    /* set the heir thread */
    _Thread_Heir = _Thread_Executing;

    /* start the idle thread with:
     *   the idle thread
     *   the numeric type of thread
     *   the entry point
     *   no pointer argument
     *   no mumeric argument */
    _Thread_Start(_Thread_Idle ,
                  THREAD_START_NUMERIC ,
                  idle ,
                  NULL ,
                  0);

}

/**  
 *  @}
 */

/**
 *  @}
 */
