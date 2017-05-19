/**
 *  @file
 *  coremsgsubmit.c
 *
 *  @brief submit a message (urgent or not) to a core message queue
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
 *  611     | 17/11/2008  | mcoutinho    | IPR 64
 *  3521    | 29/06/2009  | mcoutinho    | IPR 529
 *  5247    | 30/10/2009  | mcoutinho    | IPR 828
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreMessageQueue Message Queue Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/coremsg.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#if defined(RTEMS_MULTIPROCESSING)
    #include <rtems/score/mpci.h>
#endif


CORE_message_queue_Status _CORE_message_queue_Submit(
                                                     CORE_message_queue_Control *the_message_queue ,
                                                     void *buffer ,
                                                     size_t size ,
                                                     Objects_Id id ,
#if defined(RTEMS_MULTIPROCESSING)
    CORE_message_queue_API_mp_support_callout api_message_queue_mp_support ,
#endif
    CORE_message_queue_Submit_types submit_type ,
                                                     boolean wait ,
                                                     Watchdog_Interval timeout
                                                     )
{
    /* message to submit */
    CORE_message_queue_Buffer_control *the_message;

    /* thread to get the message */
    Thread_Control *the_thread;


    /* check the size of the message is valid */
    if(size > the_message_queue->maximum_message_size)
    {
        /* the size is too big, return error */
        return CORE_MESSAGE_QUEUE_STATUS_INVALID_SIZE;
    }

    /* is there a thread currently waiting on this message queue? */

    /* check if there are no pending messages */
    if(the_message_queue->number_of_pending_messages == 0)
    {
        /* no pending messages */

        /* dequeue the waiting thread */
        the_thread = _Thread_queue_Dequeue(&the_message_queue->Wait_queue);

        /* if there is a thread on the queue */
        if(the_thread)
        {
            /* then copy the message to the waiting thread TCB */
            _CORE_message_queue_Copy_buffer(buffer ,
                                            the_thread->Wait.return_argument ,
                                            size);

            /* set the size */
            *(size_t *) the_thread->Wait.return_argument_1 = size;

            /* set submit type (message priority) */
            the_thread->Wait.count = submit_type;

            /* if multiprocessing is enabled */
#if defined(RTEMS_MULTIPROCESSING)

            /* then check if the thread is local */
            if(!_Objects_Is_local_id(the_thread->Object.id))
            {
                /* it is not, sent remove message */
                ( *api_message_queue_mp_support ) ( the_thread , id );
            }
#endif
            /* return success */
            return CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;
        }
    }

    /* no one waiting on the message queue at this time, so attempt to
     * queue the message up for a future receive */

    /* check if there is enough room to send the message */
    if(the_message_queue->number_of_pending_messages <
       the_message_queue->maximum_pending_messages)
    {
        /* allocte space for the message from the message queue */
        the_message = _CORE_message_queue_Allocate_message_buffer(the_message_queue);

        /* check if there is space for the message
         * NOTE: If the system is consistent, this error should never occur */
        if(!the_message)
        {
            /* could not send message */
            return CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED;
        }

        /* copy the message queue to the buffer */
        _CORE_message_queue_Copy_buffer(buffer ,
                                        the_message->Contents.buffer ,
                                        size);

        /* set the size */
        the_message->Contents.size = size;

        /* set the submit type */
        the_message->priority = submit_type;

        /* and insert the message into the message queue */
        _CORE_message_queue_Insert_message(the_message_queue ,
                                           the_message ,
                                           submit_type);

        /* return success */
        return CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;
    }

    /* return too many (message queue is full) */
    return CORE_MESSAGE_QUEUE_STATUS_TOO_MANY;
}

/**  
 *  @}
 */

/**
 *  @}
 */
