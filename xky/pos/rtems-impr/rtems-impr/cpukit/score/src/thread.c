/**
 *  @file
 *  thread.c
 *
 *  @brief initialize the thread handler
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
 *  3247    | 20/05/2009  | mcoutinho    | IPR 516
 *  5247    | 30/10/2009  | mcoutinho    | IPR 828
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
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
#include <rtems/score/cpu.h>

/* @todo: remove references to ADA */
void *rtems_ada_self;


Objects_Information _Thread_Internal_information;


Thread_Control *_Thread_Idle;


Context_Control _Thread_BSP_context;


volatile uint32_t _Thread_Dispatch_disable_level;


uint32_t _Thread_Maximum_extensions;


uint32_t _Thread_Ticks_per_timeslice;


Chain_Control *_Thread_Ready_chain;


Thread_Control *_Thread_Executing = NULL;


Thread_Control *_Thread_Heir = NULL;

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
    #if ( CPU_USE_DEFERRED_FP_SWITCH == TRUE )
Thread_Control *_Thread_Allocated_fp = NULL;
    #endif
#endif


void _Thread_Handler_initialization(
                                    uint32_t ticks_per_timeslice ,
                                    uint32_t maximum_extensions
#if defined(RTEMS_MULTIPROCESSING)
    ,
                                    uint32_t maximum_proxies
#endif
    )
{
    /* iterator to search the priority */
    uint32_t index;

#if defined(RTEMS_MULTIPROCESSING)

    /* maximum */
    uint32_t maximum;
#endif


    /* BOTH stacks hooks must be set or both must be NULL.
     * do not allow mixture */
    if(!( ( _CPU_Table.stack_allocate_hook == 0 )
          == ( _CPU_Table.stack_free_hook == 0 ) ))
    {
        /* raise internal error */
        _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                 TRUE ,
                                 INTERNAL_ERROR_BAD_STACK_HOOK);
    }

    /* initialize stating that a context switch is not needed */
    _Context_Switch_necessary = FALSE;

    /* there is no thread executing at the moment (multitasking has not
     * been initiated) */
    _Thread_Executing = NULL;

    /* there is no heir thread */
    _Thread_Heir = NULL;

    /* if CPU has hardware or software FP */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )

    /* if use deferred FP switch */
    #if ( CPU_USE_DEFERRED_FP_SWITCH == TRUE )

    /* the thread fp does not exist */
    _Thread_Allocated_fp = NULL;

    #endif

#endif

    /* set the maximum number of extensions */
    _Thread_Maximum_extensions = maximum_extensions;

    /* set the thread ticks per timeslice information */
    _Thread_Ticks_per_timeslice = ticks_per_timeslice;

    /* allocate space for the thread ready chain */
    _Thread_Ready_chain = (Chain_Control *) _Workspace_Allocate_or_fatal_error(( PRIORITY_MAXIMUM + 1 ) *
                                                                               sizeof (Chain_Control ));

    /* iterate through every priority */
    for(index = 0; index <= PRIORITY_MAXIMUM; index++)
    {
        /* initialize the ready chain */
        _Chain_Initialize_empty(&_Thread_Ready_chain[ index ]);
    }

    /* if system has multiprocessing */
#if defined(RTEMS_MULTIPROCESSING)

    /* initialize the thread MP handler */
    _Thread_MP_Handler_initialization(maximum_proxies);

    /* initialize the number of maximum objects that can be active */
    if(_System_state_Is_multiprocessing)
    {
        /* set the maximum to 2 */
        maximum = 2;
    }
    else
    {
        /* set the maximum to 1 */
        maximum = 1;
    }

#endif

    /* Initialize this class of objects */
    _Objects_Initialize_information(&_Thread_Internal_information ,
                                    OBJECTS_INTERNAL_API ,
                                    OBJECTS_INTERNAL_THREADS ,
                                    /* if system has multiprocessing */
#if defined(RTEMS_MULTIPROCESSING)
        maximum ,
#else
        1 ,
#endif
        sizeof ( Thread_Control ) , /* size of this object's control block */
                                    TRUE , /* TRUE if names for this object are strings */
                                    8 /* maximum length of each object's name */
                                    /* if system has multiprocessing */
#if defined(RTEMS_MULTIPROCESSING)
        ,
                                    FALSE , /* TRUE if this is a global object class */
                                    NULL /* Proxy extraction support callout */
#endif
        );

}

/**  
 *  @}
 */

/**
 *  @}
 */
