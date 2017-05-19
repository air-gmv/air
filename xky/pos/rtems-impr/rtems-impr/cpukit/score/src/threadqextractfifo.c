/**
 *  @file
 *  threadqextractfifo.c
 *
 *  @brief remove a thread from a FIFO thread queue
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


void _Thread_queue_Extract_fifo(
                                Thread_queue_Control *the_thread_queue ,
                                Thread_Control *the_thread
                                )
{
    /* interrupt level */
    ISR_Level level;


    /* enter critical section */
    _ISR_Disable(level);

    /* check if there was a thread waiting on the thread queue */
    if(!_States_Is_waiting_on_thread_queue(the_thread->current_state))
    {
        /* leave critical section */
        _ISR_Enable(level);

        /* and return */
        return;
    }

    /* extract the thread from the chain */
    _Chain_Extract_unprotected(&the_thread->Object.Node);

    /* thread is no longer waiting on queue */
    the_thread->Wait.queue = NULL;

    /* check if there was an active watchdog */
    if(!_Watchdog_Is_active(&the_thread->Timer))
    {
        /* no, then just re-enable interrupts */
        _ISR_Enable(level);
    }
    else
    {
        /* deactivate the watchdog */
        _Watchdog_Deactivate(&the_thread->Timer);

        /* re-enable interrupts */
        _ISR_Enable(level);

        /* and remove the watchdog */
        (void) _Watchdog_Remove(&the_thread->Timer);
    }

    /* unblock the thread */
    _Thread_Unblock(the_thread);

#if defined(RTEMS_MULTIPROCESSING)

    /* if the thread is not local */
    if(!_Objects_Is_local_id(the_thread->Object.id))
    {
        /* then extract it remotely */
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

