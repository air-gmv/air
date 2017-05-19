/**
 *  @file
 *  msgqurgent.c
 *
 *  @brief send an urgent message to a message queue
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  574     | 17/11/2008  | mcoutinho    | IPR 64
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  7090    | 09/04/2010  | mcoutinho    | IPR 1931
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

#if defined(RTEMS_MULTIPROCESSING)
    #define MESSAGE_QUEUE_MP_HANDLER _Message_queue_Core_message_queue_mp_support
#else
    #define MESSAGE_QUEUE_MP_HANDLER NULL
#endif


rtems_status_code rtems_message_queue_urgent(
                                             Objects_Id id ,
                                             void *buffer ,
                                             size_t size
                                             )
{
    /* message queue to send an urgent message */
    register Message_queue_Control *the_message_queue;

    /* message queue to send an urgent message location */
    Objects_Locations location;

    /* status of the CORE message queue */
    CORE_message_queue_Status status;


    /* check if buffer is NULL */
    if(!buffer)
    {
        /* return illegal address (buffer address is NULL) */
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

            /* send a remote package with the urgent message */
            return _Message_queue_MP_Send_request_packet(MESSAGE_QUEUE_MP_URGENT_REQUEST ,
                                                         id ,
                                                         buffer ,
                                                         &size ,
                                                         0 , /* option_set */
                                                         MPCI_DEFAULT_TIMEOUT);
#endif

            /* message queue id is invalid */
        case OBJECTS_ERROR:

            /* return invalid indentifier */
            return RTEMS_INVALID_ID;

            /* message queue is local (nominal case) */
        case OBJECTS_LOCAL:

            /* send an urgent message */
            status = _CORE_message_queue_Urgent(&the_message_queue->message_queue ,
                                                buffer ,
                                                size ,
                                                id ,
#if defined(RTEMS_MULTIPROCESSING)
                MESSAGE_QUEUE_MP_HANDLER ,
#endif
                FALSE , /* sender does not block */
                                                0); /* no timeout */


            /* enable dispatch (disabled by _Message_queue_Get) */
            _Thread_Enable_dispatch();

            /* since this API does not allow for blocking sends, we can directly
             * return the returned status */
            return _Message_queue_Translate_core_message_queue_return_code(status);

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
