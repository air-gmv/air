/**
 *  @file
 *  threadhandler.c
 *
 *  @brief initialize the Thread Handler
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
 *  3551    | 29/06/2009  | mcoutinho    | IPR 537
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

#if defined(__USE__MAIN__)
extern void _main(void);
#endif

#if defined(__USE_INIT_FINI__)
extern void _init(void);
#endif

volatile boolean _Context_Switch_necessary;


void _Thread_Handler(void)
{
    /* interrupt level */
    ISR_Level level;

    /* thread executing */
    Thread_Control *executing;

#if defined(__USE_INIT_FINI__) || defined(__USE__MAIN__)

    static char doneConstructors;
    char doneCons;

#endif

    
    /* get executing thread */
    executing = _Thread_Executing;

    /* some CPUs need to tinker with the call frame or registers when the
     * thread actually begins to execute for the first time.  This is a
     * hook point where the port gets a shot at doing whatever it requires */
    _Context_Initialization_at_thread_begin();

    /* have to put level into a register for those cpu's that use
     * inline asm here */
    level = executing->Start.isr_level;

    /* set the thread ISR level */
    _ISR_Set_level(level);

#if defined(__USE_INIT_FINI__) || defined(__USE__MAIN__)

    doneCons = doneConstructors;
    doneConstructors = 1;

#endif

    /* if the CPU has hardware or software FP */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )

    /* if use deferred FP */
    #if ( CPU_USE_DEFERRED_FP_SWITCH == TRUE )

    /* if executing thread has FP */
    if(( executing->fp_context != NULL ) && !_Thread_Is_allocated_fp(executing))
    {
        /* if there is a thread with FP */
        if(_Thread_Allocated_fp != NULL)
        {
            /* save the FP */
            _Context_Save_fp(&_Thread_Allocated_fp->fp_context);
        }

        /* set the allocated FP thread */
        _Thread_Allocated_fp = executing;
    }

    #endif

#endif

    /* take care that 'begin' extensions get to complete before
     * 'switch' extensions can run.  This means must keep dispatch
     * disabled until all 'begin' extensions complete */
    _User_extensions_Thread_begin(executing);

    /* at this point, the dispatch disable level BETTER be 1 */

    _Thread_Enable_dispatch();

	/* PMK GMV - SKY TODO : this is not supported as of right now */
#if defined(__USE_INIT_FINI__TODO)
    /* _init could be a weak symbol and we SHOULD test it but it isn't
     * in any configuration I know of and it generates a warning on every
     * RTEMS target configuration.  --joel (12 May 2007) */

    if ((!doneCons) && (volatile void *)_init)
    {
        _init();
    }

#endif

    /* 
     * Instrument RVS: We use the object API class (bit 24 - 26) to distinguish
     * between IDLE task and the init task 
     */
#ifdef RVS    
    RVS_Ipoint((0xF000) | (((((unsigned int)(executing->Object.id)) >> 24) & 0x1) << 9)
                        | (((unsigned int)(executing->Object.id)) & (0x1FF)));
#endif

#if defined(__USE__MAIN__TODO)

    if(!doneCons && _main)
    {
        __main();
    }

#endif

    /* call the user thread entry point */
    executing->Wait.return_argument =
        ( *(Thread_Entry_numeric) executing->Start.entry_point )( executing->Start.numeric_argument );

    /* the return code from the user thread body
     * was placed in return_argument.  This assumed that if it returned
     * anything (which is not supporting in all APIs), then it would be
     * able to fit in a (void *) */
    _User_extensions_Thread_exitted(executing);

    /* raise internal error. should not reach this point!
     * if we reached here, the application did something bad and we cannot let
     * it continue */
    _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                             TRUE ,
                             INTERNAL_ERROR_THREAD_EXITTED);
}

/**
 *  @}
 */

/**
 *  @}
 */
