/**
 *  @file
 *  coremsg.h
 *
 *  @brief contains all the constants and structures associated
 *  with the Message queue Handler.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  600     | 17/11/2008  | mcoutinho    | IPR 64
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

#ifndef _RTEMS_SCORE_COREMSG_H
#define _RTEMS_SCORE_COREMSG_H

/**
 *  @defgroup ScoreMessageQueue Message Queue Handler
 *
 *   @brief The Message Queue handler encapsulates functionality which provides the
 *  foundation Message Queue services used in all of the APIs supported by
 *  RTEMS.
 *
 *  The message queue core object provides task synchronization and
 *  communication functions via messages passed to queue objects.
 */
/**@{*/

#ifdef __cplusplus
extern "C"
{
#endif

#include <limits.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/priority.h>
#include <rtems/score/watchdog.h>

#if defined(RTEMS_MULTIPROCESSING)
   /**
    *  @brief  Message Queue MP Callback Prototype
    *
    *  The following type defines the callout which the API provides
    *  to support global/multiprocessor operations on message_queues.
    */
   typedef void ( *CORE_message_queue_API_mp_support_callout )(
                                                                Thread_Control * ,
                                                                Objects_Id
                                                                );
#endif

   /**
    *  @brief core message queue buffer
    *
    *  The following defines the data types needed to manipulate
    *  the contents of message buffers.
    *
    *  @note  The buffer field is normally longer than a single uint32_t
    *         but since messages are variable length we just make a ptr to 1.
    */
   typedef struct
   {
      /**
       * @brief This field is the size of this message.
       */
      size_t size;
      /**
       * @brief This field contains the actual message.
       */
      uint32_t buffer[1];
   } CORE_message_queue_Buffer;

   /**
    *  @brief core message
    *
    *  The following records define the organization of a message buffer.
    */
   typedef struct
   {
      /**
       * @brief This element allows this structure to be placed on chains.
       */
      Chain_Node Node;
      /**
       * @brief This field is the priority of this message.
       */
      int priority;
      /**
       * @brief This field points to the contents of the message.
       */
      CORE_message_queue_Buffer Contents;
   } CORE_message_queue_Buffer_control;

   /**
    *  @brief Message Queue Blocking Disciplines
    *
    *  This enumerated types defines the possible blocking disciplines
    *  for a message queue.
    */
   typedef enum
   {
      /**
       * @brief This value indicates that pending messages are in FIFO order.
       */
      CORE_MESSAGE_QUEUE_DISCIPLINES_FIFO ,
      /**
       * @brief This value indicates that pending messages are in priority order.
       */
      CORE_MESSAGE_QUEUE_DISCIPLINES_PRIORITY
   } CORE_message_queue_Disciplines;

   /**
    *  @brief Message Priority for Appending
    *
    *  This is the priority constant used when appending messages onto
    *  a message queue.
    */
#define  CORE_MESSAGE_QUEUE_SEND_REQUEST   INT_MAX

   /**
    *  @brief Message Priority for Prepending
    *
    *  This is the priority constant used when prepending messages onto
    *  a message queue.
    */
#define  CORE_MESSAGE_QUEUE_URGENT_REQUEST INT_MIN

   /**
    *  @brief Message Insertion Operation Types
    *
    *  The following type details the modes in which a message
    *  may be submitted to a message queue.  The message may be posted
    *  in a send or urgent fashion.
    *
    *  @note  All other values are message priorities.  Numerically smaller
    *         priorities indicate higher priority messages.
    */
   typedef int CORE_message_queue_Submit_types;

   /**
    *  @brief Core Message Queue Return Statuses
    *
    *  This enumerated type defines the possible set of Core Message
    *  Queue handler return statuses.
    */
   typedef enum
   {
      /**
       * @brief This value indicates the operation completed sucessfully.
       */
      CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL ,
      /**
       * @brief This value indicates that the message was too large for this queue.
       */
      CORE_MESSAGE_QUEUE_STATUS_INVALID_SIZE ,
      /**
       * @brief This value indicates that there are too many messages pending.
       */
      CORE_MESSAGE_QUEUE_STATUS_TOO_MANY ,
      /**
       * @brief This value indicates that a receive was unsuccessful.
       */
      CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED ,
      /**
       * @brief This value indicates that a blocking send was unsuccessful.
       */
      CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_NOWAIT ,
      /**
       *  @brief This value indicates that the message queue being blocked upon
       *  was deleted while the thread was waiting.
       */
      CORE_MESSAGE_QUEUE_STATUS_WAS_DELETED ,
      /**
       *  @brief This value indicates that the thread had to timeout while waiting
       *  to receive a message because one did not become available.
       */
      CORE_MESSAGE_QUEUE_STATUS_TIMEOUT ,
      /**
       * @brief This value indicates that a blocking receive was unsuccessful.
       */
      CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_WAIT
   } CORE_message_queue_Status;

   /**
    *  @brief Message Queue Attributes Type
    *
    *  The following defines the control block used to manage the
    *  attributes of each message queue.
    */
   typedef struct
   {
      /**
       * @brief This field specifies the order in which blocking tasks will be
       * ordered.
       */
      CORE_message_queue_Disciplines discipline;
   } CORE_message_queue_Attributes;

   /**
    *  @brief Message Queue Notification Callback Prototype
    *
    *  The following defines the type for a Notification handler.  A notification
    *  handler is invoked when the message queue makes a 0->1 transition on
    *  pending messages.
    */
   typedef void (*CORE_message_queue_Notify_Handler )(void *);

   /**
    *  @brief Core Message Queue Control Structure
    *
    *  The following defines the control block used to manage each
    *  Message Queue
    */
   typedef struct
   {
      /**
       *  @brief This field is the Waiting Queue used to manage the set of tasks
       *  which are blocked waiting to receive a message from this queue.
       */
      Thread_queue_Control Wait_queue;
      /**
       *  @brief This element is the set of attributes which define this instance's
       *  behavior.
       */
      CORE_message_queue_Attributes Attributes;
      /**
       *  @brief This element is maximum number of messages which may be pending
       *  at any given time.
       */
      uint32_t maximum_pending_messages;
      /**
       * @brief This element is the number of messages which are currently pending.
       */
      uint32_t number_of_pending_messages;
      /**
       *  @brief This is the size in bytes of the largest message which may be
       *  sent via this queue.
       */
      uint32_t maximum_message_size;
      /**
       *  @brief This chain is the set of pending messages.  It may be ordered by
       *  message priority or in FIFO order.
       */
      Chain_Control Pending_messages;
      /**
       *  @brief This is the address of the memory allocated for message buffers.
       *  It is allocated are part of message queue initialization and freed
       *  as part of destroying it.
       */
      CORE_message_queue_Buffer *message_buffers;
      /**
       *  @brief This is the routine invoked when the message queue transitions
       *  from zero (0) messages pending to one (1) message pending.
       */
      CORE_message_queue_Notify_Handler notify_handler;
      /**
       * @brief This field is the argument passed to the notify_argument.
       */
      void *notify_argument;
      /**
       *  @brief This chain is the set of inactive messages.  A message is inactive
       *  when it does not contain a pending message.
       */
      Chain_Control Inactive_messages;
   } CORE_message_queue_Control;

   /**
    *  @brief Initialize a Message Queue
    *
    *  This routine initializes @a the_message_queue based on the parameters passed.
    *
    *  @param[in] the_message_queue points to the message queue to initialize
    *  @param[in] the_message_queue_attributes points to the attributes that
    *  will be used with this message queue instance
    *  @param[in] maximum_pending_messages is the maximum number of messages
    *  that will be allowed to pend at any given time
    *  @param[in] maximum_message_size is the size of largest message that
    *  may be sent to this message queue instance
    *
    *  @return TRUE if the message queue can be initialized.  In general,
    *  FALSE will only be returned if memory for the pending
    *  messages cannot be allocated.
    */
   boolean _CORE_message_queue_Initialize(
                                          CORE_message_queue_Control *the_message_queue ,
                                          CORE_message_queue_Attributes *the_message_queue_attributes ,
                                          uint32_t maximum_pending_messages ,
                                          uint32_t maximum_message_size
                                          );

   /**
    *  @brief close a message queue
    *
    *  This function closes a message by returning all allocated space and
    *  flushing @a the_message_queue's task wait queue.
    *
    *  @param[in] the_message_queue points to the message queue to close
    *  @param[in] remote_extract_callout is the routine to call for each thread
    *  that is extracted from the set of waiting threads
    *  @param[in] status is the status that each waiting thread will return
    *  from it's blocking service
    */
   void _CORE_message_queue_Close(
                                  CORE_message_queue_Control *the_message_queue ,
#if defined(RTEMS_MULTIPROCESSING)
       Thread_queue_Flush_callout remote_extract_callout ,
#endif
       uint32_t status
                                  );

   /**
    *  @brief flush a message queue
    *
    *  This function flushes the message_queue's pending message queue.  The
    *  number of messages flushed from the queue is returned.
    *
    *  @param[in] the_message_queue the message_queue to be flushed
    *
    *  @return returns the number of messages flushed from the queue
    */
   uint32_t _CORE_message_queue_Flush(
                                      CORE_message_queue_Control *the_message_queue
                                      );

   /**
    *  @brief support the flushing of a message queue
    *
    *  This message handler routine removes all messages from a message queue
    *  and returns them to the inactive message pool.  The number of messages
    *  flushed from the queue is returned
    *  INTERRUPT LATENCY:
    *  only case
    *
    *  @param[in] the_message_queue pointer to message queue
    *
    *  @return returns number of messages placed on inactive chain
    */
   uint32_t _CORE_message_queue_Flush_support(
                                              CORE_message_queue_Control *the_message_queue
                                              );

   /**
    *  @brief flush waiting threads
    *
    *  This function flushes the threads which are blocked on
    *  @a the_message_queue's pending message queue.  They are
    *  unblocked whether blocked sending or receiving.
    *
    *  @param[in] the_message_queue points to the message queue to flush
    */
   void _CORE_message_queue_Flush_waiting_threads(
                                                  CORE_message_queue_Control *the_message_queue
                                                  );

   /**
    *  @brief broadcast a message to the message queue
    *
    *  This function sends a message for every thread waiting on the queue and
    *  returns the number of threads made ready by the message.
    *
    *  @param[in] the_message_queue points to the message queue
    *  @param[in] buffer is the starting address of the message to broadcast
    *  @param[in] size is the size of the message being broadcast
    *  @param[in] id is the RTEMS object Id associated with this message queue.
    *  It is used when unblocking a remote thread.
    *  @param[in] api_message_queue_mp_support is the routine to invoke if
    *  a thread that is unblocked is actually a remote thread.
    *  @param[out] count points to the variable that will contain the
    *  number of tasks that are sent this message
    *
    *  @return @a *count will contain the number of messages sent
    *  indication of the successful completion or reason for failure
    */
   CORE_message_queue_Status _CORE_message_queue_Broadcast(
                                                           CORE_message_queue_Control *the_message_queue ,
                                                           void *buffer ,
                                                           size_t size ,
                                                           Objects_Id id ,
#if defined(RTEMS_MULTIPROCESSING)
       CORE_message_queue_API_mp_support_callout api_message_queue_mp_support ,
#endif
       uint32_t *count
                                                           );

   /**
    *  @brief submit a message to the message queue
    *
    *  This routine implements the send and urgent message functions. It
    *  processes a message that is to be submitted to the designated
    *  message queue.  The message will either be processed as a
    *  send message which it will be inserted at the rear of the queue
    *  or it will be processed as an urgent message which will be inserted
    *  at the front of the queue.
    *
    *  @param[in] the_message_queue points to the message queue
    *  @param[in] buffer is the starting address of the message to send
    *  @param[in] size is the size of the message being send
    *  @param[in] id is the RTEMS object Id associated with this message queue.
    *  It is used when unblocking a remote thread.
    *  @param[in] api_message_queue_mp_support is the routine to invoke if
    *  a thread that is unblocked is actually a remote thread.
    *  @param[in] submit_type determines whether the message is prepended,
    *  appended, or enqueued in priority order.
    *  @param[in] wait indicates whether the calling thread is willing to block
    *  if the message queue is full.
    *  @param[in] timeout is the maximum number of clock ticks that the calling
    *  thread is willing to block if the message queue is full.
    *
    *  @return indication of the successful completion or reason for failure
    */
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
                                                        );

   /**
    *  @brief determine the size a message from the message queue
    *
    *  This kernel routine dequeues a message, copies the message buffer to
    *  a given destination buffer, and frees the message buffer to the
    *  inactive message pool.  The thread will be blocked if wait is TRUE,
    *  otherwise an error will be given to the thread if no messages are available.
    *
    *  @param[in] the_message_queue points to the message queue
    *  @param[in] id is the RTEMS object Id associated with this message queue.
    *  It is used when unblocking a remote thread.
    *  @param[in] buffer is the starting address of the message buffer to
    *  to be filled in with a message
    *  @param[in] size is the size of the @a buffer and indicates the maximum
    *  size message that the caller can receive.
    *  @param[in] wait indicates whether the calling thread is willing to block
    *  if the message queue is empty.
    *  @param[in] timeout is the maximum number of clock ticks that the calling
    *  thread is willing to block if the message queue is empty.
    *
    *  @note Returns message priority via return are in TCB.
    */
   void _CORE_message_queue_Seize(
                                  CORE_message_queue_Control *the_message_queue ,
                                  Objects_Id id ,
                                  void *buffer ,
                                  size_t *size ,
                                  boolean wait ,
                                  Watchdog_Interval timeout
                                  );

   /**
    *  @brief insert a message into the message queue
    *
    *  This kernel routine inserts the specified message into the
    *  message queue.  It is assumed that the message has been filled
    *  in before this routine is called.
    *  INTERRUPT LATENCY:
    *  insert
    *
    *  @param[in] the_message_queue pointer to message queue
    *  @param[in] the_message message to insert
    *  @param[in] submit_type insert indication
    */
   void _CORE_message_queue_Insert_message(
                                           CORE_message_queue_Control *the_message_queue ,
                                           CORE_message_queue_Buffer_control *the_message ,
                                           CORE_message_queue_Submit_types submit_type
                                           );

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/coremsg.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/*  end of include file */

/**  
 *  @}
 */
