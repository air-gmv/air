/**
 *  @file
 *  taskresume.c
 *
 *  @brief resume a task
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
 *  7116    | 09/04/2010  | mcoutinho    | IPR 1931
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


rtems_status_code rtems_task_resume(
                                    Objects_Id id
                                    )
{
    /* task to resume */
    register Thread_Control *the_thread;

    /* task to resume location */
    Objects_Locations location;


    /* get the thread */
    the_thread = _Thread_Get(id , &location);

    /* check the task location */
    switch(location)
    {
#if defined(RTEMS_MULTIPROCESSING)

            /* task is remote */
        case OBJECTS_REMOTE:

            /* send a packet to the remote node */
            return _RTEMS_tasks_MP_Send_request_packet(RTEMS_TASKS_MP_RESUME_REQUEST ,
                                                       id ,
                                                       0 , /* Not used */
                                                       0 , /* Not used */
                                                       0); /* Not used */

#endif
            
            /* task id is invalid */
        case OBJECTS_ERROR:

            /* return invalid identifier error */
            return RTEMS_INVALID_ID;

            /* task is local (nominal case) */
        case OBJECTS_LOCAL:

            /* determine if the thread is already suspended */
            if(_States_Is_suspended(the_thread->current_state))
            {
                /* task is suspended */

                /* resume the thread */
                _Thread_Resume(the_thread , TRUE);

                /* enable dispacth (disabled by _Thread_Get) */
                _Thread_Enable_dispatch();

                /* return success */
                return RTEMS_SUCCESSFUL;
            }
            else
            {
                /* task is not suspended */

                /* enable dispacth (disabled by _Thread_Get) */
                _Thread_Enable_dispatch();

                /* return incorrect task state */
                return RTEMS_INCORRECT_STATE;
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
