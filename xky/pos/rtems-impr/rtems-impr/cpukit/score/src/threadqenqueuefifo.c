/**
 *  @file
 *  threadqenqueuefifo.c
 *
 *  @brief enqueue a thread on a FIFO thread queue
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


void _Thread_queue_Enqueue_fifo(
                                Thread_queue_Control *the_thread_queue ,
                                Thread_Control *the_thread
                                )
{
    /* interrupt level */
    ISR_Level level;

    /* thread queue sync state */
    Thread_queue_States sync_state;


    /* enter critical section */
    _ISR_Disable(level);

    /* get the old thread queue state */
    sync_state = the_thread_queue->sync_state;

    /* set the thread queue state to synchronized */
    the_thread_queue->sync_state = THREAD_QUEUE_SYNCHRONIZED;

    /* check the old thread queue sync state */
    switch(sync_state)
    {

            /* sync state was synchronized */
        case THREAD_QUEUE_SYNCHRONIZED:

            /* this should never happen.  It indicates that someone did not
             * enter a thread queue critical section */
            break;

             /* sync state was nothing happened */
        case THREAD_QUEUE_NOTHING_HAPPENED:

            /* append the thread to the FIFO queue */
            _Chain_Append_unprotected(&the_thread_queue->Queues.Fifo ,
                                      &the_thread->Object.Node);

            /* set the thread queue into the thread */
            the_thread->Wait.queue = the_thread_queue;

            /* leave critical section */
            _ISR_Enable(level);

            /* and return */
            return;

             /* sync state was timeout */
        case THREAD_QUEUE_TIMEOUT:

            /* set the thread return code */
            the_thread->Wait.return_code = the_thread->Wait.queue->timeout_status;

            /* thread is no longer waiting on thread queue */
            the_thread->Wait.queue = NULL;

            /* leave critical section */
            _ISR_Enable(level);

            /* and leave */
            break;

             /* sync state was satisfied */
        case THREAD_QUEUE_SATISFIED:

            /* check if there is an active watchdog */
            if(_Watchdog_Is_active(&the_thread->Timer))
            {
                /* there is an active watchdog, so */

                /* deactivate the watchdog */
                _Watchdog_Deactivate(&the_thread->Timer);

                /* reset the thread wait queue */
                the_thread->Wait.queue = NULL;

                /* leave critical section */
                _ISR_Enable(level);

                /* and remove the thread watchdog */
                (void) _Watchdog_Remove(&the_thread->Timer);
            }
            else
            {
                /* no active watchdog */

                /* leave critical section */
                _ISR_Enable(level);
            }

            /* and leave */
            break;

            /* only the above disciplines are defined */
        default:

            /* raise internal error */
            _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                     TRUE ,
                                     INTERNAL_ERROR_INVALID_THREAD_QUEUE_STATE);
    }

    /* unblock the thread */
    _Thread_Unblock(the_thread);

#if defined(RTEMS_MULTIPROCESSING)

    /* global objects with thread queue's should not be operated on from an
     * ISR.  But the sync code still must allow short timeouts to be processed
     * correctly */
    if(!_Objects_Is_local_id(the_thread->Object.id))
    {
        _Thread_MP_Free_proxy(the_thread);
    }
    
#endif

}

/**  
 *  @}
 */

/**
 *  @}
 */
