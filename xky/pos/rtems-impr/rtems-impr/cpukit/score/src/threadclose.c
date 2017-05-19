/**
 *  @file
 *  threadclose.c
 *
 *  @brief close a thread
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
 *  620     | 17/11/2008  | mcoutinho    | IPR 64
 *  4343    | 15/09/2009  | mcoutinho    | IPR 99
 *  4343    | 15/09/2009  | mcoutinho    | IPR 553
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


void _Thread_Close(
                   Objects_Information *information ,
                   Thread_Control *the_thread
                   )
{
    /* call user extensions on thread delete */
    _User_extensions_Thread_delete(the_thread);

    /* release all mutexes held by this thread */
    _Thread_Free_mutexes(the_thread);

    /* leave the list of blocked threads*/
    if(the_thread->Wait.node.next != NULL || the_thread->Wait.node.previous != NULL)
    {
        /* extract the thread wait node */
        _Chain_Extract(&the_thread->Wait.node);
    }

    /* close the thread object information */
    _Objects_Close(information , &the_thread->Object);

    /* set the thread state to transient */
    _Thread_Set_state(the_thread , STATES_TRANSIENT);

    /* remove the thread from the thread queue (if any) */
    if(!_Thread_queue_Extract_with_proxy(the_thread))
    {
        /* check to see if the thread was waiting on a queue with a timeout */
        if(_Watchdog_Is_active(&the_thread->Timer))
        {
            /* if it was, remove the thread watchdog */
            (void) _Watchdog_Remove(&the_thread->Timer);
        }
    }

    /* if the CPU has hardware or software FP */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )

    /* if use deferred FP */
    #if ( CPU_USE_DEFERRED_FP_SWITCH == TRUE )

    /* check if the thread has FP */
    if(_Thread_Is_allocated_fp(the_thread))
    {
        /* it has, then deallocated it */
        _Thread_Deallocate_fp();
    }
    
    #endif

    /* reset the thread FP context */
    the_thread->fp_context = NULL;

    /* if the thread has FP context */
    if(the_thread->Start.fp_context)
    {
        /* then free it */
        (void) _Workspace_Free(the_thread->Start.fp_context);
    }

#endif

    /* free the thread stack */
    _Thread_Stack_Free(the_thread);

    /* if there are thread extensions */
    if(the_thread->extensions)
    {
        /* free any extensions the thread has */
        (void) _Workspace_Free(the_thread->extensions);
    }

    /* reset the thread stack */
    the_thread->Start.stack = NULL;

    /* reset the thread extensions */
    the_thread->extensions = NULL;
}

/**  
 *  @}
 */

/**
 *  @}
 */
