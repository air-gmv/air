/**
 *  @file
 *  tasksetpriority.c
 *
 *  @brief set/get a task priority
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
 *  7119    | 09/04/2010  | mcoutinho    | IPR 1931
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


rtems_status_code rtems_task_set_priority(
                                          Objects_Id id ,
                                          rtems_task_priority new_priority ,
                                          rtems_task_priority *old_priority
                                          )
{
    /* task to set the priority */
    register Thread_Control *the_thread;

    /* task to set the priority location */
    Objects_Locations location;


    /* check if the new priority is valid */
    if(new_priority != RTEMS_CURRENT_PRIORITY &&
       !_RTEMS_tasks_Priority_is_valid(new_priority))
    {
        /* return invalid priority */
        return RTEMS_INVALID_PRIORITY;
    }

    /* check the address of the old priority */
    if(!old_priority)
    {
        /* return invalid address error */
        return RTEMS_INVALID_ADDRESS;
    }

    /* get the thread */
    the_thread = _Thread_Get(id , &location);

    /* check the task location */
    switch(location)
    {
#if defined(RTEMS_MULTIPROCESSING)

            /* task is remote */
        case OBJECTS_REMOTE:

            /* set the task remote argument to the old priority */
            _Thread_Executing->Wait.return_argument = old_priority;

            /* send a packet to the remote node */
            return _RTEMS_tasks_MP_Send_request_packet(RTEMS_TASKS_MP_SET_PRIORITY_REQUEST ,
                                                       id ,
                                                       new_priority ,
                                                       0 , /* Not used */
                                                       0 /* Not used */);
#endif

            /* task id is invalid */
        case OBJECTS_ERROR:

            /* return invalid identifier error */
            return RTEMS_INVALID_ID;

            /* task is local (nominal case) */
        case OBJECTS_LOCAL:

            /* get the old priority */
            *old_priority = the_thread->current_priority;

            /* check if the priority is to be changed */
            if(new_priority != RTEMS_CURRENT_PRIORITY)
            {
                /* change the TCB priority */
                the_thread->real_priority = new_priority;

                /* change priority if the thread does not hold resources (mutexes)
                 * or, if the thread holds resources, change the priority only to
                 * increase it */
                if(the_thread->resource_count == 0 ||
                   the_thread->current_priority > new_priority)
                {
                    /* change thread priority to new priority */
                    _Thread_Change_priority(the_thread , new_priority , FALSE);
                }
            }

            /* enable dispacth (disabled by _Thread_Get) */
            _Thread_Enable_dispatch();

            /* return success */
            return RTEMS_SUCCESSFUL;

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
