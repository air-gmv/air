/**
 *  @file
 *  threadq.c
 *
 *  @brief Initializes the Thread Queue Handler
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


void _Thread_queue_Initialize(
                              Thread_queue_Control *the_thread_queue ,
                              Thread_queue_Disciplines the_discipline ,
                              States_Control state ,
                              uint32_t timeout_status
                              )
{
    /* header iterator */
   uint32_t index;

   
   /* initialize the thread queue state to the thread state */
   the_thread_queue->state = state;

   /* initialize the thread queue discipline */
   the_thread_queue->discipline = the_discipline;

   /* initialize the thread queue timeout status */
   the_thread_queue->timeout_status = timeout_status;

   /* initialize the thread queue sync state to synchronized */
   the_thread_queue->sync_state = THREAD_QUEUE_SYNCHRONIZED;

   /* check the thread queue discipline */
   switch(the_discipline)
   {
       /* discipline is FIFO */
      case THREAD_QUEUE_DISCIPLINE_FIFO:

         /* initialize the thread queue FIFO chain */
         _Chain_Initialize_empty(&the_thread_queue->Queues.Fifo);

         /* and leave */
         break;

         /* discipline is priority */
      case THREAD_QUEUE_DISCIPLINE_PRIORITY:

          /* loop through every priority header */
         for(index = 0;
             index < TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS;
             index++)
         {
            /* initialize each of the priority headers chains */
            _Chain_Initialize_empty(&the_thread_queue->Queues.Priority[index]);
         }

         /* and leave */
         break;

         /* invalid thread queue discipline */
      default:
          
         /* raise internal error */
         _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                  TRUE ,
                                  INTERNAL_ERROR_INVALID_THREAD_QUEUE_DISCIPLINE);
   }

}

/**  
 *  @}
 */

/**
 *  @}
 */
