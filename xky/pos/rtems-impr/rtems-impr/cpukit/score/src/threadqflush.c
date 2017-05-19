/**
 *  @file
 *  threadqflush.c
 *
 *  @brief flush a thread queue
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
 *  3757    | 13/07/2009  | mcoutinho    | IPR 553
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


void _Thread_queue_Flush(
                         Thread_queue_Control *the_thread_queue ,
#if defined(RTEMS_MULTIPROCESSING)
    Thread_queue_Flush_callout remote_extract_callout ,
#endif
    uint32_t status
                         )
{
    /* thread to flush */
    Thread_Control *the_thread;


    /* iterate through all the threads on the queue
     * and extract them */
    while(( the_thread = _Thread_queue_Dequeue(the_thread_queue) ))
    {

        /* if multiprocessing is enabled */
#if defined(RTEMS_MULTIPROCESSING)

        /* if thread is not local */
        if(!_Objects_Is_local_id(the_thread->Object.id))
        {
            /* remotely flush the thread */
            ( *remote_extract_callout )( the_thread );
        }
        else
        {

#endif
            /* set the thread return code */
            the_thread->Wait.return_code = status;

            /* if multiprocessing is enabled */
#if defined(RTEMS_MULTIPROCESSING)

            /* end bracket */
        }
        
#endif

    }
}

/**  
 *  @}
 */

/**
 *  @}
 */
