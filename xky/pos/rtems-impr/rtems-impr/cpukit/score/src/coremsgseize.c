/**
 *  @file
 *  coremsgseize.c
 *
 *  @brief seize a message from a core message queue
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
 *  4417    | 21/09/2009  | mcoutinho    | IPR 616
 *  4445    | 21/09/2009  | mcoutinho    | IPR 768
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


void _CORE_message_queue_Seize(
                               CORE_message_queue_Control *the_message_queue ,
                               Objects_Id id ,
                               void *buffer ,
                               size_t *size ,
                               boolean wait ,
                               Watchdog_Interval timeout
                               )
{
    /* interrupt level */
    ISR_Level level;

    /* message received */
    CORE_message_queue_Buffer_control *the_message;

    /* executing thread */
    Thread_Control *executing;


    /* get the executing thread */
    executing = _Thread_Executing;

    /* set the default return code to successful */
    executing->Wait.return_code = CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL;

    /* enter critical zone */
    _ISR_Disable(level);

    /* check if there are pending messages */
    if(the_message_queue->number_of_pending_messages != 0)
    {
        /* there is at least one message, seize it */

        /* decrement the message queue number of pending messages */
        the_message_queue->number_of_pending_messages -= 1;

        /* get the pending message */
        the_message = _CORE_message_queue_Get_pending_message(the_message_queue);

        /* leave critical zone */
        _ISR_Enable(level);

        /* the message is not supposed to be NULL*/
        if(the_message == NULL)
        {
            /* raise internal error */
            _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                     TRUE ,
                                     INTERNAL_ERROR_INVALID_PENDING_MESSAGE);
        }

        /* set the message size as the output argument */
        *size = the_message->Contents.size;

        /* set the calling thread count level to the priority of the message */
        _Thread_Executing->Wait.count = the_message->priority;

        /* copy the message to the message buffer */
        _CORE_message_queue_Copy_buffer(the_message->Contents.buffer , buffer , *size);

        /* free the buffer */
        _CORE_message_queue_Free_message_buffer(the_message_queue , the_message);

        /* and leave */
        return;
    }

    /* no pending message, checks if wants to wait */
    if(!wait)
    {
        /* doesnt want to wait, leave critical zone */
        _ISR_Enable(level);

        /* set the return code to unsatisfied */
        executing->Wait.return_code = CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_NOWAIT;

        /* and leave */
        return;
    }

    /* if we are here then the thread must wait for a message */

    /* enter the thread queue critical section */
    _Thread_queue_Enter_critical_section(&the_message_queue->Wait_queue);

    /* initialize the calling thread Wait parameters */
    executing->Wait.queue = &the_message_queue->Wait_queue; /* queue */
    executing->Wait.id = id; /* message queue identifier */
    executing->Wait.return_argument = buffer; /* buffer */
    executing->Wait.return_argument_1 = (void *) size; /* size */

    /* Wait.count will be filled in with the message priority */

    /* leave critical zone */
    _ISR_Enable(level);

    /* enqueue the calling thread on the message queue */
    _Thread_queue_Enqueue(&the_message_queue->Wait_queue , timeout);
}

/**  
 *  @}
 */

/**
 *  @}
 */
