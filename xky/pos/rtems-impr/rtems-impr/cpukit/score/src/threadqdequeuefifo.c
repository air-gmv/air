/**
 *  @file
 *  threadqdequeuefifo.c
 *
 *  @brief dequeue a thread from a FIFO thread queue
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
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
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
 *  @addtogroup ScoreThreadQ Thread Queue Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tqdata.h>


Thread_Control *_Thread_queue_Dequeue_fifo(
                                           Thread_queue_Control *the_thread_queue
                                           )
{
    /* interrupt level */
    ISR_Level level;

    /* thread to dequeue */
    Thread_Control *the_thread;


    /* enter critical section */
    _ISR_Disable(level);

    /* check if the FIFO queue is not empty */
    if(!_Chain_Is_empty(&the_thread_queue->Queues.Fifo))
    {
        /* get the first thread of the chain */
        the_thread = (Thread_Control *)
            _Chain_Get_first_unprotected(&the_thread_queue->Queues.Fifo);

        /* the thread is no longer on a queue */
        the_thread->Wait.queue = NULL;

        /* check if the watchdog is active */
        if(!_Watchdog_Is_active(&the_thread->Timer))
        {
            /* leave critical section */
            _ISR_Enable(level);

            /* if it is not active then unblock the thread */
            _Thread_Unblock(the_thread);
        }
        else
        {
            /* if it is active, then deactive it */
            _Watchdog_Deactivate(&the_thread->Timer);

            /* leave critical section */
            _ISR_Enable(level);

            /* and remove it */
            (void) _Watchdog_Remove(&the_thread->Timer);

            /* and unblock the thread */
            _Thread_Unblock(the_thread);
        }

        /* if multiprocessing is enabled */
#if defined(RTEMS_MULTIPROCESSING)

        /* if the thread is not local */
        if(!_Objects_Is_local_id(the_thread->Object.id))
        {
            /* then free it remotely */
            _Thread_MP_Free_proxy(the_thread);
        }
#endif

        /* return the thread that was dequeued */
        return the_thread;
    }

    /* the chain is empty, no thread to dequeue */

    /* check the thread queue sync state */
    switch(the_thread_queue->sync_state)
    {

            /* synchronized or satisfied */
        case THREAD_QUEUE_SYNCHRONIZED:
        case THREAD_QUEUE_SATISFIED:

            /* leave critical section */
            _ISR_Enable(level);

            /* return NULL (no thread) */
            return NULL;

            /* nothing happended or timeout */
        case THREAD_QUEUE_NOTHING_HAPPENED:
        case THREAD_QUEUE_TIMEOUT:

            /* an interrupt occurred which made the executing thread the one 
             * to dequeue */

            /* set the thread queue state to satisfied */
            the_thread_queue->sync_state = THREAD_QUEUE_SATISFIED;

            /* leave critical section */
            _ISR_Enable(level);

            /* return the executing thread */
            return _Thread_Executing;

            /* only the above disciplines are defined */
        default:

            /* raise internal error */
            _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                     TRUE ,
                                     INTERNAL_ERROR_INVALID_THREAD_QUEUE_STATE);

            /* return NULL (no thread) */
            return NULL;
    }
}

/**  
 *  @}
 */

/**
 *  @}
 */
