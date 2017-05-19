/**
 *  @file
 *  threadinitialize.c
 *
 *  @brief initialize a thread structure
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
 *  234     | 09/10/2008  | mcoutinho    | IPR 67
 *  622     | 17/11/2008  | mcoutinho    | IPR 70
 *  3704    | 09/07/2009  | mcoutinho    | IPR 548
 *  3909    | 21/07/2009  | mcoutinho    | IPR 99
 *  3909    | 21/07/2009  | mcoutinho    | IPR 553
 *  4430    | 21/09/2009  | mcoutinho    | IPR 685
 *  4441    | 21/09/2009  | mcoutinho    | IPR 574
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
#include <rtems/score/watchdog.h>
#include <rtems/score/wkspace.h>


boolean _Thread_Initialize(
                           Objects_Information *information ,
                           Thread_Control *the_thread ,
                           size_t stack_size ,
                           boolean is_fp ,
                           Priority_Control priority ,
                           boolean is_preemptible ,
                           Thread_CPU_budget_algorithms budget_algorithm ,
                           Thread_CPU_budget_algorithm_callout budget_callout ,
                           uint32_t isr_level ,
                           Objects_Name name
                           )
{
    /* thread stack size */
    size_t actual_stack_size = 0;

    /* pointer to the stack area */
    void *stack = NULL;

    /* if the CPU has hardware or software FP */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )

    /* pointer to the FP area */
    void *fp_area;

#endif

    /* pointer to the extensions area */
    void *extensions_area;


    /* initialize the Ada self pointer */
    the_thread->rtems_ada_self = NULL;

    /* allocate and Initialize the stack for this thread */
    actual_stack_size = _Thread_Stack_Allocate(the_thread , stack_size);

    /* if could not allocate sufficient stack */
    if(!actual_stack_size || actual_stack_size < stack_size)
    {
        /* return false (could not initialize thread) */
        return FALSE;
    }

    /* get the thread stack area */
    stack = the_thread->Start.stack;

    /* set the thread allocated stack flag to true */
    the_thread->Start.core_allocated_stack = TRUE;

    /* initialize the stack information */
    _Stack_Initialize(&the_thread->Start.Initial_stack ,
                      stack ,
                      actual_stack_size);

    /* if the CPU has hardware or software FP */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )

    /* if the thread has FP */
    if(is_fp)
    {
        /* allocate the floating point area for this thread */
        fp_area = _Workspace_Allocate(CONTEXT_FP_SIZE);

        /* if could not allocate */
        if(!fp_area)
        {
            /* undo previous steps */

            /* free the stack */
            _Thread_Stack_Free(the_thread);

            /* and return false (could not initialize thread) */
            return FALSE;
        }

        /* get the address of the FP context start */
        fp_area = _Context_Fp_start(fp_area , 0);

    }
    else
    {
        /* thread does not have FP */

        /* set the FP area to NULL */
        fp_area = NULL;
    }

    /* set the thread FP context */
    the_thread->fp_context = fp_area;

    /* set the thread start FP context */
    the_thread->Start.fp_context = fp_area;

#endif

    /* initialize the thread timer */
    _Watchdog_Initialize(&the_thread->Timer , NULL , 0 , NULL);


    /* allocate the extensions area for this thread */

    /* if there are thread extensions */
    if(_Thread_Maximum_extensions)
    {
        /* allocate the extension area */
        extensions_area = _Workspace_Allocate(( _Thread_Maximum_extensions + 1 ) *
            sizeof ( void *));

        /* if could not allocate */
        if(!extensions_area)
        {
            /* undo previous steps */

            /* if the CPU has hardware or software FP */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
            /* if the thread has FP */
            if(fp_area)
            {
                /* free the FP area */
                (void) _Workspace_Free(fp_area);
            }
#endif
            /* free the allocated stack */
            _Thread_Stack_Free(the_thread);

            /* and return false (could not initialize thread) */
            return FALSE;
        }
    }
    else
    {
        /* there is no extension area */
        extensions_area = NULL;
    }

    /* set the thread extensions area */
    the_thread->extensions = (void **) extensions_area;

    /* clear the extensions area so extension users can determine
     * if they are linked to the thread. An extension user may
     * create the extension long after tasks have been created
     * so they cannot rely on the thread create user extension
     * call */

    /* if there are extensions */
    if(the_thread->extensions)
    {
        /* iterator through the extensions */
        int i;

        /* run through every extension */
        for(i = 0; i < ( _Thread_Maximum_extensions + 1 ); i++)
        {
            /* and initialize them to NULL */
            the_thread->extensions[i] = NULL;
        }
    }

    /* general initialization */

    /* initialize the thread preemption attribute */
    the_thread->Start.is_preemptible = is_preemptible;

    /* initialize the thread budget algorithm attribute */
    the_thread->Start.budget_algorithm = budget_algorithm;

    /* initialize the thread budget callout function */
    the_thread->Start.budget_callout = budget_callout;

    /* initialize the thread interrupt level attribute */
    the_thread->Start.isr_level = isr_level;

    /* set the thread initial state */
    the_thread->current_state = STATES_DORMANT;

    /* initialize the thread wait thread queue to none */
    the_thread->Wait.queue = NULL;

    /* initialize the thread wait thread itself */
    the_thread->Wait.thread = the_thread;

    /* initialize the thread wait node next to none */
    the_thread->Wait.node.next = NULL;

    /* initialize the thread wait node previous to none */
    the_thread->Wait.node.previous = NULL;

    /* the thread does not hold any mutexes */
    _Chain_Initialize_empty(&the_thread->Wait.mutexes_hold);

    /* no threads are blocking on this thread */
    _Chain_Initialize_empty(&the_thread->Wait.threads_blocked);

    /* dont have any blocked mutexes */
    the_thread->Wait.mutex_blocked = NULL;

    /* initialize the thread resource count to none */
    the_thread->resource_count = 0;

    /* initialize the thread suspend count to none */
    the_thread->suspend_count = 0;

    /* initialize the thread real priority the initial priority */
    the_thread->real_priority = priority;

    /* initialize the thread current priority the initial priority */
    the_thread->Start.initial_priority = priority;

    /* set the thread priority in the system */
    _Thread_Set_priority(the_thread , priority);

    /* open the object */
    _Objects_Open(information , &the_thread->Object , name);

    /* invoke create extensions */

    /* if there is a thread create user extension which tells not to create the
     * thread */
    if(!_User_extensions_Thread_create(the_thread))
    {
        /* then undo previous steps */

        /* if there was an extension area */
        if(extensions_area)
        {
            /* then free the extension area */
            (void) _Workspace_Free(extensions_area);
        }

        /* if the CPU has hardware or software FP */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )

        /* if the thread was FP */
        if(fp_area)
        {
            /* then free the FP area */
            (void) _Workspace_Free(fp_area);
        }
#endif

        /* free the stack */
        _Thread_Stack_Free(the_thread);

        /* and return false (could not initialize thread) */
        return FALSE;
    }

    /* initialized the thread */
    return TRUE;
}

/**  
 *  @}
 */

/**
 *  @}
 */
