/**
 *  @file
 *  taskvariableadd.c
 *
 *  @brief add a per task variable
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
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  7122    | 09/04/2010  | mcoutinho    | IPR 1931
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
#include <rtems/rtems/tasks.h>
#include <rtems/score/wkspace.h>


rtems_status_code rtems_task_variable_add(
                                          rtems_id tid ,
                                          void **ptr ,
                                          void (*dtor )(void *)
                                          )
{
    /* task to add the variable to */
    Thread_Control *the_thread;

    /* task to add the variable to location */
    Objects_Locations location;

    /* task variable list */
    rtems_task_variable_t *tvp , *new;

    
    /* check if the variable is not NULL */
    if(!ptr)
    {
        /* return invalid address error */
        return RTEMS_INVALID_ADDRESS;
    }

    /* get the thread */
    the_thread = _Thread_Get(tid , &location);

    /* check the task location */
    switch(location)
    {
#if defined(RTEMS_MULTIPROCESSING)

            /* task is remote */
        case OBJECTS_REMOTE:

            /* dispatch threads */
            _Thread_Dispatch();

            /* return illegal operation on remote task */
            return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

            /* task id is invalid */
        case OBJECTS_ERROR:

            /* return invalid identifier error */
            return RTEMS_INVALID_ID;

            /* task is local (nominal case) */
        case OBJECTS_LOCAL:

            /* determine if the variable is already in this task's list */

            /* get the thread variable list */
            tvp = the_thread->task_variables;

            /* iterate through the variable list */
            while(tvp)
            {
                /* if the pointer is equal */
                if(tvp->ptr == ptr)
                {
                    /* update the destructor */
                    tvp->dtor = dtor;

                    /* enable dispatch (disabled by _Thread_Get) */
                    _Thread_Enable_dispatch();

                    /* and return success */
                    return RTEMS_SUCCESSFUL;
                }

                /* search the next variable */
                tvp = (rtems_task_variable_t *) tvp->next;
            }

            /* allocate memory for this task variable */
            new = (rtems_task_variable_t *) _Workspace_Allocate(sizeof (rtems_task_variable_t ));

            /* if could not allocate memory for the variable */
            if(new == NULL)
            {
                /* enable dispatch (disabled by _Thread_Get) */
                _Thread_Enable_dispatch();

                /* return no memory error */
                return RTEMS_NO_MEMORY;
            }

            /* initialize the new variable */
            new->gval = *ptr;
            new->ptr = ptr;
            new->dtor = dtor;
            new->next = (struct rtems_task_variable_tt *) the_thread->task_variables;

            /* place the new variable in the thread */
            the_thread->task_variables = new;

            /* enable dispatch (disabled by _Thread_Get) */
            _Thread_Enable_dispatch();

            /* return success */
            return RTEMS_SUCCESSFUL;

            /* default clause: the object location is invalid */
        default:

            /* an internal error occured and the object location is invalid */
            return RTEMS_INTERNAL_INVALID_OBJECT_LOCATION;
    }
}

/**  
 *  @}
 */

/**
 *  @}
 */
