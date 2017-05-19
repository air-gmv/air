/**
 *  @file
 *  msgqreceive.c
 *
 *  @brief receive a message from a message queue
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
 *  7087    | 09/04/2010  | mcoutinho    | IPR 1931
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


rtems_status_code rtems_message_queue_receive(
                                              Objects_Id id ,
                                              void *buffer ,
                                              size_t *size ,
                                              uint32_t option_set ,
                                              rtems_interval timeout
                                              )
{
    /* message queue to receive a message from */
    register Message_queue_Control *the_message_queue;

    /* message queue to receive a message from location */
    Objects_Locations location;

    /* wait or not for the message */
    boolean wait;


    /* check buffer address */
    if(!buffer)
    {
        /* return invalid address error */
        return RTEMS_INVALID_ADDRESS;
    }

    /* check output size address */
    if(!size)
    {
        /* return invalid address error */
        return RTEMS_INVALID_ADDRESS;
    }

    /* get message queue */
    the_message_queue = _Message_queue_Get(id , &location);

    /* check the message queue location */
    switch(location)
    {

#if defined(RTEMS_MULTIPROCESSING)
        case OBJECTS_REMOTE:

            /* receive a remote package with the message */
            return _Message_queue_MP_Send_request_packet(MESSAGE_QUEUE_MP_RECEIVE_REQUEST ,
                                                         id ,
                                                         buffer ,
                                                         size ,
                                                         option_set ,
                                                         timeout);
#endif
            /* case identifier has wrong location */
        case OBJECTS_ERROR:
            /* return invalid indentifier */
            return RTEMS_INVALID_ID;

            /* message queue is local (nominal case) */
        case OBJECTS_LOCAL:

            /* check whether to wait for the message */
            if(_Options_Is_no_wait(option_set))
            {
                /* dont wait for message */
                wait = FALSE;
            }
            else
            {
                /* wait for message*/
                wait = TRUE;
            }

            /* seize a message */
            _CORE_message_queue_Seize(&the_message_queue->message_queue ,
                                      the_message_queue->Object.id ,
                                      buffer ,
                                      size ,
                                      wait ,
                                      timeout);

            /* enable dispatch (disabled by _Message_queue_Get) */
            _Thread_Enable_dispatch();

            /* translate the return code to a rtems_status_code */
            return _Message_queue_Translate_core_message_queue_return_code(_Thread_Executing->Wait.return_code);

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
