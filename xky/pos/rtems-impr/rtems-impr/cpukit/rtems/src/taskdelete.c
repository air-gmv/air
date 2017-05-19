/**
 *  @file
 *  taskdelete.c
 *
 *  @brief delete a task
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
 *  4343    | 15/09/2009  | mcoutinho    | IPR 553
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  7109    | 09/04/2010  | mcoutinho    | IPR 1931
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


rtems_status_code rtems_task_delete(
                                    Objects_Id id
                                    )
{
    /* the task to delete */
    register Thread_Control *the_thread;

    /* the task to delete location */
    Objects_Locations location;

    /* the task to delete object information */
    Objects_Information *the_information;


    /* get the thread */
    the_thread = _Thread_Get(id , &location);

    /* check the task location */
    switch(location)
    {
#if defined(RTEMS_MULTIPROCESSING)

            /* task is remote */
        case OBJECTS_REMOTE:

            /* dispatch the tasks */
            _Thread_Dispatch();

            /* return error (cannot delete a remote task) */
            return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

            /* task id is invalid */
        case OBJECTS_ERROR:

            /* return invalid id error */
            return RTEMS_INVALID_ID;

            /* task is local (nominal case) */
        case OBJECTS_LOCAL:

            /* get the thread object information */
            the_information = _Objects_Get_information(the_thread->Object.id);

            /* if there is no information */
            if(!the_information)
            {
                /* this should never happen if _Thread_Get() works right */
                /* then re-enable thread dispatch (disabled by _Thread_Get) */
                _Thread_Enable_dispatch();

                /* and return invalid identifier */
                return RTEMS_INVALID_ID;

            }

            /* close the thread */
            _Thread_Close(the_information , the_thread);

            /* free the thread */
            _RTEMS_tasks_Free(the_thread);

#if defined(RTEMS_MULTIPROCESSING)

            /* check if task is global */
            if(the_thread->is_global)
            {
                /* task is global, close the object MP information */
                _Objects_MP_Close(&_RTEMS_tasks_Information , the_thread->Object.id);

                /* send a packet announcing the task has been deleted */
                _RTEMS_tasks_MP_Send_process_packet(RTEMS_TASKS_MP_ANNOUNCE_DELETE ,
                                                    the_thread->Object.id ,
                                                    0); /* Not used */
            }
#endif

            /* enable dispatch (disabled by _Thread_Get)*/
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
