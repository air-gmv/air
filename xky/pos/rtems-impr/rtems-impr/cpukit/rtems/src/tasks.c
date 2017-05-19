/**
 *  @file
 *  tasks.c
 *
 *  @brief initialize the Task Manager
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
 *  3247    | 20/05/2009  | mcoutinho    | IPR 516
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6310    | 01/03/2010  | mcoutinho    | IPR 69
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  7117    | 09/04/2010  | mcoutinho    | IPR 1931
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
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


Objects_Information _RTEMS_tasks_Information;


/**
 * @brief invoke the user create task extension
 **/
boolean _RTEMS_tasks_Create_extension(
                                      Thread_Control *executing ,
                                      Thread_Control *created
                                      )
{
    /* the created task API */
    RTEMS_API_Control *api;

    /* iterator */
    int i;

    /* allocate space for the api */
    api = _Workspace_Allocate(sizeof ( RTEMS_API_Control ));

    /* if the api could not be created */
    if(!api)
    {
        /* return false */
        return FALSE;
    }

    /* set the allocated api */
    created->API_Extensions[ THREAD_API_RTEMS ] = api;

    /* set the pending events to none (initialization) */
    api->pending_events = EVENT_SETS_NONE_PENDING;

    /* set the task variables to none (initialization) */
    created->task_variables = NULL;

    /* initialize all the notepad entries */
    for(i = 0; i < RTEMS_NUMBER_NOTEPADS; i++)
    {
        /* with 0 */
        api->Notepads[i] = 0;
    }

    /* return true */
    return TRUE;
}


/**
 *  @brief invoke the user start task extension
 *  
 *  This extension routine is invoked when a task is started for the
 *  first time.
 */
User_extensions_routine _RTEMS_tasks_Start_extension(
                                                     Thread_Control *executing ,
                                                     Thread_Control *started
                                                     )
{
    /* started thread API */
    RTEMS_API_Control *api;

    /* get the started thread RTEMS API */
    api = started->API_Extensions[ THREAD_API_RTEMS ];

    /* initialize the pending events to none */
    api->pending_events = EVENT_SETS_NONE_PENDING;
}


/**
 *  @brief invoke the user delete task extension
 *  
 *  This extension routine is invoked when a task is deleted.
 */
User_extensions_routine _RTEMS_tasks_Delete_extension(
                                                      Thread_Control *executing ,
                                                      Thread_Control *deleted
                                                      )
{
    /* task variables */
    rtems_task_variable_t *tvp;

    /* iterator for the next task variable */
    rtems_task_variable_t *next;

    /* free per task variable memory */

    tvp = deleted->task_variables;

    /* setting pointer to null */
    deleted->task_variables = NULL;

    /* loop over all the task variables */
    while(tvp)
    {
        /* get the next variable */
        next = (rtems_task_variable_t *) tvp->next;

        /* check if the deleted thread is executing */
        if(_Thread_Is_executing(deleted))
        {
            /* call destructor if needed */
            if(tvp->dtor)
            {
                /* calling the destructor function */
                ( *tvp->dtor )( *tvp->ptr );
            }
            /* the variable is equal to the global value */
            *tvp->ptr = tvp->gval;
        }
            /* if it is not the executing thread then */
        else
        {
            /* call destructor if needed */
            if(tvp->dtor)
            {
                /* calling the destructor function */
                ( *tvp->dtor )( tvp->tval );
            }
        }

        /* free the variable memory */
        _Workspace_Free(tvp);

        /* advance to the next task variable */
        tvp = next;
    }

    /* free API specific memory */
    (void) _Workspace_Free(deleted->API_Extensions[ THREAD_API_RTEMS ]);

    /* set value to null */
    deleted->API_Extensions[ THREAD_API_RTEMS ] = NULL;
}


/**
 *  @brief invoke the user task context switch extension
 *  
 *  This extension routine is invoked at each context switch.
 */
void _RTEMS_tasks_Switch_extension(
                                   Thread_Control *executing ,
                                   Thread_Control *heir
                                   )
{
    /* Per Task Variables */
    rtems_task_variable_t *tvp;

    /* set the per task variables */
    tvp = executing->task_variables;

    /* update the executing thread variables values */
    while(tvp)
    {
        /* copy the variables */
        tvp->tval = *tvp->ptr;
        *tvp->ptr = tvp->gval;

        /* advance to the next task variable */
        tvp = (rtems_task_variable_t *) tvp->next;
    }

    tvp = heir->task_variables;

    /* update the heir thread variables values */
    while(tvp)
    {
        /* copy the variables */
        tvp->gval = *tvp->ptr;
        *tvp->ptr = tvp->tval;

        /* advance to the next task variable */
        tvp = (rtems_task_variable_t *) tvp->next;
    }
}


API_extensions_Control _RTEMS_tasks_API_extensions = {
    { NULL , NULL } ,
                                                      NULL , /* predriver */
                                                      _RTEMS_tasks_Initialize_user_tasks , /* postdriver */
                                                      NULL /* post switch -> not used (only for signals which were removed) */
};


User_extensions_Control _RTEMS_tasks_User_extensions = {
    { NULL , NULL } ,
    {
        { NULL , NULL } , _RTEMS_tasks_Switch_extension
    } ,
    { _RTEMS_tasks_Create_extension , /* create */
     _RTEMS_tasks_Start_extension , /* start */
     _RTEMS_tasks_Start_extension , /* restart */
     _RTEMS_tasks_Delete_extension , /* delete */
     _RTEMS_tasks_Switch_extension , /* switch */
     NULL , /* begin */
     NULL , /* exitted */
     NULL /* fatal */ }
};


void _RTEMS_tasks_Manager_initialization(
                                         uint32_t maximum_tasks
                                         )
{
    /* initialize the information for the task manager */
    _Objects_Initialize_information(&_RTEMS_tasks_Information , /* object information table */
                                    OBJECTS_CLASSIC_API , /* object API */
                                    OBJECTS_RTEMS_TASKS , /* object class */
                                    maximum_tasks , /* maximum objects of this class */
                                    sizeof ( Thread_Control ) , /* size of this object's control block */
                                    FALSE , /* TRUE if the name is a string */
                                    RTEMS_MAXIMUM_NAME_LENGTH /* maximum length of an object name */
#if defined(RTEMS_MULTIPROCESSING)
        ,
                                    TRUE , /* TRUE if this is a global object class */
                                    NULL /* Proxy extraction support callout */
#endif
        );

    /* add all the extensions for this API */
    _User_extensions_Add_API_set(&_RTEMS_tasks_User_extensions);

    /* Add all the extensions */
    _API_extensions_Add(&_RTEMS_tasks_API_extensions);

#if defined(RTEMS_MULTIPROCESSING)

    /* Register the MP Process Packet routine */
    _MPCI_Register_packet_processor(MP_PACKET_TASKS ,
                                    _RTEMS_tasks_MP_Process_packet);
#endif

}


void _RTEMS_tasks_Initialize_user_tasks(void)
{
    /* check if the function exists */
    if(_RTEMS_tasks_Initialize_user_tasks_p)
    {
        /* then call it to initialize the tasks */
        ( *_RTEMS_tasks_Initialize_user_tasks_p )( );
    }
}

/**  
 *  @}
 */

/**
 *  @}
 */
