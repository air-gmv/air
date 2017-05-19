/**
 *  @file
 *  taskident.c
 *
 *  @brief identify a task based on its name
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
 *  7111    | 09/04/2010  | mcoutinho    | IPR 1931
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


rtems_status_code rtems_task_ident(
                                   rtems_name name ,
                                   uint32_t node ,
                                   Objects_Id *id
                                   )
{
    /* status of the object name to id translation */
    Objects_Name_or_id_lookup_errors status;


    /* check the idendifer address */
    if(!id)
    {
        /* if null, return invalid address error */
        return RTEMS_INVALID_ADDRESS;
    }

    /* check the easiest case first */

    /* check if trying to get the id of the calling thread */
    if(name == OBJECTS_ID_OF_SELF)
    {
        /* get the id of the executing thread */
        *id = _Thread_Executing->Object.id;

        /* return success */
        return RTEMS_SUCCESSFUL;
    }

    /* get the object name of the task object information */
    status = _Objects_Name_to_id(&_RTEMS_tasks_Information ,
                                 (Objects_Name) name ,
#if defined(RTEMS_MULTIPROCESSING)
        node ,
#endif
        id);

    /* return the translated error */
    return _Status_Object_name_errors_to_status[ status ];
}

/**  
 *  @}
 */

/**
 *  @}
 */
