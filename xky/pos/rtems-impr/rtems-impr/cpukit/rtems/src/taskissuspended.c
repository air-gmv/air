/**
 *  @file
 *  taskissuspended.c
 *
 *  @brief determine if a task is suspended
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
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  7113    | 09/04/2010  | mcoutinho    | IPR 1931
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


rtems_status_code rtems_task_is_suspended(
                                          Objects_Id id
                                          )
{
    /* the task to determine if it is suspended */
    register Thread_Control *the_thread;

    /* the task to determine if it is suspended location */
    Objects_Locations location;


    /* get the thread */
    the_thread = _Thread_Get(id , &location);

    /* check the task location */
    switch(location)
    {

#if defined(RTEMS_MULTIPROCESSING)

            /* task is remote */
        case OBJECTS_REMOTE:

            /* dispatch tasks */
            _Thread_Dispatch();
            /* return invalid operation (cannot determine if a remote task is
             * suspended */
            return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;

#endif

            /* task id is invalid */
        case OBJECTS_ERROR:

            /* return invalid identifier error */
            return RTEMS_INVALID_ID;

            /* task is local (nominal case) */
        case OBJECTS_LOCAL:

            /* check the thread state */
            if(!_States_Is_suspended(the_thread->current_state))
            {
                /* enable dispacth (disabled by _Thread_Get) */
                _Thread_Enable_dispatch();

                /* if the task is not suspended return successful */
                return RTEMS_SUCCESSFUL;
            }
            else
            {
                /* enable dispacth (disabled by _Thread_Get) */
                _Thread_Enable_dispatch();

                /* if the thread is already suspended return the proper value */
                return RTEMS_ALREADY_SUSPENDED;
            }

            /* default clause: the object location is invalid */
        default:
            /* an internal error occured and the object location is invalid */
            return RTEMS_INTERNAL_INVALID_OBJECT_LOCATION;
    }
}

/**  
 *  @}
 */

/**
 *  @}
 */
