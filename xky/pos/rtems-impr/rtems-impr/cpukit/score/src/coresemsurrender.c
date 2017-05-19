/**
 *  @file
 *  coresemsurrender.c
 *
 *  @brief surrender a core semaphore
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
 *  611     | 17/11/2008  | mcoutinho    | IPR 64
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
 *  @addtogroup ScoreSemaphore Semaphore Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/coresem.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#if defined(RTEMS_MULTIPROCESSING)
    #include <rtems/score/mpci.h>
#endif


CORE_semaphore_Status _CORE_semaphore_Surrender(
                                                CORE_semaphore_Control *the_semaphore ,
                                                Objects_Id id
#if defined(RTEMS_MULTIPROCESSING)
    ,
                                                CORE_semaphore_API_mp_support_callout api_semaphore_mp_support
#endif
    )
{
    /* thread which will hold the semaphore (if any) */
    Thread_Control *the_thread;

    /* interrupt level */
    ISR_Level level;

    /* status of the surrender */
    CORE_semaphore_Status status;


    /* set the default status to success */
    status = CORE_SEMAPHORE_STATUS_SUCCESSFUL;

    /* dequeue a thread and check it is exists */
    if(( the_thread = _Thread_queue_Dequeue(&the_semaphore->Wait_queue) ))
    {

#if defined(RTEMS_MULTIPROCESSING)

        /* if the thread is not local */
        if(!_Objects_Is_local_id(the_thread->Object.id))
        {
            /* then call the remote function */
            ( *api_semaphore_mp_support ) ( the_thread , id );
        }
#endif

    }
    else
    {
        /* enter critical section */
        _ISR_Disable(level);

        /* check if the semaphore count is lower than the maximum count specified */
        if(the_semaphore->count < the_semaphore->Attributes.maximum_count)
        {
            /* it is, so can increase the count */
            the_semaphore->count += 1;
        }
        else
        {
            /* it isnt, so return error */
            status = CORE_SEMAPHORE_MAXIMUM_COUNT_EXCEEDED;
        }

        /* leave critical section */
        _ISR_Enable(level);
    }

    /* return the function result */
    return status;
}

/**  
 *  @}
 */

/**
 *  @}
 */
