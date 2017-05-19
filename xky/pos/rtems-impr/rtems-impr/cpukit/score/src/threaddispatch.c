/**
 *  @file
 *  threaddispatch.c
 *
 *  @brief performs thread dispatch if needed
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  621     | 17/11/2008  | mcoutinho    | IPR 69
 *  621     | 17/11/2008  | mcoutinho    | IPR 70
 *  3247    | 20/05/2009  | mcoutinho    | IPR 516
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

/* plugin clocktick callback								*/
extern void pmk_plugin_kernel_thread_tick(void);
/* Reset plugin clocktick callback delayed flag 			*/
extern void pmk_reset_dpp_flag(void);

#if ( CPU_INLINE_ENABLE_DISPATCH == FALSE )


void _Thread_Enable_dispatch(void)
{
    /* decrease the thread dispatch disable level */
    /* if it is not zero */
    if(--_Thread_Dispatch_disable_level)
    {
        /* then just return */
        return;
    }

    /* else, call thread dispatch */
    _Thread_Dispatch();
}
#endif


void _Thread_Dispatch(void)
{
    /* RVS_Ipoint _Thread_Dispatch_Start */
#ifdef RVS
    RVS_Ipoint(0xF50A);
#endif

    /* thread executing */
    Thread_Control *executing;

    /* heir thread */
    Thread_Control *heir;

    /* interrupt level */
    ISR_Level level;

    /* get the executing thread */
    executing = _Thread_Executing;

    /* enter critical section */
    _ISR_Disable(level);

    /* while context switch is necessary */
    while(_Context_Switch_necessary == TRUE)
    {
        /* get heir thread */
        heir = _Thread_Heir;

        /* disable thread dispatch */
        _Thread_Dispatch_disable_level = 1;

        /* context switch is not necessary now */
        _Context_Switch_necessary = FALSE;

        /* set the new thread executing to be the heir thread */
        _Thread_Executing = heir;

        /* ada stuff */
        executing->rtems_ada_self = rtems_ada_self;
        rtems_ada_self = heir->rtems_ada_self;

        /* reset the heir thread cpu time budget (if any) */
        if(heir->budget_algorithm == THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE)
        {
            /* resest the heir thread budget */
            heir->cpu_time_budget = _Thread_Ticks_per_timeslice;
        }

        /* leave critical section */
        _ISR_Enable(level);

        /* call user thread switch extensions */
        _User_extensions_Thread_switch(executing , heir);

        /* 
         * Instrument RVS: We use the object API class (bit 24 - 26) to distinguish
         * between IDLE task and the init task 
         */
#ifdef RVS        
        RVS_Ipoint((0xF800) | (((((unsigned int)(heir->Object.id)) >> 24) & 0x1) << 9)
                            | (((unsigned int)(heir->Object.id)) & (0x1FF)));
#endif

        /* if the CPU has hardware floating point, then we must address saving
         * and restoring it as part of the context switch.
         * The second conditional compilation section selects the algorithm used
         * to context switch between floating point tasks.  The deferred algorithm
         * can be significantly better in a system with few floating point tasks
         * because it reduces the total number of save and restore FP context
         * operations.  However, this algorithm can not be used on all CPUs due
         * to unpredictable use of FP registers by some compilers for integer
         * operations */

        /* if the CPU has hardware or software FP */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )

        /* if not use deferred FP */
    #if ( CPU_USE_DEFERRED_FP_SWITCH != TRUE )

        /* if the executing thread has FP */
        if(executing->fp_context != NULL)
        {
            /* then save it */
            _Context_Save_fp(&executing->fp_context);
        }

    #endif

#endif

        /* perform context switch here! */
        _Context_Switch(&executing->Registers , &heir->Registers);

        /* if the CPU has hardware or software FP */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )

        /* if use deferred FP */
    #if ( CPU_USE_DEFERRED_FP_SWITCH == TRUE )

        /* if the executing thread has FP */
        if(( executing->fp_context != NULL ) &&
           !_Thread_Is_allocated_fp(executing))
        {
            /* if there is a thread FP */
            if(_Thread_Allocated_fp != NULL)
            {
                /* save the FP context */
                _Context_Save_fp(&_Thread_Allocated_fp->fp_context);
            }

            /* restore the FP context of the running thread */
            _Context_Restore_fp(&executing->fp_context);

            /* set the thread FP to the running thread */
            _Thread_Allocated_fp = executing;
        }

    #else

        /* if the executing thread has FP */
        if(executing->fp_context != NULL)
        {
            /* restore the FP context of the executing thread */
            _Context_Restore_fp(&executing->fp_context);
        }

    #endif

