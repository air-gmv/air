/**
 *  @file
 *  coremsg.c
 *
 *  @brief initialize a core message queue
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


boolean _CORE_message_queue_Initialize(
                                       CORE_message_queue_Control *the_message_queue ,
                                       CORE_message_queue_Attributes *the_message_queue_attributes ,
                                       uint32_t maximum_pending_messages ,
                                       uint32_t maximum_message_size
                                       )
{
    /* memory needed for the messages */
    uint32_t message_buffering_required;

    /* memory allocated for each message */
    uint32_t allocated_message_size;

    /* thread queue discipline */
    Thread_queue_Disciplines discipline;


    /* initialize the message queue fields */

    /* initialize the maximum pending messages */
    the_message_queue->maximum_pending_messages = maximum_pending_messages;

    /* initialize the number of pending messages */
    the_message_queue->number_of_pending_messages = 0;

    /* initialize the maximum size of each message */
    the_message_queue->maximum_message_size = maximum_message_size;

    /* set the message queue notify setting */
    _CORE_message_queue_Set_notify(the_message_queue , NULL , NULL);

    /* round size up to multiple of a pointer for chain init and
     * check for overflow on adding overhead to each message */
    allocated_message_size = maximum_message_size;
    if(allocated_message_size & ( sizeof (uint32_t ) - 1 ))
    {
        allocated_message_size += sizeof (uint32_t );
        allocated_message_size &= ~( sizeof (uint32_t ) - 1 );
    }

    /* check the allocated message size */
    if(allocated_message_size < maximum_message_size)
    {
        /* is smaller than the maximum, return FALSE */
        return FALSE;
    }

    /* calculate how much total memory is required for message buffering and
     * check for overflow on the multiplication */
    message_buffering_required = maximum_pending_messages *
        ( allocated_message_size + sizeof (CORE_message_queue_Buffer_control ) );

    /* check if the allocated message size is smaller */
    if(message_buffering_required < allocated_message_size)
    {
        /* error, return FALSE */
        return FALSE;
    }

    /* allocate the message memory */
    the_message_queue->message_buffers = (CORE_message_queue_Buffer *)
        _Workspace_Allocate(message_buffering_required);

    /* check if message queue buffers could be allocated */
    if(the_message_queue->message_buffers == 0)
    {
        /* could not, return FALSE */
        return FALSE;
    }

    /* initialize the pool of inactive messages
     *   with the starting address of the message queue message area
     *   the maximum pending messages corresponds to the number of nodes on the chain
     *   and the node size is the size of each message plus the size of MBC */
    _Chain_Initialize(&the_message_queue->Inactive_messages ,
                      the_message_queue->message_buffers ,
                      maximum_pending_messages ,
                      allocated_message_size + sizeof ( CORE_message_queue_Buffer_control ));

    /* initialize the pending messages chain as an empty chain */
    _Chain_Initialize_empty(&the_message_queue->Pending_messages);

    /* check the discipline */
    if(_CORE_message_queue_Is_priority(the_message_queue_attributes))
    {
        /* discipline is priority */
        discipline = THREAD_QUEUE_DISCIPLINE_PRIORITY;
    }
    else
    {
        /* discipline is FIFO */
        discipline = THREAD_QUEUE_DISCIPLINE_FIFO;
    }

    /* initialize the message queue thread queue with
     *   the discipline of the messsage queue
     *   the state waiting for message
     *   the timeout status */
    _Thread_queue_Initialize(&the_message_queue->Wait_queue ,
                             discipline ,
                             STATES_WAITING_FOR_MESSAGE ,
                             CORE_MESSAGE_QUEUE_STATUS_TIMEOUT);

    return TRUE;
}

/**  
 *  @}
 */

/**
 *  @}
 */
