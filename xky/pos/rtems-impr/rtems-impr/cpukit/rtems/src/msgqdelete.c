/**
 *  @file
 *  msgqdelete.c
 *
 *  @brief delete a message queue
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
 *  7083    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_MESSAGE Message Queue Manager
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/coremsg.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#if defined(RTEMS_MULTIPROCESSING)
    #include <rtems/score/mpci.h>
#endif
#include <rtems/rtems/status.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/message.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/support.h>


rtems_status_code rtems_message_queue_delete(
                                             Objects_Id id
                                             )
{
    /* message queue to delete */
    register Message_queue_Control *the_message_queue;

    /* message queue to delete location*/
    Objects_Locations location;


    /* get the message queue */
    the_message_queue = _Message_queue_Get(id , &location);

    /* check the message queue location */
    switch(location)
    {

#if defined(RTEMS_MULTIPROCESSING)

            /* message queue is remote */
        case OBJECTS_REMOTE:

            /* re-enable thread dispatch */
            _Thread_Dispatch();

            /* return error (message queue cannot be deleted remotely) */
            return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

            /* message queue id is invalid */
        case OBJECTS_ERROR:

            /* return error (invalid id) */
            return RTEMS_INVALID_ID;

            /* message queue is local (nominal case) */
        case OBJECTS_LOCAL:

            /* close the object */
            _Objects_Close(&_Message_queue_Information ,
                           &the_message_queue->Object);

            /* close the message queue */
            _CORE_message_queue_Close(&the_message_queue->message_queue ,
#if defined(RTEMS_MULTIPROCESSING)
                _Message_queue_MP_Send_object_was_deleted ,
#endif
                CORE_MESSAGE_QUEUE_STATUS_WAS_DELETED);

            /* free the message queue */
            _Message_queue_Free(the_message_queue);

#if defined(RTEMS_MULTIPROCESSING)

            /* check if the message queue is global */
            if(_Attributes_Is_global(the_message_queue->attribute_set))
            {
                /* close the multiprocessor object information */
                _Objects_MP_Close(&_Message_queue_Information ,
                                  the_message_queue->Object.id);

                /* send a message to delete the remote message queue */
                _Message_queue_MP_Send_process_packet(MESSAGE_QUEUE_MP_ANNOUNCE_DELETE ,
                                                      the_message_queue->Object.id ,
                                                      0 , /* Not used */
                                                      0);
            }
#endif

            /* enable dispatch (disabled by _Message_queue_Get) */
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
