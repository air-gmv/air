/**
 *  @file
 *  taskgetnote.c
 *
 *  @brief get task note
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
 *  7110    | 09/04/2010  | mcoutinho    | IPR 1931
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


rtems_status_code rtems_task_get_note(
                                      Objects_Id id ,
                                      uint32_t notepad ,
                                      uint32_t *note
                                      )
{
    /* the task to get the note */
    register Thread_Control *the_thread;

    /* the task to get the note location */
    Objects_Locations location;

    /* the task to get the note API information */
    RTEMS_API_Control *api;


    /* check if the note address is NULL */
    if(!note)
    {
        /* return invalid address error */
        return RTEMS_INVALID_ADDRESS;
    }

    /* check the notepad number is valid
     * @note There is no check for < RTEMS_NOTEPAD_FIRST because that would
     * be checking an unsigned number for being negative */
    if(notepad > RTEMS_NOTEPAD_LAST)
    {
        /* return invalid number error */
        return RTEMS_INVALID_NUMBER;
    }

    /* optimize the most likely case to avoid the Thread_Dispatch */

    /* check if is getting a note of the calling thread */
    if(_Objects_Are_ids_equal(id , OBJECTS_ID_OF_SELF) ||
       _Objects_Are_ids_equal(id , _Thread_Executing->Object.id))
    {
        /* get the RTEMS API */
        api = _Thread_Executing->API_Extensions[ THREAD_API_RTEMS ];

        /* get the note */
        *note = api->Notepads[ notepad ];

        /* return success */
        return RTEMS_SUCCESSFUL;
    }

    /* get the task */
    the_thread = _Thread_Get(id , &location);

    /* check the task location */
    switch(location)
    {

#if defined(RTEMS_MULTIPROCESSING)

            /* task is remote */
        case OBJECTS_REMOTE:

            /* set the thread return argument to the note */
            _Thread_Executing->Wait.return_argument = note;

            /* send a packet to the remote node */
            return _RTEMS_tasks_MP_Send_request_packet(RTEMS_TASKS_MP_GET_NOTE_REQUEST ,
                                                       id ,
                                                       0 , /* Not used */
                                                       notepad ,
                                                       0 /* Not used */);
#endif

            /* task id is invalid */
        case OBJECTS_ERROR:

            /* return invalid identifier error */
            return RTEMS_INVALID_ID;

            /* task is local (nominal case) */
        case OBJECTS_LOCAL:
            /* get the RTEMS API */
            api = the_thread->API_Extensions[ THREAD_API_RTEMS ];

            /* get the note */
            *note = api->Notepads[ notepad ];

            /* enable thread dispatch (disabled by _Thread_Get) */
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
