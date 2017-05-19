/**
 *  @file
 *  coresemflush.c
 *
 *  @brief flush a core semaphore
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


void _CORE_semaphore_Flush(
                           CORE_semaphore_Control *the_semaphore ,
#if defined(RTEMS_MULTIPROCESSING)
    Thread_queue_Flush_callout remote_extract_callout ,
#endif
    uint32_t status
                           )
{
   /* flush the semaphore thread queue with :
    *   the semaphore wait queue
    *   if multiprocessing, the remote callout extract function
    *   the status */
   _Thread_queue_Flush(&the_semaphore->Wait_queue ,
                       /* if multiprocessing */
#if defined(RTEMS_MULTIPROCESSING)
                       /* then call remote extract callout function */
       remote_extract_callout ,
#endif
       status);

}

/**  
 *  @}
 */

/**
 *  @}
 */
