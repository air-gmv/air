/**
 *  @file
 *  coremsg.inl
 *
 *  @brief contains the static inline implementation of all
 *  inlined routines in the Core Message Handler.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  607     | 17/11/2008  | mcoutinho    | IPR 64
 *  4546    | 28/09/2009  | mcoutinho    | IPR 778
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6116    | 18/02/2010  | mcoutinho    | IPR 1818
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
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

#ifndef _RTEMS_SCORE_COREMSG_INL
#define _RTEMS_SCORE_COREMSG_INL

#include <string.h>   /* needed for memcpy */
#include <rtems/system.h>


/**
 *  @brief send a message to a core message queue
 *  
 *  This routine sends a message to the end of the specified message queue.
 *
 *  @param[in] the_message_queue the message queue to send the message
 *  @param[in] buffer the buffer containing the message
 *  @param[in] size the size of the message (in number of bytes)
 *  @param[in] id the RTEMS object Id associated with this message queue
 *  It is used when unblocking a remote thread
 *  @param[in] api_message_queue_mp_support the routine to invoke if
 *  a thread that is unblocked is actually a remote thread
 *  @param[in] wait indicate whether to wait to send a message or not
 *  @param[in] timeout indicate the amount of time to wait
 *
 *  @return returns the status of the message send
 */
RTEMS_INLINE_ROUTINE CORE_message_queue_Status _CORE_message_queue_Send(
                                                                        CORE_message_queue_Control *the_message_queue ,
                                                                        void *buffer ,
                                                                        size_t size ,
                                                                        Objects_Id id ,
#if defined(RTEMS_MULTIPROCESSING)
    CORE_message_queue_API_mp_support_callout api_message_queue_mp_support ,
#endif
    boolean wait ,
                                                                        Watchdog_Interval timeout
                                                                        )
{
   /* submit the message to the message queue with normal priority with
    *   the message queue to where to send the message
    *   the message itself
    *   the size of the message
    *   the id of the message queue
    *   if multiprocessing, the callout support function
    *   normal priority
    *   wait option
    *   timeout interval */
   return _CORE_message_queue_Submit(the_message_queue ,
                                     buffer ,
                                     size ,
                                     id ,
#if defined(RTEMS_MULTIPROCESSING)
       api_message_queue_mp_support ,
#endif
       CORE_MESSAGE_QUEUE_SEND_REQUEST , 
                                     wait , 
                                     timeout); 
}


/**
 *  @brief send an urgent message to a message queue
 *  
 *  This routine sends a message to the front of the specified message queue.
 *
 *  @param[in] the_message_queue the message queue to send the message
 *  @param[in] buffer the buffer containing the message
 *  @param[in] size the size of the message (in number of bytes)
 *  @param[in] id the RTEMS object Id associated with this message queue.
 *  It is used when unblocking a remote thread
 *  @param[in] api_message_queue_mp_support the routine to invoke if
 *  a thread that is unblocked is actually a remote thread
 *  @param[in] wait indicate whether to wait to send a message or not
 *  @param[in] timeout indicate the amount of time to wait
 *
 *  @return returns the status of the message send
 */
RTEMS_INLINE_ROUTINE CORE_message_queue_Status _CORE_message_queue_Urgent(
                                                                          CORE_message_queue_Control *the_message_queue ,
                                                                          void *buffer ,
                                                                          size_t size ,
                                                                          Objects_Id id ,
#if defined(RTEMS_MULTIPROCESSING)
    CORE_message_queue_API_mp_support_callout api_message_queue_mp_support ,
#endif
    boolean wait ,
                                                                          Watchdog_Interval timeout
                                                                          )
{
   /* submit the message to the message queue with urgent priority with:
    *   the message queue
    *   the buffer (message)
    *   size of the message
    *   if of the message queue
    *   if multiprocessing, the API support
    *   the urgent priority
    *   wait option
    *   timeout interval */
   return _CORE_message_queue_Submit(the_message_queue ,
                                     buffer ,
                                     size ,
                                     id ,
#if defined(RTEMS_MULTIPROCESSING)
       api_message_queue_mp_support ,
#endif
       CORE_MESSAGE_QUEUE_URGENT_REQUEST , /* urgent priority */
                                     wait , /* sender may block */
                                     timeout); /* timeout interval */
}


/**
 *  @brief copy the content of a message from a source to a destination
 *  
 *  This routine copies the contents of the source message buffer
 *  to the destination message buffer.
 *
 *  @param[in] source the source of the message
 *  @param[out] destination the place where to copy the message
 *  @param[in] size the size of the message
 */
RTEMS_INLINE_ROUTINE void _CORE_message_queue_Copy_buffer(
                                                          const void *source ,
                                                          void *destination ,
                                                          size_t size
                                                          )
{
   /* copy the message from the source to the destination */
   memcpy(destination , source , size);
}


