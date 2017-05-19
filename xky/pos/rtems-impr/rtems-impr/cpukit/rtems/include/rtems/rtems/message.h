/**
 *  @file
 *  message.h
 *
 *  @brief contains all the constants and structures associated
 *  with the Message Queue Manager.  This manager provides a mechanism for
 *  communication and synchronization between tasks using messages.
 *
 *  Directives provided are:
 *
 *     + create a queue \n
 *     + get ID of a queue \n
 *     + delete a queue \n
 *     + put a message at the rear of a queue \n
 *     + put a message at the front of a queue \n
 *     + broadcast N messages to a queue \n
 *     + receive message from a queue \n
 *     + flush all messages on a queue \n
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
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @defgroup RTEMS_API_MESSAGE Message Queue Manager
 *
 *  @brief The Message Queue Manager allows the user to send/receive messages from one
 *  thread to another. This allows thread synchronization/communication
 *  @{
 */

#ifndef _RTEMS_RTEMS_MESSAGE_H
#define _RTEMS_RTEMS_MESSAGE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/rtems/types.h>
#include <rtems/rtems/status.h>
#include <rtems/score/chain.h>
#include <rtems/score/object.h>
#include <rtems/rtems/attr.h>
#include <rtems/score/coremsg.h>

   /**
    *  @brief modes in which a message
    *  may be submitted to a message queue.  The message may be posted
    *  in a send or urgent fashion.
    */
   typedef enum
   {
      MESSAGE_QUEUE_SEND_REQUEST = 0 ,
      MESSAGE_QUEUE_URGENT_REQUEST = 1
   } Message_queue_Submit_types;

   /**
    *  @brief define the control block used to manage
    *  each message queue.
    */
   typedef struct
   {
      Objects_Control Object;
      rtems_attribute attribute_set;
      CORE_message_queue_Control message_queue;
   } Message_queue_Control;

   /**
    *  @brief information control block used to
    *  manage this class of objects.
    */
   extern Objects_Information _Message_queue_Information;

   /**
    *  @brief initialize the Message Queue Manager
    *
    *  This routine initializes all message queue manager related
    *  data structures.
    *
    *  @param[in] maximum_message_queues number of message queues to initialize
    */
   void _Message_queue_Manager_initialization(
                                              uint32_t maximum_message_queues
                                              );

   /**
    *  @brief create a message queue
    *
    *  This directive creates a message queue by allocating and initializing
    *  a message queue data structure.
    *
    *  @param[in] name user defined queue name
    *  @param[in] count maximum message and reserved buffer count
    *  @param[in] max_message_size maximum size of each message
    *  @param[in] attribute_set process method
    *  @param[in] id pointer to queue
    *  @param[out] id queue id
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_message_queue_create(
                                                rtems_name name ,
                                                uint32_t count ,
                                                uint32_t max_message_size ,
                                                rtems_attribute attribute_set ,
                                                Objects_Id *id
                                                );

   /**
    *  @brief ident a message queue
    *
    *  This routine implements the rtems_message_queue_ident directive.
    *  This directive returns the message queue ID associated with NAME.
    *  If more than one message queue is named name, then the message
    *  queue to which the ID belongs is arbitrary.  node indicates the
    *  extent of the search for the ID of the message queue named name.
    *  The search can be limited to a particular node or allowed to
    *  encompass all nodes.
    *
    *  @param[in] name the name of the message queue to ident
    *  @param[in] node the multiprocessor node
    *  @param[out] id the address where to place the id
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_message_queue_ident(
                                               rtems_name name ,
                                               uint32_t node ,
                                               Objects_Id *id
                                               );

   /**
    *  @brief delete a message queue
    *
    *  This directive allows a thread to delete the message queue specified
    *  by the given queue identifier.
    *
    *  @param[in] id queue id
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_message_queue_delete(
                                                Objects_Id id
                                                );

   /**
    *  @brief send a message to a message queue
    *
    *  This routine implements the rtems_message_queue_send directive.
    *  This directive sends the message buffer to the message queue
    *  indicated by ID.  If one or more tasks is blocked waiting
    *  to receive a message from this message queue, then one will
    *  receive the message.  The task selected to receive the
    *  message is based on the task queue discipline algorithm in
    *  use by this particular message queue.  If no tasks are waiting,
    *  then the message buffer will be placed at the REAR of the
    *  chain of pending messages for this message queue.
    *
    *  @param[in] id pointer to message queue
    *  @param[in] buffer pointer to message buffer
    *  @param[in] size size of message to sent
    *
    *  @return returns RTEMS_SUCCESSFUL if successful.
    *  the error_code if unsuccessful
    */
   rtems_status_code rtems_message_queue_send(
                                              Objects_Id id ,
                                              void *buffer ,
                                              size_t size
                                              );

   /**
    *  @brief send an urgent message to a message queue
    *
    *  This routine implements the rtems_message_queue_urgent directive.
    *  This directive has the same behavior as rtems_message_queue_send
    *  except that if no tasks are waiting, the message buffer will
    *  be placed at the FRONT of the chain of pending messages rather
    *  than at the REAR.
    *
    *  @param[in] id pointer to message queue
    *  @param[in] buffer pointer to message buffer
    *  @param[in] size size of message to sent urgently
    *
    *  @return returns RTEMS_SUCCESSFUL if successful.
    *  the error_code if unsuccessful
    */
   rtems_status_code rtems_message_queue_urgent(
                                                Objects_Id id ,
                                                void *buffer ,
                                                size_t size
                                                );

   /**
    *  @brief broadcast a message to a message queue
    *
    *  This directive sends a message for every thread waiting on the queue
    *  designated by id.
    *
    *  @param[in] id pointer to message queue
    *  @param[in] buffer pointer to message buffer
    *  @param[in] size size of message to broadcast
    *  @param[out] count pointer to area to store number of threads made ready
    *  of threads made ready
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_message_queue_broadcast(
                                                   Objects_Id id ,
                                                   void *buffer ,
                                                   size_t size ,
                                                   uint32_t *count
                                                   );

   /**
    *  @brief receive a message from a message queue
    *
    *  This directive dequeues a message from the designated message queue
    *  and copies it into the requesting thread's buffer.
    *
    *  @param[in] id queue id
    *  @param[in] buffer pointer to message buffer
    *  @param[in] size size of message receive
    *  @param[in] option_set options on receive
    *  @param[in] timeout number of ticks to wait
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_message_queue_receive(
                                                 Objects_Id id ,
                                                 void *buffer ,
                                                 size_t *size ,
                                                 uint32_t option_set ,
                                                 rtems_interval timeout
                                                 );

   /**
    *  @brief flush the messages of a message queue
    *
    *  This directive removes all pending messages from a queue and returns
    *  the number of messages removed.  If no messages were present then
    *  a count of zero is returned.
    *
    *  @param[in] id queue id
    *  @param[in] count return area for count
    *  @param[out] count number of messages removed ( 0 = empty queue )
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_message_queue_flush(
                                               Objects_Id id ,
                                               uint32_t *count
                                               );

   /**
    *  @brief get the number of pending messages of a message queue
    *
    *  This directive returns the number of messages pending.
    *
    *  @param[in] id queue id
    *  @param[in] count return area for count
    *  @param[out] count number of messages removed ( 0 = empty queue )
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code rtems_message_queue_get_number_pending(
                                                            Objects_Id id ,
                                                            uint32_t *count
                                                            );


   /**
    *  @brief submit a message to a message queue
    *
    *  This routine implements the directives rtems_message_queue_send
    *  and rtems_message_queue_urgent.  It processes a message that is
    *  to be submitted to the designated message queue.  The message will
    *  either be processed as a send send message which it will be inserted
    *  at the rear of the queue or it will be processed as an urgent message
    *  which will be inserted at the front of the queue.
    *
    *  @param[in] id message queue id
    *  @param[in] buffer buffer which contains the message
    *  @param[in] size size of the message in number of bytes
    *  @param[in] submit_type type of submition (urgent or normal)
    *
    *  @return RTEMS_SUCCESSFUL if successful
    *  error code if unsuccessful
    */
   rtems_status_code _Message_queue_Submit(
                                           Objects_Id id ,
                                           void *buffer ,
                                           size_t size ,
                                           Message_queue_Submit_types submit_type
                                           );

   /**
    *  @brief allocate a message queue
    *
    *  Allocate a message queue and the space for its messages
    *
    *  @param[in] count maximum message and reserved buffer count
    *  @param[in] max_message_size maximum size of each message
    *
    *  @return the message queue set if successful, NULL otherwise
    */
   Message_queue_Control *_Message_queue_Allocate(
                                                  uint32_t count ,
                                                  uint32_t max_message_size
                                                  );

   /**
    *  @brief translate the return code of the Message Queue Manager to
    *  rtems_status_code
    *
    *  This function returns a RTEMS status code based on the core message queue
    *  status code specified.
    *
    *  @param[in] the_message_queue_status message_queue status code to translate
    *
    *  @return translated RTEMS status code
    */
   rtems_status_code _Message_queue_Translate_core_message_queue_return_code(
                                                                             uint32_t the_message_queue_status
                                                                             );

#if defined(RTEMS_MULTIPROCESSING)
   /**
    *  @brief support to send a message on a multiprocessing system
    *
    *  @param[in] the_thread the remote thread the message was submitted to
    *  @param[in] id id of the message queue
    */
   void _Message_queue_Core_message_queue_mp_support(
                                                     Thread_Control *the_thread ,
                                                     Objects_Id id
                                                     );
#endif

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/message.inl>
#endif
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/msgmp.h>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

/**  
 *  @}
 */

/**
 *  @}
 */
