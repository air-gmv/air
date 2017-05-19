/**
 *  @file
 *  threadfreemutexes.c
 *
 *  @brief free all mutexes own by a thread
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 2008
 *  EDISOFT - Empresa de Serviços e Desenvolvimento de Software, S.A.
 *
 *  http://rtemscentre.edisoft.pt
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  3908    | 21/07/2009  | mcoutinho    | IPR 553
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9877 $ | $Date: 2011-03-18 18:39:36 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreThread Thread Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/apiext.h>
#include <rtems/score/context.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/states.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/thread.h>
#include <rtems/score/coremutex.h>
#include <rtems/rtems/sem.h>


void _Thread_Free_mutexes(Thread_Control *thread)
{
    /* iterator node */
    Chain_Node *the_node;
    

    /* for every mutex on the list of hold mutexes by the thread */
    while(!_Chain_Is_empty(&thread->Wait.mutexes_hold))
    {
        /* get the first node */
        the_node = thread->Wait.mutexes_hold.first;

        /* get the mutex
         * @note the node is the first position of the mutex control block, so
         * we can cast the node to a mutex */
        CORE_mutex_Control *mutex = (CORE_mutex_Control *) the_node;

        /* get the semaphore associated with this mutex */
        Semaphore_Control *semaphore = (Semaphore_Control *) mutex->semaphore;

        /* surrender the mutex with the semaphore identifier
         * @note this will remove the node from the chain */
        _CORE_mutex_Surrender(mutex , semaphore->Object.id , TRUE);

        /* in the next iteration the chain has one less node */
    }
}

/**
 *  @}
 */

/**
 *  @}
 */
