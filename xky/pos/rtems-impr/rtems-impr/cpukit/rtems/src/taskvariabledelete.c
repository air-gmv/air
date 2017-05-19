/**
 *  @file
 *  taskvariabledelete.c
 *
 *  @brief delete a per task variable
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
 *  7123    | 09/04/2010  | mcoutinho    | IPR 1931
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


rtems_status_code rtems_task_variable_delete(
                                             rtems_id tid ,
                                             void **ptr
                                             )
{
    /* task to delete the variable */
    Thread_Control *the_thread;

    /* task to delete the variable location */
    Objects_Locations location;

    /* list of variables */
    rtems_task_variable_t *tvp , *prev;

    /* check if the variable is not NULL */
    if(!ptr)
    {
        /* return invalid variable address */
        return RTEMS_INVALID_ADDRESS;
    }

    /* initially there is no previous variable */
    prev = NULL;

    /* get the thread */
    the_thread = _Thread_Get(tid , &location);

    /* check the task location */
    switch(location)
    {

#if defined(RTEMS_MULTIPROCESSING)

            /* task is remote */
        case OBJECTS_REMOTE:

            /* dispatch tasks */
            _Thread_Dispatch();

            /* set the return value to illegal operation */
            return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

            /* task id is invalid */
        case OBJECTS_ERROR:

            /* return invalid identifier error */
            return RTEMS_INVALID_ID;

            /* task is local (nominal case) */
        case OBJECTS_LOCAL:

            /* figure out if the variable is already in this task's list */

            /* get the thread variable list */
            tvp = the_thread->task_variables;

            /* iterate through the variable list */
            while(tvp)
            {
                /* if the pointer is equal */
                if(tvp->ptr == ptr)
                {
                    /* if there is a previous variable */
                    if(prev)
                    {
                        /* update the pointer of the previous variable to the next one
                         * (node is being removed from list) */
                        prev->next = tvp->next;
                    }
                    else
                    {
                        /* otherwise, initialize the task variables to the next variable */
                        the_thread->task_variables = (rtems_task_variable_t *) tvp->next;
                    }


                    /* handle the destructor now */

                    /* if the calling thread is the one deleting the variable */
                    if(_Thread_Is_executing(the_thread))
                    {
                        /* if there is a destructor */
                        if(tvp->dtor)
                        {
                            /* call the destructor now */
                            ( *tvp->dtor )( *tvp->ptr );
                        }
                        /* update the pointer */
                        *tvp->ptr = tvp->gval;
                    }
                    else
                    {
                        /* if there is a destructor */
                        if(tvp->dtor)
                        {
                            /* call the destructor now */
                            ( *tvp->dtor )( tvp->tval );
                        }
                    }

                    /* free the memory of the variable */
                    _Workspace_Free(tvp);

                    /* enable dispatch (disabled by _Thread_Get) */
                    _Thread_Enable_dispatch();

                    /* return success */
                    return RTEMS_SUCCESSFUL;
                }

                /* didnt find the variable */

                /* search following variable on the list */
                prev = tvp;
                tvp = (rtems_task_variable_t *) tvp->next;
            }

            /* enable dispacth (disabled by _Thread_Get) */
            _Thread_Enable_dispatch();

            /* didnt find the variable, return invalid address error */
            return RTEMS_INVALID_ADDRESS;

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
