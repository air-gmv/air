/**
 *  @file
 *  coremsginsert.c
 *
 *  @brief insert a message to end of a core message queue
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2005.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  3509    | 19/06/2009  | mcoutinho    | IPR 528
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


void _CORE_message_queue_Insert_message(
                                        CORE_message_queue_Control *the_message_queue ,
                                        CORE_message_queue_Buffer_control *the_message ,
                                        CORE_message_queue_Submit_types submit_type
                                        )
{
    /* interrupt level */
    ISR_Level level;


    /* the priority is equal to the submition type */
    the_message->priority = submit_type;

    /* check the submit type */
    switch(submit_type)
    {
            /* case is to send a message normally */
        case CORE_MESSAGE_QUEUE_SEND_REQUEST:

            /* enter critical zone */
            _ISR_Disable(level);

            /* increase the number of pending messages */
            the_message_queue->number_of_pending_messages++;

            /* append the message */
            _CORE_message_queue_Append_unprotected(the_message_queue , the_message);

            /* leave critical zone */
            _ISR_Enable(level);

            /* and leave */
            break;

            /*case is to send a message urgently */
        case CORE_MESSAGE_QUEUE_URGENT_REQUEST:

            /* enter critical zone */
            _ISR_Disable(level);

            /* increase the number of pending messages */
            the_message_queue->number_of_pending_messages++;

            /* prepend the message */
            _CORE_message_queue_Prepend_unprotected(the_message_queue ,
                                                    the_message);
            /* leave critical zone */
            _ISR_Enable(level);

            /* and leave */
            break;

            /* invalid submit type */
        default:

            /* raise internal error */
            _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                     TRUE ,
                                     INTERNAL_ERROR_INVALID_MESSAGE_SUBMIT_TYPE);

    }
}

/**  
 *  @}
 */

/**
 *  @}
 */
