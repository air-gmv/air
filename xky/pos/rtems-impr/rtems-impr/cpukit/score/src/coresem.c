/**
 *  @file
 *  coresem.c
 *
 *  @brief initialize a core semaphore
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
 *  5247    | 30/10/2009  | mcoutinho    | IPR 828
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


void _CORE_semaphore_Initialize(
                                CORE_semaphore_Control *the_semaphore ,
                                CORE_semaphore_Attributes *the_semaphore_attributes ,
                                uint32_t initial_value
                                )
{
    /* semaphore discipline */
    Thread_queue_Disciplines discipline;

    /* initialize the semaphore fields */

    /* initialize the semaphore attributes */
    the_semaphore->Attributes = *the_semaphore_attributes;

    /* initialize the semaphore initial count */
    the_semaphore->count = initial_value;

    /* if the semaphore queueing discipline is priority */
    if(_CORE_semaphore_Is_priority(the_semaphore_attributes))
    {
        /* then discipline is priority */
        discipline = THREAD_QUEUE_DISCIPLINE_PRIORITY;
    }
    else
    {
        /* else, discipline is FIFO */
        discipline = THREAD_QUEUE_DISCIPLINE_FIFO;
    }

    /* initialize the thread queue of the semaphore */
    _Thread_queue_Initialize(&the_semaphore->Wait_queue ,
                             discipline ,
                             STATES_WAITING_FOR_SEMAPHORE ,
                             CORE_SEMAPHORE_TIMEOUT);
}

/**  
 *  @}
 */

/**
 *  @}
 */
