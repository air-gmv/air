/**
 *  @file
 *  taskinitusers.c
 *
 *  @brief initialize initial user tasks
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  4400    | 21/09/2009  | mcoutinho    | IPR 563
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  7112    | 09/04/2010  | mcoutinho    | IPR 1931
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
#include <rtems/config.h>
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


void _RTEMS_tasks_Initialize_user_tasks_body(void)
{
    /* iterator */
    uint32_t index;

    /* number of threads */
    uint32_t maximum;

    /* id of each task */
    rtems_id id;

    /* return value of the calls to RTEMS API */
    rtems_status_code res;

    /* pointer to the each task */
    rtems_initialization_tasks_table *user_tasks;

    /* API configuration table */
    rtems_api_configuration_table *api_configuration;


    /* get the api configuration table */
    api_configuration = _Configuration_Table->RTEMS_api_configuration;

    /* NOTE:  This is slightly different from the Ada implementation */

    /* get the user tasks table */
    user_tasks = api_configuration->User_initialization_tasks_table;

    /* get the number of initialization tasks */
    maximum = api_configuration->number_of_initialization_tasks;

    /* iterate through every task on the initialization tasks table */
    for(index = 0; index < maximum; index++)
    {
        /* create a task with:
         *   configuration table name
         *   configuration table initial priority
         *   configuration table stack size
         *   configuration table mode set
         *   configuration table attribute set */
        res = rtems_task_create(user_tasks[ index ].name ,
                                user_tasks[ index ].initial_priority ,
                                user_tasks[ index ].stack_size ,
                                user_tasks[ index ].mode_set ,
                                user_tasks[ index ].attribute_set ,
                                &id);

        /* if not able to create the task */
        if(!rtems_is_status_successful(res))
        {
            /* raise an internal error */
            _Internal_error_Occurred(INTERNAL_ERROR_RTEMS_API , TRUE , res);
        }

        /* start the threads with:
         *   configuration table entry point
         *   configuration table argument */
        res = rtems_task_start(id ,
                               user_tasks[ index ].entry_point ,
                               user_tasks[ index ].argument);

        /* if not able to start the task */
        if(!rtems_is_status_successful(res))
        {
            /* raise an internal error */
            _Internal_error_Occurred(INTERNAL_ERROR_RTEMS_API , TRUE , res);
        }
    }
}

/**  
 *  @}
 */

/**
 *  @}
 */
