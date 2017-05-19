/**
 *  @file
 *  eventsend.c
 *
 *  @brief send an event
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
 *  7074    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_EVENT Event Manager
 *  @{
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/event.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/tasks.h>


rtems_status_code rtems_event_send(
                                   Objects_Id id ,
                                   rtems_event_set event_in
                                   )
{
    /* thread to send an event */
    register Thread_Control *the_thread;

    /* thread to send an event location */
    Objects_Locations location;

    /* api of the thread */
    RTEMS_API_Control *api;


    /* get the thread */
    the_thread = _Thread_Get(id , &location);

    /* check the task location */
    switch(location)
    {
#if defined(RTEMS_MULTIPROCESSING)

            /* task is remote */
        case OBJECTS_REMOTE:

            /* send an event to the corresponding multiprocessor node */
            return ( _Event_MP_Send_request_packet(EVENT_MP_SEND_REQUEST ,
                                                   id ,
                                                   event_in) );
#endif

            /* task id is invalid */
        case OBJECTS_ERROR:

            /* the thread could not be get, so the id is invalid */
            return RTEMS_INVALID_ID;

            /* task is local (nominal case) */
        case OBJECTS_LOCAL:

            /* get the thread api extension */
            api = the_thread->API_Extensions[ THREAD_API_RTEMS ];

            /* post the event to the thread */
            _Event_sets_Post(event_in , &api->pending_events);

            /* surrender an event to the thread */
            _Event_Surrender(the_thread);

            /* enable dispatch (disabled by _Thread_Get) */
            _Thread_Enable_dispatch();

            /* and return success */
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
