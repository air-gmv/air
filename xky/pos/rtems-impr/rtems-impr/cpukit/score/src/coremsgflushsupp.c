/**
 *  @file
 *  coremsgflushsupp.c
 *
 *  @brief additional support to flush the messages of a core message queue
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


uint32_t _CORE_message_queue_Flush_support(
                                           CORE_message_queue_Control *the_message_queue
                                           )
{
    /* interrupt level */
    ISR_Level level;

    /* first inactive node */
    Chain_Node *inactive_first;

    /* first pending message */
    Chain_Node *message_queue_first;

    /* last pending message */
    Chain_Node *message_queue_last;

    /* number of pending messages */
    uint32_t count;

    
    /* Currently, RTEMS supports no API that has both flush and blocking
     * sends.  Thus, this routine assumes that there are no senders
     * blocked waiting to send messages.  In the event, that an API is
     * added that can flush a message queue when threads are blocked
     * waiting to send, there are two basic behaviors envisioned:
     *   (1) The thread queue of pending senders is a logical extension
     * of the pending message queue.  In this case, it should be
     * flushed using the _Thread_queue_Flush() service with a status
     * such as CORE_MESSAGE_QUEUE_SENDER_FLUSHED (which currently does
     * not exist).  This can be implemented without changing the "big-O"
     * of the message flushing part of the routine.
     *   (2) Only the actual messages queued should be purged.  In this case,
     * the blocked sender threads must be allowed to send their messages.
     * In this case, the implementation will be forced to individually
     * dequeue the senders and queue their messages.  This will force
     * this routine to have "big O(n)" where n is the number of blocked
     * senders.  If there are more messages pending than senders blocked,
     * then the existing flush code can be used to dispose of the remaining
     * pending messages.
     * For now, though, we are very happy to have a small routine with
     * fixed execution time that only deals with pending messages */

    /* enter critical section */
    _ISR_Disable(level);

    /* get first inactive message */
    inactive_first = the_message_queue->Inactive_messages.first;

    /* get first pending message */
    message_queue_first = the_message_queue->Pending_messages.first;

    /* get last pending message */
    message_queue_last = the_message_queue->Pending_messages.last;

    /* set the first inactive message to the first pending message */
    the_message_queue->Inactive_messages.first = message_queue_first;

    /* set the last pending message next field to the first inactive message */
    message_queue_last->next = inactive_first;

    /* set the first inactive message previous field to the last pending message */
    inactive_first->previous = message_queue_last;

    /* set the first pending message previous field to the head of the chain of inactive messages */
    message_queue_first->previous =
        _Chain_Head(&the_message_queue->Inactive_messages);

    /* initialize the chain of pending messages */
    _Chain_Initialize_empty(&the_message_queue->Pending_messages);

    /* get the number of pending messages */
    count = the_message_queue->number_of_pending_messages;

    /* reset the number of pending messages to 0 */
    the_message_queue->number_of_pending_messages = 0;

    /* leave critical section */
    _ISR_Enable(level);

    /* return the previous number of pending messages */
    return count;
}

/**  
 *  @}
 */

/**
 *  @}
 */
