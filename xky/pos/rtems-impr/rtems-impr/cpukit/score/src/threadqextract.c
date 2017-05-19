/**
 *  @file
 *  threadqextract.c
 *
 *  @brief removes a thread from a thread queue
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


void _Thread_queue_Extract(
                           Thread_queue_Control *the_thread_queue ,
                           Thread_Control *the_thread
                           )
{
   /* check the thread queue discipline */
   switch(the_thread_queue->discipline)
   {
         /* discipline is FIFO */
      case THREAD_QUEUE_DISCIPLINE_FIFO:

         /* extract the thread from the FIFO queue */
         _Thread_queue_Extract_fifo(the_thread_queue , the_thread);

         /* and leave */
         break;

         /* discipline is priority */
      case THREAD_QUEUE_DISCIPLINE_PRIORITY:

         /* extract the thread from the priority queue */
         _Thread_queue_Extract_priority(the_thread_queue , the_thread);

         /* and leave */
         break;

         /* only the above disciplines are defined */
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
