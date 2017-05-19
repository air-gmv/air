/**
 *  @file
 *  threadstart.c
 *
 *  @brief start a thread
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


boolean _Thread_Start(
                      Thread_Control *the_thread ,
                      Thread_Start_types the_prototype ,
                      void *entry_point ,
                      void *pointer_argument ,
                      uint32_t numeric_argument
                      )
{
    /* check if the thread state is dormant (never been started) */
    if(_States_Is_dormant(the_thread->current_state))
    {
        /* set the thread entry point */
        the_thread->Start.entry_point = (Thread_Entry) entry_point;

        /* set the thread prototype */
        the_thread->Start.prototype = the_prototype;

        /* set the thread pointer argument */
        the_thread->Start.pointer_argument = pointer_argument;

        /* set the thread numeric argument */
        the_thread->Start.numeric_argument = numeric_argument;

        /* load the thread environment */
        _Thread_Load_environment(the_thread);

        /* change the thread state to ready */
        _Thread_Ready(the_thread);

        /* call user thread start extensions */
        _User_extensions_Thread_start(the_thread);

        /* thread has been started */
        return TRUE;
    }

    /* if the thread has already been started return FALSE */
    return FALSE;
}

/**  
 *  @}
 */

/**
 *  @}
 */
