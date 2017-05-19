/**
 *  @file
 *  tasks.inl
 *
 *  @brief static inline implementation of all inlined
 *  routines in the with RTEMS Tasks Manager.
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

#ifndef _RTEMS_RTEMS_TASKS_INL
#define _RTEMS_RTEMS_TASKS_INL


/**
 *  @brief allocate task
 *  
 *  This function allocates a task control block from
 *  the inactive chain of free task control blocks.
 *
 *  @return returns the allocated task
 */
RTEMS_INLINE_ROUTINE Thread_Control *_RTEMS_tasks_Allocate(void)
{
   /* allocate the task object information */
   return (Thread_Control *) _Objects_Allocate(&_RTEMS_tasks_Information);
}


/**
 *  @brief free a task
 *  
 *  This routine frees a task control block to the
 *  inactive chain of free task control blocks.
 *
 *  @param[in] the_task the task to free
 */
RTEMS_INLINE_ROUTINE void _RTEMS_tasks_Free(
                                            Thread_Control *the_task
                                            )
{
   /* free the task object information with
    *   the information table of the task
    *   the task object to free */
   _Objects_Free(_Objects_Get_information(the_task->Object.id) ,
                 &the_task->Object);
}


/**
 *  @brief convert an RTEMS API priority to a RTEMS core priority
 *  
 *  This function converts an RTEMS API priority into a core priority.
 *
 *  @param[in] priority the priority to convert
 *
 *  @return returns the converted priority
 */
RTEMS_INLINE_ROUTINE Priority_Control _RTEMS_tasks_Priority_to_Core(
                                                                    rtems_task_priority priority
                                                                    )
{
   /* there is no actual conversion */
   return (Priority_Control) priority;
}


/**
 *  @brief determine if a RTEMS API priority is valid
 *
 *  @param[in] the_priority the priority to determine if it is valid
 *
 *  @return returns TRUE if the_priority is a valid user task priority
 *  and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE boolean _RTEMS_tasks_Priority_is_valid(
                                                            rtems_task_priority the_priority
                                                            )
{
   /* the priority must inside the interval
    * [RTEMS_MINIMUM_PRIORITY; RTEMS_MAXIMUM_PRIORITY] */
   return ( ( the_priority >= RTEMS_MINIMUM_PRIORITY ) &&
            ( the_priority <= RTEMS_MAXIMUM_PRIORITY ) );
}

#endif
/* end of include file */

/**  
 *  @}
 */

/**
 *  @}
 */
