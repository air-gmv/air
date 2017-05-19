/**
 *  @file
 *  threadstackfree.c
 *
 *  @brief free the stack of a thread
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
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
#include <rtems/score/threadq.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>


void _Thread_Stack_Free(
                        Thread_Control *the_thread
                        )
{
   /* if the API provided the stack space, then don't free it */
   if(!the_thread->Start.core_allocated_stack)
   {
       /* return */
      return;
   }

   /* call ONLY the CPU table stack free hook, or the
    * the RTEMS workspace free. This is so the free
    * routine properly matches the allocation of the stack */
   if(_CPU_Table.stack_free_hook)
   {
      /* free the stack with the CPU hook */
      ( *_CPU_Table.stack_free_hook )( the_thread->Start.Initial_stack.area );
   }
   else
   {
      /* free the stack memory with the Workspace Handler */
      _Workspace_Free(the_thread->Start.Initial_stack.area);
   }
}

/**  
 *  @}
 */

/**
 *  @}
 */
