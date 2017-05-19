/**
 *  @file
 *  msgqsend.c
 *
 *  @brief send a message to a message queue
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
 *  3596    | 02/07/2009  | mcoutinho    | IPR 97
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  7088    | 09/04/2010  | mcoutinho    | IPR 1931
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
#endif


rtems_status_code rtems_message_queue_send(
                                           Objects_Id id ,
                                           void *buffer ,
                                           size_t size
                                           )
{
    /* message queue to where to sent the message */
    register Message_queue_Control *the_message_queue;

    /* message queue location */
    Objects_Locations location;

    /* CORE message Handler status */
    CORE_message_queue_Status status;

    /* result of this function */
    rtems_status_code res;


    /* check if buffer is NULL */
    if(!buffer)
    {
        /* return invalid address */
        res = RTEMS_INVALID_ADDRESS;
    }
    else
    {
        /* get the message queue */
        the_message_queue = _Message_queue_Get(id , &location);

        /* check the message queue location */
        switch(location)
        {
#if defined(RTEMS_MULTIPROCESSING)

                /* message queue is remote */
            case OBJECTS_REMOTE:

                /* send a remote packet */
                return _Message_queue_MP_Send_request_packet(
                                                             MESSAGE_QUEUE_MP_SEND_REQUEST ,
                                                             id ,
                                                             buffer ,
                                                             &size ,
                                                             0 , /* option_set */
                                                             MPCI_DEFAULT_TIMEOUT);
                break;
#endif

                /* message queue id is invalid */
            case OBJECTS_ERROR:

                /* set the return code to invalid indentifier */
                res = RTEMS_INVALID_ID;

                /* and leave */
                break;

                /* message queue is remote */
            case OBJECTS_LOCAL:

                /* send the message */
                status = _CORE_message_queue_Send(&the_message_queue->message_queue ,
                                                  buffer ,
                                                  size ,
                                                  id ,
#if defined(RTEMS_MULTIPROCESSING)              
                    MESSAGE_QUEUE_MP_HANDLER ,
#endif
                    FALSE , /* sender does not block */
                                                  0); /* no timeout */


                /* enable dispatch (disabled by _Message_queue_Get)*/
                _Thread_Enable_dispatch();

                /* since this API does not allow for blocking sends, we can
                 * directly return the returned status */
                res = _Message_queue_Translate_core_message_queue_return_code(status);

                /* and leave */
                break;

                /* default clause: the object location is invalid */
            default:
                
                /* an internal error occured and the object location is invalid */
                res = RTEMS_INTERNAL_INVALID_OBJECT_LOCATION;
        }
    }
    return res;
}

/**  
 *  @}
 */

/**
 *  @}
 */
