/**
 *  @file
 *  threadstackallocate.c
 *
 *  @brief allocate the stack for a thread
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
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
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


size_t _Thread_Stack_Allocate(
                              Thread_Control *the_thread ,
                              size_t stack_size
                              )
{
    /* stack address (initialized to NULL) */
    void *stack_addr = 0;

    /* stack size (initialized to the request stack size) */
    size_t the_stack_size = stack_size;

    /* check the stack size and correct it if needed */
    if(!_Stack_Is_enough(the_stack_size))
    {
        /* stack size is the minimum */
        the_stack_size = STACK_MINIMUM_SIZE;
    }

    /* call ONLY the CPU table stack allocate hook, _or_ the
     * the RTEMS workspace allocate.  This is so the stack free
     * routine can call the correct deallocation routine */
    if(_CPU_Table.stack_allocate_hook)
    {
        /* get the stack address calculated by the CPU stack allocate hook */
        stack_addr = ( *_CPU_Table.stack_allocate_hook )( the_stack_size );
    }
    else
    {
        /* First pad the requested size so we allocate enough memory
         * so the context initialization can align it properly.  The address
         * returned the workspace allocate must be directly stored in the
         * stack control block because it is later used in the free sequence.
         * Thus it is the responsibility of the CPU dependent code to
         * get and keep the stack adjust factor, the stack alignment, and
         * the context initialization sequence in sync */

        /* adjust the size */
        the_stack_size = _Stack_Adjust_size(the_stack_size);

        /* allocate the stack */
        stack_addr = _Workspace_Allocate(the_stack_size);
    }

    /* if stack could not be allocated */
    if(!stack_addr)
    {
        /* return 0 */
        the_stack_size = 0;
    }

    /* initialize the thread stack to the address */
    the_thread->Start.stack = stack_addr;

    /* return the stack size [bytes] */
    return the_stack_size;
}

/**  
 *  @}
 */

/**
 *  @}
 */
