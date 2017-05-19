/**
 *  @file
 *  threadq.h
 *
 *  @brief contains all the constants and structures associated
 *  with the manipulation of objects.
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
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

#ifndef _RTEMS_SCORE_THREADQ_H
#define _RTEMS_SCORE_THREADQ_H

/**
 *  @defgroup ScoreThreadQ Thread Queue Handler
 *
 *  @brief This handler encapsulates functionality related to managing sets of threads
 *  blocked waiting for resources.
 */
/**@{*/

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/score/tqdata.h>

#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>

   /**
    *  @brief indefinite wait.
    */
#define THREAD_QUEUE_WAIT_FOREVER  WATCHDOG_NO_TIMEOUT

   /**
    *  @brief define the callout used when a remote task
    *  is extracted from a local thread queue.
    */
   typedef void ( *Thread_queue_Flush_callout )(
                                                 Thread_Control *
                                                 );

   /**
    * @brief define the callout used for timeout processing
    * methods.
    */
   typedef void ( *Thread_queue_Timeout_callout )(
                                                   Objects_Id ,
                                                   void *
                                                   );

   /**
    *  @brief remove a thread from the specified threadq
    *
    *  If the threadq discipline is FIFO, it unblocks a thread, and cancels its
    *  timeout timer.  Priority discipline is processed elsewhere.
    *  INTERRUPT LATENCY:
    *  check sync
    *
    *  @param[in] the_thread_queue pointer to threadq
    *
    *  @return returns thread dequeued or NULL
    */
   Thread_Control *_Thread_queue_Dequeue(
                                         Thread_queue_Control *the_thread_queue
                                         );

   /** @brief enqueue the currently executing thread on
    *  the_thread_queue with an optional timeout.
    */
#define _Thread_queue_Enqueue( _the_thread_queue, _timeout ) \
  _Thread_queue_Enqueue_with_handler( \
    _the_thread_queue, \
    _timeout, \
    _Thread_queue_Timeout )


   /**
    *  @brief block a thread, place it on a thread, and optionally
    *  start a timeout timer.
    *  INTERRUPT LATENCY:
    *  only case
    *
    *  @param[in] the_thread_queue pointer to threadq
    *  @param[in] timeout interval to wait
    *  @param[in] handler the handler to call when a timeout occurs
    */
   void _Thread_queue_Enqueue_with_handler(
                                           Thread_queue_Control* the_thread_queue ,
                                           Watchdog_Interval timeout ,
                                           Thread_queue_Timeout_callout handler
                                           );

   /**
    *  @brief requeue a thread on a thread queue
    *
    *  This routine is invoked when a thread changes priority and should be
    *  moved to a different position on the thread queue.
    *  INTERRUPT LATENCY: NONE
    *
    *  @param[in] the_thread_queue pointer to a threadq header
    *  @param[in] the_thread pointer to a thread control block
    */
   void _Thread_queue_Requeue(
                              Thread_queue_Control *the_thread_queue ,
                              Thread_Control *the_thread
                              );

   /**
    *  @brief extract a thread from a thread queue
    *
    *  This routine removes a specific thread from the specified threadq,
    *  deletes any timeout, and unblocks the thread.
    *  INTERRUPT LATENCY: NONE
    *
    *  @param[in] the_thread_queue pointer to a threadq header
    *  @param[in] the_thread pointer to a thread control block
    */
   void _Thread_queue_Extract(
                              Thread_queue_Control *the_thread_queue ,
                              Thread_Control *the_thread
                              );

   /**
    *  @brief extract a thread from a thread queue even if it is in another node
    *
    *  This routine extracts the_thread from the_thread_queue
    *  and ensures that if there is a proxy for this task on
    *  another node, it is also dealt with.
    *
    *  @param[in] the_thread the thread to extract
    *
    *  @return returns TRUE if the thread was extracted and FALSE otherwise
    */
   boolean _Thread_queue_Extract_with_proxy(
                                            Thread_Control *the_thread
                                            );


   /**
    *  @brief determine the first thread on the queue
    *
    *  This function returns a pointer to the "first" thread
    *  on the_thread_queue.  The "first" thread is selected
    *  based on the discipline of the_thread_queue.
    *
    *  @param[in] the_thread_queue the thread queue from which to extract the first
    *  thread
    *
    *  @return returns the first thread on the queue or NULL if none exists
    */
   Thread_Control *_Thread_queue_First(
                                       Thread_queue_Control *the_thread_queue
                                       );

   /**
    *  @brief flush a thread queue
    *
    *  This kernel routine flushes the given thread queue
    *
    *  @param[in] the_thread_queue pointer to threadq to be flushed
    *  @param[in] remote_extract_callout pointer to routine which extracts a remote thread
    *  @param[in] status status to return to the thread
    */
   void _Thread_queue_Flush(
                            Thread_queue_Control *the_thread_queue ,
#if defined(RTEMS_MULTIPROCESSING)
       Thread_queue_Flush_callout remote_extract_callout ,
#endif
       uint32_t status
                            );

   /**
    *  @brief initialize the specified thread queue
    *
    *  @param[in] the_thread_queue pointer to a threadq header
    *  @param[in] the_discipline queueing discipline
    *  @param[in] state state of waiting threads
    *  @param[in] timeout_status return on a timeout
    */
   void _Thread_queue_Initialize(
                                 Thread_queue_Control *the_thread_queue ,
                                 Thread_queue_Disciplines the_discipline ,
                                 States_Control state ,
                                 uint32_t timeout_status
                                 );

   /**
    *  @brief remove a thread from a thread queue
    *
    *  This routine removes a thread from the specified PRIORITY based
    *  threadq, unblocks it, and cancels its timeout timer.
    *  INTERRUPT LATENCY:
    *  only case
    *
    *  @param[in] the_thread_queue pointer to thread queue
    *
    *  @return returns thread dequeued or NULL
    */
   Thread_Control *_Thread_queue_Dequeue_priority(
                                                  Thread_queue_Control *the_thread_queue
                                                  );

   /**
    *  @brief enqueue a thread on a thread queue with priority protocol
    *
    *  This routine places a blocked thread on a priority thread queue.
    *  INTERRUPT LATENCY:
    *  forward less than
    *  forward equal
    *
    *  @param[in] the_thread_queue pointer to threadq
    *  @param[in] the_thread thread to insert
    */
   void _Thread_queue_Enqueue_priority(
                                       Thread_queue_Control *the_thread_queue ,
                                       Thread_Control *the_thread
                                       );

   /**
    *  @brief remove a specific thread from a specified thread queue
    *
    *  This routine removes a specific thread from the specified threadq,
    *  deletes any timeout, and unblocks the thread.
    *  INTERRUPT LATENCY:
    *  EXTRACT_PRIORITY
    *
    *  @param[in] the_thread_queue pointer to a threadq header
    *  @param[in] the_thread pointer to a thread control block
    *  @param[in] requeuing TRUE if requeuing and should not alter timeout or state
    */
   void _Thread_queue_Extract_priority_helper(
                                              Thread_queue_Control *the_thread_queue ,
                                              Thread_Control *the_thread ,
                                              boolean requeuing
                                              );

   /**
    * @brief remove a specific thread from a specified thread queue without the need
    * to requeue
    *
    * This macro wraps the underlying call and hides the requeuing argument.
    */

