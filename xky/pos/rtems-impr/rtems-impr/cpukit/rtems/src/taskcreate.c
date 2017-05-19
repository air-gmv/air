/**
 *  @file
 *  taskcreate.c
 *
 *  @brief create a task
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  583     | 17/11/2008  | mcoutinho    | IPR 69
 *  4428    | 21/09/2009  | mcoutinho    | IPR 685
 *  5247    | 30/10/2009  | mcoutinho    | IPR 828
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  7108    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_TASK Task Manager
 *  @{
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/modes.h>
#include <rtems/score/object.h>
#include <rtems/score/stack.h>
#include <rtems/score/states.h>
#include <rtems/rtems/tasks.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tod.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>
#include <rtems/score/sysstate.h>


rtems_status_code rtems_task_create(
                                    rtems_name name ,
                                    rtems_task_priority initial_priority ,
                                    size_t stack_size ,
                                    rtems_mode initial_modes ,
                                    rtems_attribute attribute_set ,
                                    Objects_Id *id
                                    )
{
#if defined(RTEMS_MULTIPROCESSING)
    /* global task object information */
    Objects_MP_Control *the_global_object = NULL;

    /* flag to indicate if the task is global/local */
    boolean is_global;
#endif

    /* thread to create */
    register Thread_Control *the_thread;

    /* floating point flag */
    boolean is_fp;

    /* task is preemptive flag */
    boolean is_preempt;

    /* flag indicating if the thread was correctly initialized */
    boolean status;

    /* copy of the attribute set with the added CPU required attributes
     * and minus the attributes not supported by the CPU */
    rtems_attribute the_attribute_set;

    /* priority translated into the Super Core language */
    Priority_Control core_priority;

    /* created thread API */
    RTEMS_API_Control *api;

    /* created thread budget algorithm */
    Thread_CPU_budget_algorithms budget;


    /* check if id address is valid */
    if(!id)
    {
        /* return invalid address error */
        return RTEMS_INVALID_ADDRESS;
    }

    /* check if name is valid */
    if(!rtems_is_name_valid(name))
    {
        /* return invalid name error */
        return RTEMS_INVALID_NAME;
    }

    /* Core Thread Initialize insures we get the minimum amount of
     * stack space */

    /* Fix the attribute set to match the attributes which
     * this processor (1) requires and (2) is able to support.
     * First add in the required flags for attribute_set
     * Typically this might include FP if the platform
     * or application required all tasks to be fp aware.
     * Then turn off the requested bits which are not supported */

    /* set the attribute set to another variable with the attributes required for
     * this architecture */
    the_attribute_set = _Attributes_Set(attribute_set , ATTRIBUTES_REQUIRED);

    /* remove the attributes not supported for this architecture */
    the_attribute_set = _Attributes_Clear(the_attribute_set ,
                                          ATTRIBUTES_NOT_SUPPORTED);

    /* check the floating point attributes */
    if(_Attributes_Is_floating_point(the_attribute_set))
    {
        /* is floating point */
        is_fp = TRUE;
    }
    else
    {
        /* is not floating point */
        is_fp = FALSE;
    }

    /* validate the RTEMS API priority and convert it to the core priority range */

    /* is it a system task ?*/
    if(!_Attributes_Is_system_task(the_attribute_set))
    {
        /* check the priority range */
        if(!_RTEMS_tasks_Priority_is_valid(initial_priority))
        {
            /* if not valid, return invalid priority error */
            return RTEMS_INVALID_PRIORITY;
        }
    }
    /* else, does not check the priority for system tasks */

    /* convert the priority to a core priority */
    core_priority = _RTEMS_tasks_Priority_to_Core(initial_priority);

#if defined(RTEMS_MULTIPROCESSING)

    /* make sure system is MP if this task is global */
    if(_Attributes_Is_global(the_attribute_set))
    {
        /* task is global */
        is_global = TRUE;

        /* check if the system has multiprocessing */
        if(!_System_state_Is_multiprocessing)
        {
            /* it does not, return error */
            return RTEMS_MP_NOT_CONFIGURED;
        }
    }
    else
    {
        /* task is not global */
        is_global = FALSE;
    }
