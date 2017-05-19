/**
 *  @file
 *  threadstartmultitasking.c
 *
 *  @brief start thread multitasking
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2006.
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


void _Thread_Start_multitasking(void)
{
    /* the system is now multitasking and completely initialized.
     * This system thread now either "goes away" in a single processor
     * system or "turns into" the server thread in an MP system */

    /* set the system state to UP */
    _System_state_Set(SYSTEM_STATE_UP);

    /* context switch is not necessary */
    _Context_Switch_necessary = FALSE;

    /* executing thread is equal to the heir thread */
    _Thread_Executing = _Thread_Heir;

    /* get the init task(s) running.
     * Note: Thread_Dispatch() is normally used to dispatch threads.  As
     *       part of its work, Thread_Dispatch() restores floating point
     *       state for the heir task.
     *       This code avoids Thread_Dispatch(), and so we have to restore
     *       (actually initialize) the floating point state "by hand".
     *       Ignore the CPU_USE_DEFERRED_FP_SWITCH because we must always
     *       switch in the first thread if it is FP */

    /* if the CPU has hardware or software floating point */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
    /* then don't need to worry about saving BSP's floating point state */

    /* if heir thread has FP */
    if(_Thread_Heir->fp_context != NULL)
    {
        /* then place it */
        _Context_Restore_fp(&_Thread_Heir->fp_context);
    }

#endif

    /* perform a CPU context switch to the heir thread */
    _Context_Switch(&_Thread_BSP_context , &_Thread_Heir->Registers);
}

/**  
 *  @}
 */

/**
 *  @}
 */