#define _Thread_queue_Extract_priority( _the_thread_queue, _the_thread ) \
  _Thread_queue_Extract_priority_helper( _the_thread_queue, _the_thread, FALSE )


   /**
    *  @brief determine the pointer to the first thread on the thread queue
    *
    *  This function returns a pointer to the "first" thread
    *  on the_thread_queue.  The "first" thread is the highest
    *  priority thread waiting on the_thread_queue.
    *
    *  @param[in] the_thread_queue the thread queue from which to extract the first
    *  thread
    *
    *  @return returns the first thread on the queue or NULL if none exists
    */
   Thread_Control *_Thread_queue_First_priority(
                                                Thread_queue_Control *the_thread_queue
                                                );

   /**
    *  @brief remove a thread from a thread queue with FIFO protocol
    *
    *  This routine removes a thread from the specified threadq.
    *  INTERRUPT LATENCY:
    *  check sync
    *  FIFO
    *
    *  @param[in] the_thread_queue pointer to threadq
    *
    *  @return returns thread dequeued or NULL
    */
   Thread_Control *_Thread_queue_Dequeue_fifo(
                                              Thread_queue_Control *the_thread_queue
                                              );

   /**
    *  @brief place a thread on a thread queue with FIFO protocol
    *
    *  This routine places a blocked thread on a FIFO thread queue.
    *  INTERRUPT LATENCY:
    *  only case
    *
    *  @param[in] the_thread_queue pointer to threadq
    *  @param[in] the_thread pointer to the thread to block
    */
   void _Thread_queue_Enqueue_fifo(
                                   Thread_queue_Control *the_thread_queue ,
                                   Thread_Control *the_thread
                                   );

   /**
    *  @brief remove a thread from a thread queue with FIFO protocol
    *
    *  This routine removes a specific thread from the specified threadq,
    *  deletes any timeout, and unblocks the thread.
    *  INTERRUPT LATENCY:
    *  EXTRACT_FIFO
    *
    *  @param[in] the_thread_queue pointer to a threadq header
    *  @param[in] the_thread pointer to the thread to block
    */
   void _Thread_queue_Extract_fifo(
                                   Thread_queue_Control *the_thread_queue ,
                                   Thread_Control *the_thread
                                   );

   /**
    *  @brief determine the first thread on the thread queue with FIFO protocol
    *
    *  This function returns a pointer to the "first" thread
    *  on the_thread_queue.  The first thread is the thread
    *  which has been waiting longest on the_thread_queue.
    *
    *  @param[in] the_thread_queue the thread queue from which to extract the first
    *  thread
    *
    *  @return returns the first thread on the queue or NULL if none exists
    */
   Thread_Control *_Thread_queue_First_fifo(
                                            Thread_queue_Control *the_thread_queue
                                            );

   /**
    *  @brief announce to the thread queue that a timeout has occurred and a thread
    *  should be removed from the queue
    *
    *  This routine is invoked when a task's request has not
    *  been satisfied after the timeout interval specified to
    *  enqueue.  The task represented by ID will be unblocked and
    *  its status code will be set in it's control block to indicate
    *  that a timeout has occurred.
    *
    *  @param[in] id the id of the thread to be removed from the queue due to a timeout
    *  @param[in] ignored this argument is ignored
    */
   void _Thread_queue_Timeout(
                              Objects_Id id ,
                              void *ignored
                              );


#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */

/**  
 *  @}
 */
