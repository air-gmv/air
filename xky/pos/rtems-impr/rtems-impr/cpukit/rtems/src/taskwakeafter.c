/**
 *  @file
 *  taskwakeafter.c
 *
 *  @brief sleep a task for a specified number of clock ticks
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
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  7124    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
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


rtems_status_code rtems_task_wake_after(
                                        rtems_interval ticks
                                        )
{
    /* disable thread dispatch */
    _Thread_Disable_dispatch();

    /* if ticks is zero then */
    if(ticks == 0)
    {
        /* yield the CPU to another thread of equal priority (if any) */
        _Thread_Yield_processor();
    }
    else
    {
        /* set the executing thread state to "delaying" */
        _Thread_Set_state(_Thread_Executing , STATES_DELAYING);

        /* initialize the watchdog */
        _Watchdog_Initialize(&_Thread_Executing->Timer ,
                             _Thread_Delay_ended ,
                             _Thread_Executing->Object.id ,
                             NULL);

        /* and insert the watchdog to the clock ticks chain */
        _Watchdog_Insert_ticks(&_Thread_Executing->Timer , ticks);
    }

    /* enable thread dispatch */
    _Thread_Enable_dispatch();

    /* return success */
    return RTEMS_SUCCESSFUL;
}

/**  
 *  @}
 */

/**
 *  @}
 */
