/**
 *  @file
 *  threadloadenv.c
 *
 *  @brief load the environment to a thread (initializes the thread)
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
 *  3247    | 20/05/2009  | mcoutinho    | IPR 516
 *  3910    | 21/07/2009  | mcoutinho    | IPR 99
 *  3910    | 21/07/2009  | mcoutinho    | IPR 553
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


void _Thread_Load_environment(
                              Thread_Control *the_thread
                              )
{
    /* flag indicating if thread has FP */
    boolean is_fp;

    
    /* if the CPU has hardware or software FP */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )

    /* if the thread has FP */
    if(the_thread->Start.fp_context)
    {
        /* then initialize the thread FP with the start information */
        the_thread->fp_context = the_thread->Start.fp_context;

        /* initialize the FP context*/
        _Context_Initialize_fp(&the_thread->fp_context);

        /* the thread has now FP */
        is_fp = TRUE;
    }
    else
    {
#endif
        /* thread does not have FP */
        is_fp = FALSE;

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
    }
#endif

    /* load the initial block information */

    /* the thread does not hold any mutexes */
    _Chain_Initialize_empty(&the_thread->Wait.mutexes_hold);

    /* no threads are blocking on this thread */
    _Chain_Initialize_empty(&the_thread->Wait.threads_blocked);

    /* dont have any blocked mutexes */
    the_thread->Wait.mutex_blocked = NULL;

    /* load the start information */

    /* load the thread preempt based on the start information */
    the_thread->is_preemptible = the_thread->Start.is_preemptible;

    /* load the thread budget algorithm based on the start information */
    the_thread->budget_algorithm = the_thread->Start.budget_algorithm;

    /* load the thread budget function based on the start information */
    the_thread->budget_callout = the_thread->Start.budget_callout;

    /* initialize the thread CPU context with:
     *   the thread CPU registers
     *   the thread initial stack area
     *   the thread initial stack size
     *   the thread isr level
     *   the thread entry point (is the common entry point to all threads)
     *   the flag indicating if the thread has FP */
    _Context_Initialize(&the_thread->Registers ,
                        the_thread->Start.Initial_stack.area ,
                        the_thread->Start.Initial_stack.size ,
                        the_thread->Start.isr_level ,
                        _Thread_Handler ,
                        is_fp);

}

/**  
 *  @}
 */

/**
 *  @}
 */
