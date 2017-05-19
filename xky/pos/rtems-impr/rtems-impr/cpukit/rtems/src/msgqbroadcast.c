/**
 *  @file
 *  msgqbroadcast.c
 *
 *  @brief broadcast a message to a message queue
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
 *  7081    | 09/04/2010  | mcoutinho    | IPR 1931
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


rtems_status_code rtems_message_queue_broadcast(
                                                Objects_Id id ,
                                                void *buffer ,
                                                size_t size ,
                                                uint32_t *count
                                                )
{
    /* message queue to broadcast */
    register Message_queue_Control *the_message_queue;

    /* message queue location */
    Objects_Locations location;

    /* status of the CORE Message queue broadcast */
    CORE_message_queue_Status core_status;


    /* check if buffer is NULL */
    if(!buffer)
    {
        /* return invalid address */
        return RTEMS_INVALID_ADDRESS;
    }

    /* check if count is 0 */
    if(!count)
    {
        /* return invalid address */
        return RTEMS_INVALID_ADDRESS;
    }

    /* get the message queue */
    the_message_queue = _Message_queue_Get(id , &location);

    /* check the message queue location */
    switch(location)
    {

#if defined(RTEMS_MULTIPROCESSING)

            /* message queue is remote */
        case OBJECTS_REMOTE:

            /* set the thread return argument */
            _Thread_Executing->Wait.return_argument = count;

            /* send a remote packet */
            return _Message_queue_MP_Send_request_packet(MESSAGE_QUEUE_MP_BROADCAST_REQUEST ,
                                                         id ,
                                                         buffer ,
                                                         &size ,
                                                         0 , /* option_set not used */
                                                         MPCI_DEFAULT_TIMEOUT);
#endif

            /* message queue id is invalid */
        case OBJECTS_ERROR:

            /* return invalid identifier */
            return RTEMS_INVALID_ID;

            /* message queue is local (nominal case) */
        case OBJECTS_LOCAL:

            /* broadcast the message to the message queue */
            core_status = _CORE_message_queue_Broadcast(&the_message_queue->message_queue ,
                                                        buffer ,
                                                        size ,
                                                        id ,
#if defined(RTEMS_MULTIPROCESSING)
                _Message_queue_Core_message_queue_mp_support ,
#endif
                count);

            /* re-enable thread dispatch (disabled by _Message_queue_Get) */
            _Thread_Enable_dispatch();

            /* return the translated message queue code */
            return _Message_queue_Translate_core_message_queue_return_code(core_status);

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