/**
 *  @brief allocate a message buffer from a message queue
 *  
 *  This function allocates a message buffer from the inactive
 *  message buffer chain.
 *
 *  @param[in] the_message_queue the message queue
 *
 *  @return returns the allocated message buffer
 */
RTEMS_INLINE_ROUTINE CORE_message_queue_Buffer_control *
_CORE_message_queue_Allocate_message_buffer(
                                            CORE_message_queue_Control *the_message_queue
                                            )
{
   /* get the next node from the message queue inactive messages chain 
    * can cast to a CORE_message_queue_Buffer_control since each node on the 
    * chain is a message */
   return (CORE_message_queue_Buffer_control *)
       _Chain_Get(&the_message_queue->Inactive_messages);
}


/**
 *  @brief free message buffer from a message queue
 *  
 *  This routine frees a message buffer to the inactive
 *  message buffer chain.
 *
 *  @param[in] the_message_queue the message queue
 *  @param[in] the_message the message to free
 */
RTEMS_INLINE_ROUTINE void _CORE_message_queue_Free_message_buffer(
                                                                  CORE_message_queue_Control *the_message_queue ,
                                                                  CORE_message_queue_Buffer_control *the_message
                                                                  )
{
   /* append the node to the message queue inactive messages chain */
   _Chain_Append(&the_message_queue->Inactive_messages , &the_message->Node);
}


/**
 *  @brief get pending message from the message queue
 *  
 *  This function removes the first message from the_message_queue
 *  and returns a pointer to it.
 *
 *  @param[in] the_message_queue the message queue
 *
 *  @return returns the pending message
 */
RTEMS_INLINE_ROUTINE
CORE_message_queue_Buffer_control *_CORE_message_queue_Get_pending_message(
                                                                           CORE_message_queue_Control *the_message_queue
                                                                           )
{
   /* get the first node on the chain of pending messages
    * can cast to a CORE_message_queue_Buffer_control since each node on the
    * chain is a message */
   return (CORE_message_queue_Buffer_control *)
       _Chain_Get_unprotected(&the_message_queue->Pending_messages);
}


/**
 *  @brief determine if the message queue has priority queueing
 *  
 *  This function returns TRUE if the priority attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 *
 *  @param[in] the_attribute the attribute of the message queue
 *
 *  @return returns TRUE of the message queue has priority queueing and FALSE
 *  otherwise
 */
RTEMS_INLINE_ROUTINE boolean _CORE_message_queue_Is_priority(
                                                             CORE_message_queue_Attributes *the_attribute
                                                             )
{
   /* return true if the discipline is priority queueing */
   return (the_attribute->discipline == CORE_MESSAGE_QUEUE_DISCIPLINES_PRIORITY );
}


/**
 *  @brief append a message to the message queue (unprotected)
 *  
 *  This routine places the_message at the rear of the outstanding
 *  messages on the_message_queue.
 *
 *  @param[in] the_message_queue the message queue
 *  @param[in] the_message the message to append
 */
RTEMS_INLINE_ROUTINE void _CORE_message_queue_Append_unprotected(
                                                                 CORE_message_queue_Control *the_message_queue ,
                                                                 CORE_message_queue_Buffer_control *the_message
                                                                 )
{
   /* append (i.e. to the end) a message to the message queue chain of pending
    * messages */
   _Chain_Append_unprotected(&the_message_queue->Pending_messages ,
                             &the_message->Node);
}


/**
 *  @brief prepend a message to the message queue (unprotected)
 *  
 *  This routine places the_message at the front of the outstanding
 *  messages on the_message_queue.
 *
 *  @param[in] the_message_queue the message queue
 *  @param[in] the_message the message to prepend
 */
RTEMS_INLINE_ROUTINE void _CORE_message_queue_Prepend_unprotected(
                                                                  CORE_message_queue_Control *the_message_queue ,
                                                                  CORE_message_queue_Buffer_control *the_message
                                                                  )
{
   /* prepend (i.e. to the front) a message to the message queue list of
    * pending messages */
   _Chain_Prepend_unprotected(&the_message_queue->Pending_messages ,
                              &the_message->Node);
}


/**
 *  @brief set the notify information for the message queue
 *  
 *  This routine initializes the notification information for the_message_queue.
 *
 *  @param[in] the_message_queue the message queue
 *  @param[in] the_handler the notification handler
 *  @param[in] the_argument the argument of the handler when it is invoked
 */
RTEMS_INLINE_ROUTINE void _CORE_message_queue_Set_notify(
                                                         CORE_message_queue_Control *the_message_queue ,
                                                         CORE_message_queue_Notify_Handler the_handler ,
                                                         void *the_argument
                                                         )
{
   /* set the message queue notify handler */
   the_message_queue->notify_handler = the_handler;

   /* set the message queue notify argument */
   the_message_queue->notify_argument = the_argument;
}

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
