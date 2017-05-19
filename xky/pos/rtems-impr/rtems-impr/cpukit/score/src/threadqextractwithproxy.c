/**
 *  @file
 *  threadqextractwithproxy.c
 *
 *  @brief remove a thread from a thread queue if it is on a local or remote
 *  node (for multiprocessing)
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
 *  623     | 17/11/2008  | mcoutinho    | IPR 64
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


boolean _Thread_queue_Extract_with_proxy(
                                         Thread_Control *the_thread
                                         )
{
    /* thread state */
    States_Control state;

    /* if multiprocessing is enabled */
#if defined(RTEMS_MULTIPROCESSING)

    /* thread object information */
    Objects_Information *the_information;

    /* proxy extract callout function */
    Objects_Thread_queue_Extract_callout proxy_extract_callout;

#endif


    /* get the thread state */
    state = the_thread->current_state;

    /* check if the thread is waiting on a thread queue */
    if(_States_Is_waiting_on_thread_queue(state))
    {

        /* if multiprocessing is enabled */
#if defined(RTEMS_MULTIPROCESSING)

        /* if the thread is locally blocked and waiting for RPC */
        if(_States_Is_waiting_for_rpc_reply(state) &&
           _States_Is_locally_blocked(state))
        {
            /* then get the object information */
            the_information = _Objects_Get_information(the_thread->Wait.id);

            /* get the proxy callout function */
            proxy_extract_callout =
                (Objects_Thread_queue_Extract_callout) the_information->extract;

            /* if there is a proxy callout function */
            if(proxy_extract_callout)
            {
                /* tehn call the callout function */
                ( *proxy_extract_callout )( the_thread );
            }
        }

#endif

        /* extract the thread from the thread queue */
        _Thread_queue_Extract(the_thread->Wait.queue , the_thread);

        /* and return true */
        return TRUE;
    }

    /* thread was not waiting on a queue */
    return FALSE;
}

/**  
 *  @}
 */

/**
 *  @}
 */
