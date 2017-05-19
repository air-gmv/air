/**
 *  @file
 *  msgqgetnumberpending.c
 *
 *  @brief get the number of messages in a message queue
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
 *  7085    | 09/04/2010  | mcoutinho    | IPR 1931
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


rtems_status_code rtems_message_queue_get_number_pending(
                                                         Objects_Id id ,
                                                         uint32_t *count
                                                         )
{
    /* message queue to get the number of messages pending */
    register Message_queue_Control *the_message_queue;

    /* message queue to get the number of messages pending location */
    Objects_Locations location;


    /* check if count is NULL */
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

            /* set the thread return argument to the count address */
            _Thread_Executing->Wait.return_argument = count;

            /* get number of pending messages from the remote message queue */
            return _Message_queue_MP_Send_request_packet(MESSAGE_QUEUE_MP_GET_NUMBER_PENDING_REQUEST ,
                                                         id ,
                                                         0 , /* buffer not used */
                                                         0 , /* size */
                                                         0 , /* option_set not used */
                                                         MPCI_DEFAULT_TIMEOUT);
#endif

            /* message queue id is invalid */
        case OBJECTS_ERROR:

            /* return invalid indentifier */
            return RTEMS_INVALID_ID;

            /* message queue is local (nominal case) */
        case OBJECTS_LOCAL:

            /* get the message queue number of pending messages */
            *count = the_message_queue->message_queue.number_of_pending_messages;

            /* enable dispatch (disabled by _Message_queue_Get) */
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
