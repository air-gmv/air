/**
 *  @file
 *  taskwakewhen.c
 *
 *  @brief wake a task at a given time wall
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
#include <rtems/rtems/clock.h>
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


rtems_status_code rtems_task_wake_when(
                                       rtems_time_of_day *time_buffer
                                       )
{
    /* number of seconds since epoch to wake up the task */
    Watchdog_Interval seconds;


    /* check if the time of day has been set */
    if(!_TOD_Is_set)
    {
        /* return not defined error */
        return RTEMS_NOT_DEFINED;
    }

    /* check the address of the time buffer */
    if(!time_buffer)
    {
        /* if NULL, return invalid address error */
        return RTEMS_INVALID_ADDRESS;
    }

    /* initialize the time buffer ticks to zero (wake when only has second
     * resolution) */
    time_buffer->ticks = 0;

    /* check if time buffer has a valid date */
    if(!_TOD_Validate(time_buffer))
    {
        /* if not valid, return invalid clock error */
        return RTEMS_INVALID_CLOCK;
    }

    /* convert the time buffer to seconds */
    seconds = _TOD_To_seconds(time_buffer);

    /* if seconds is too low */
    if(seconds <= _TOD_Seconds_since_epoch)
    {
        /* return invalid clock error */
        return RTEMS_INVALID_CLOCK;
    }

    /* disable thread dispatching */
    _Thread_Disable_dispatch();

    /* set the thread state to waiting for time */
    _Thread_Set_state(_Thread_Executing , STATES_WAITING_FOR_TIME);

    /* initialize the watchdog */
    _Watchdog_Initialize(&_Thread_Executing->Timer ,
                         _Thread_Delay_ended ,
                         _Thread_Executing->Object.id ,
                         NULL);

    /* and insert it on the watchdog seconds chain */
    _Watchdog_Insert_seconds(&_Thread_Executing->Timer ,
                             seconds - _TOD_Seconds_since_epoch);

    /* re-enable thread dispatch */
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