#endif

    /* Make sure system is MP if this task is global */

    /* Disable dispatch for protection */
    _Thread_Disable_dispatch();

    /* Allocate the thread control block and -- if the task is global --
     * allocate a global object control block.
     *
     * NOTE:  This routine does not use the combined allocate and open
     *        global object routine because this results in a lack of
     *        control over when memory is allocated and can be freed in
     *        the event of an error */
    the_thread = _RTEMS_tasks_Allocate();

    /* check if thread was allocated */
    if(!the_thread)
    {
        /* if it was not, then undo dispatch */
        _Thread_Enable_dispatch();

        /* and return error */
        return RTEMS_TOO_MANY;
    }

#if defined(RTEMS_MULTIPROCESSING)

    /* check if the task is global */
    if(is_global)
    {
        /* allocate the remote task */
        the_global_object = _Objects_MP_Allocate_global_object();

        /* check if the remote task was correctly allocated */
        if(_Objects_MP_Is_null_global_object(the_global_object))
        {
            /* the task was not remotely allocated , free it */
            _RTEMS_tasks_Free(the_thread);

            /* and re-enable thread dispatch */
            _Thread_Enable_dispatch();

            /* and return that too many tasks were allocated */
            return RTEMS_TOO_MANY;
        }
    }
#endif

    /* check timeslice mode */
    if(_Modes_Is_timeslice(initial_modes))
    {
        /* bugdet is timeslice */
        budget = THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE;
    }
    else
    {
        /* bugdet is none */
        budget = THREAD_CPU_BUDGET_ALGORITHM_NONE;
    }

    /* check preempt mode */
    if(_Modes_Is_preempt(initial_modes))
    {
        /* preempt */
        is_preempt = TRUE;
    }
    else
    {
        /* no-preempt */
        is_preempt = FALSE;
    }

    /* Initialize the core thread for this task with:
     *   Tasks object information
     *   the thread to initialize
     *   stack size (will be checked in this function if it is valid)
     *   is floating point flag
     *   priority of the thread
     *   preempt or no-preempt flag
     *   budget algorithm (timeslice or not)
     *   no budget algorithm callout
     *   interrupt mode
     *   thread name */
    status = _Thread_Initialize(&_RTEMS_tasks_Information ,
                                the_thread ,
                                stack_size ,
                                is_fp ,
                                core_priority ,
                                is_preempt ,
                                budget ,
                                NULL , /* no budget algorithm callout */
                                _Modes_Get_interrupt_level(initial_modes) ,
                                (Objects_Name) name);

    /* if the thread was incorrectly initialized */
    if(!status)
    {
        /* then an error occurred, undo previous steps */

#if defined(RTEMS_MULTIPROCESSING)

        /* if the thread is global */
        if(is_global)
        {
            /* free the global thread object */
            _Objects_MP_Free_global_object(the_global_object);
        }

#endif

        /* free the thread */
        _RTEMS_tasks_Free(the_thread);

        /* and enable dispatch */
        _Thread_Enable_dispatch();

        /* return unsatisfied */
        return RTEMS_UNSATISFIED;
    }

    /* get the thread RTEMS api */
    api = the_thread->API_Extensions[ THREAD_API_RTEMS ];

    /* set the out identifier */
    *id = the_thread->Object.id;

#if defined(RTEMS_MULTIPROCESSING)

    /* initialize the thread global field */
    the_thread->is_global = is_global;

    /* if the thread is global */
    if(is_global)
    {
        /* then open the multiprocessing object */
        _Objects_MP_Open(&_RTEMS_tasks_Information ,
                         the_global_object ,
                         name ,
                         the_thread->Object.id);

        /* and send the packet to the remote node */
        _RTEMS_tasks_MP_Send_process_packet(RTEMS_TASKS_MP_ANNOUNCE_CREATE ,
                                            the_thread->Object.id ,
                                            name);
    }

#endif

    /* re-enable dispatch */
    _Thread_Enable_dispatch();

    /* return success */
    return RTEMS_SUCCESSFUL;
}

/**  
 *  @}
 */

/**
 *  @}
 */