#endif

        /* get the executing thread */
        executing = _Thread_Executing;

        /* enter critical section again */
        _ISR_Disable(level);
    }

    /* reset the thread dispatch disable level */
    _Thread_Dispatch_disable_level = 0;

    /* leave critical section */
    _ISR_Enable(level);

    /* RVS_Ipoint _Thread_Dispatch_End */
#ifdef RVS        
    RVS_Ipoint(0xF50B);
#endif
}

#ifdef RVS
void _ISR_Thread_Dispatch(void)
{
    /* RVS_Ipoint _ISR_Thread_Dispatch_Start */
    RVS_Ipoint(0xF510);

    /* thread executing */
    Thread_Control *executing;

    /* heir thread */
    Thread_Control *heir;

    /* interrupt level */
    ISR_Level level;

    /* get the executing thread */
    executing = _Thread_Executing;

    /* enter critical section */
    _ISR_Disable(level);

    /* while context switch is necessary */
    while(_Context_Switch_necessary == TRUE)
    {
        /* get heir thread */
        heir = _Thread_Heir;

        /* disable thread dispatch */
        _Thread_Dispatch_disable_level = 1;

        /* context switch is not necessary now */
        _Context_Switch_necessary = FALSE;

        /* set the new thread executing to be the heir thread */
        _Thread_Executing = heir;

        /* ada stuff */
        executing->rtems_ada_self = rtems_ada_self;
        rtems_ada_self = heir->rtems_ada_self;

        /* reset the heir thread cpu time budget (if any) */
        if(heir->budget_algorithm == THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE)
        {
            /* resest the heir thread budget */
            heir->cpu_time_budget = _Thread_Ticks_per_timeslice;
        }

        /* leave critical section */
        _ISR_Enable(level);

        /* call user thread switch extensions */
        _User_extensions_Thread_switch(executing , heir);

        /* Instrument RVS */
        RVS_Ipoint((0xF800) | (((((unsigned int)(heir->Object.id)) >> 24) & 0x1) << 9)
                            | (((unsigned int)(heir->Object.id)) & (0x1FF)));

        /* if the CPU has hardware floating point, then we must address saving
         * and restoring it as part of the context switch.
         * The second conditional compilation section selects the algorithm used
         * to context switch between floating point tasks.  The deferred algorithm
         * can be significantly better in a system with few floating point tasks
         * because it reduces the total number of save and restore FP context
         * operations.  However, this algorithm can not be used on all CPUs due
         * to unpredictable use of FP registers by some compilers for integer
         * operations */

        /* if the CPU has hardware or software FP */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )

        /* if not use deferred FP */
    #if ( CPU_USE_DEFERRED_FP_SWITCH != TRUE )

        /* if the executing thread has FP */
        if(executing->fp_context != NULL)
        {
            /* then save it */
            _Context_Save_fp(&executing->fp_context);
        }

    #endif

#endif

        /* perform context switch here! */
        _Context_Switch(&executing->Registers , &heir->Registers);

        /* if the CPU has hardware or software FP */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )

        /* if use deferred FP */
    #if ( CPU_USE_DEFERRED_FP_SWITCH == TRUE )

        /* if the executing thread has FP */
        if(( executing->fp_context != NULL ) &&
           !_Thread_Is_allocated_fp(executing))
        {
            /* if there is a thread FP */
            if(_Thread_Allocated_fp != NULL)
            {
                /* save the FP context */
                _Context_Save_fp(&_Thread_Allocated_fp->fp_context);
            }

            /* restore the FP context of the running thread */
            _Context_Restore_fp(&executing->fp_context);

            /* set the thread FP to the running thread */
            _Thread_Allocated_fp = executing;
        }

    #else

        /* if the executing thread has FP */
        if(executing->fp_context != NULL)
        {
            /* restore the FP context of the executing thread */
            _Context_Restore_fp(&executing->fp_context);
        }

    #endif

#endif

        /* get the executing thread */
        executing = _Thread_Executing;

        /* enter critical section again */
        _ISR_Disable(level);
    }

    /* reset the thread dispatch disable level */
    _Thread_Dispatch_disable_level = 0;

    /* leave critical section */
    _ISR_Enable(level);

    /* RVS_Ipoint _ISR_Thread_Dispatch_End */
    RVS_Ipoint(0xF511);
}
#endif /* RVS */

/**  
 *  @}
 */

/**
 *  @}
 */
