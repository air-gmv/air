/**
 *  @file
 *  coremsgbroadcast.c
 *
 *  @brief broadcast a message to a core message queue
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
 *  619     | 17/11/2008  | mcoutinho    | IPR 64
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


CORE_message_queue_Status _CORE_message_queue_Broadcast(
                                                        CORE_message_queue_Control *the_message_queue ,
                                                        void *buffer ,
                                                        size_t size ,
                                                        Objects_Id id ,
#if defined(RTEMS_MULTIPROCESSING)
    CORE_message_queue_API_mp_support_callout api_message_queue_mp_support ,
#endif
    uint32_t *count
                                                        )
{
    /* iterator thread to broadcast the message */
    Thread_Control *the_thread;

    /* number of threads that received the message */
    uint32_t number_broadcasted;

    /* thread wait information */
    Thread_Wait_information *waitp;

    /* size of the message sent */
    uint32_t constrained_size;


    /* If there are pending messages, then there can't be threads
     * waiting for us to send them a message.
     * NOTE: This check is critical because threads can block on
     *       send and receive and this ensures that we are broadcasting
     *       the message to threads waiting to receive -- not to send */
    if(the_message_queue->number_of_pending_messages != 0)
    {
        /* no messages were sent */
        *count = 0;

        /* return success */
        return CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;
    }

    /* There must be no pending messages if there is a thread waiting to
     * receive a message */

    /* initialized the number of threads that have received a message to none */
    number_broadcasted = 0;

    /* iterate through all the threads waiting to receive a message on
     * this message queue */
    while(( the_thread = _Thread_queue_Dequeue(&the_message_queue->Wait_queue) ))
    {
        /* get the thread information */
        waitp = &the_thread->Wait;

        /* increment the number of threads that have received a message */
        number_broadcasted += 1;

        /* limit the maximum size to the message queue maximum size
         * @todo: move the calculation of the constrained size to outside the 
         * cycle. The message queue maximum message size is never changed, so,
         * why calculate it at every loop? */

        /* get the original size */
        constrained_size = size;

        /* if size is larger than the maximum size */
        if(size > the_message_queue->maximum_message_size)
        {
            /* then limit the size */
            constrained_size = the_message_queue->maximum_message_size;
        }

        /* copy the message
         *  from buffer
         *  to the thread return argument
         *  with the size of the message */
        _CORE_message_queue_Copy_buffer(buffer ,
                                        waitp->return_argument ,
                                        constrained_size);

        /* the return argument is the size */
        *(uint32_t *) the_thread->Wait.return_argument_1 = size;

        /* if multiprocessing is enabled */
#if defined(RTEMS_MULTIPROCESSING)
        /* then check if the thread is local */
        if(!_Objects_Is_local_id(the_thread->Object.id))
        {
            /* it is not, so send the message to the remote node */
            ( *api_message_queue_mp_support ) ( the_thread , id );
        }
#endif
    }

    /* place the count as the number of threads that received the message */
    *count = number_broadcasted;

    /* return success */
    return CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;
}

/**  
 *  @}
 */

/**
 *  @}
 */
