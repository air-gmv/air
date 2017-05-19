/**
 *  @file
 *  coremsgclose.c
 *
 *  @brief close a core message queue
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
 *  3615    | 02/07/2009  | mcoutinho    | IPR 97
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


void _CORE_message_queue_Close(
                               CORE_message_queue_Control *the_message_queue ,
#if defined(RTEMS_MULTIPROCESSING)
    Thread_queue_Flush_callout remote_extract_callout ,
#endif
    uint32_t status
                               )
{
    /* flush blocked threads whether they were blocked on
     * a send or receive */
    _Thread_queue_Flush(&the_message_queue->Wait_queue ,
#if defined(RTEMS_MULTIPROCESSING)
        remote_extract_callout ,
#endif
        status);

    /* this removes all messages from the pending message queue. Since
     * we just flushed all waiting threads, we don't have to worry about
     * the flush satisfying any blocked senders as a side-effect */

    /* if there are pending messages */
    if(the_message_queue->number_of_pending_messages != 0)
    {
        /* flush the pending messages */
        (void) _CORE_message_queue_Flush_support(the_message_queue);
    }

    /* free the space allocated to the messages */
    (void) _Workspace_Free(the_message_queue->message_buffers);

}

/**  
 *  @}
 */

/**
 *  @}
 */
