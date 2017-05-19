/**
 *  @file
 *  semcreate.c
 *
 *  @brief create a semaphore
 *
 *  This package is the implementation of the Semaphore Manager.
 *  This manager utilizes standard Dijkstra counting semaphores to provide
 *  synchronization and mutual exclusion capabilities.
 *
 *  Directives provided are:
 *
 *     + create a semaphore\n
 *     + get an ID of a semaphore\n
 *     + delete a semaphore\n
 *     + acquire a semaphore\n
 *     + release a semaphore\n
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
 *  3223    | 15/05/2009  | mcoutinho    | IPR 98
 *  3263    | 22/05/2009  | mcoutinho    | IPR 99
 *  3760    | 13/07/2009  | mcoutinho    | IPR 463
 *  3760    | 13/07/2009  | mcoutinho    | IPR 553
 *  4402    | 21/09/2009  | mcoutinho    | IPR 580
 *  4530    | 25/09/2009  | mcoutinho    | IPR 776
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  7101    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_SEMAPHORE Semaphore Manager
 *  @{
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/attr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>

#include <rtems/score/priority.inl>
#include <rtems/rtems/sem.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/coresem.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#if defined(RTEMS_MULTIPROCESSING)
    #include <rtems/score/mpci.h>
#endif
#include <rtems/score/sysstate.h>

#include <rtems/score/interr.h>


rtems_status_code rtems_semaphore_create(
                                         rtems_name name ,
                                         uint32_t count ,
                                         rtems_attribute attribute_set ,
                                         rtems_task_priority priority_ceiling ,
                                         rtems_id *id
                                         )
{
    /* the new semaphore */
    register Semaphore_Control *the_semaphore;

    /* the new CORE mutex attributes (if simple binary or binary semaphore) */
    CORE_mutex_Attributes the_mutex_attributes;

    /* the new CORE semaphore attributes (counting semaphore) */
    CORE_semaphore_Attributes the_semaphore_attributes;

    /* core mutex is locked or not */
    uint32_t lock;


    /* check if the name is valid */
    if(!rtems_is_name_valid(name))
    {
        /* return invalid name */
        return RTEMS_INVALID_NAME;
    }

    /* check if id is valid */
    if(!id)
    {
        /* return invalid id address */
        return RTEMS_INVALID_ADDRESS;
    }

#if defined(RTEMS_MULTIPROCESSING)

    /* check if the semaphore is global */
    if(_Attributes_Is_global(attribute_set))
    {
        /* check if multiprocessing is enabled */
        if(!_System_state_Is_multiprocessing)
        {
            /* return multiprocessing not configured */
            return RTEMS_MP_NOT_CONFIGURED;
        }

        /* check if the semaphore has priority inheritance */
        if(_Attributes_Is_inherit_priority(attribute_set))
        {
            /* return not defined */
            return RTEMS_NOT_DEFINED;
        }
    }
    else
#endif

        /* if the semaphore is inheritance or ceiling */
        if(_Attributes_Is_inherit_priority(attribute_set) ||
           _Attributes_Is_priority_ceiling(attribute_set))
    {

        /* check if it is binary and has priority protocol */
        if(!( _Attributes_Is_binary_semaphore(attribute_set) &&
              _Attributes_Is_priority(attribute_set) ))
        {
            /* if it is not binary or is not priority protocol then
             * it is not possible to be inheritance or ceiling */
            return RTEMS_NOT_DEFINED;
        }
    }

    /* semaphore cannot have both priority ceiling and priority inherance protocols */
    if(_Attributes_Is_inherit_priority(attribute_set) &&
       _Attributes_Is_priority_ceiling(attribute_set))
    {
        /* return not defined */
        return RTEMS_NOT_DEFINED;
    }

    /* ceiling semaphore lock must have been locked by a lower priority thread */
    if(_Attributes_Is_priority_ceiling(attribute_set) && count == 0 &&
       priority_ceiling > _Thread_Executing->current_priority)
    {
        /* return invalid priority */
        return RTEMS_INVALID_PRIORITY;
    }

    /* check if the priority ceiling value is valid */
    if(_Attributes_Is_priority_ceiling(attribute_set) && !_Priority_Is_valid(priority_ceiling))
    {
        /* return invalid priority */
        return RTEMS_INVALID_PRIORITY;
    }

    /* check if the semaphore is binary or simple binary and count is larger than 1 */
    if(!_Attributes_Is_counting_semaphore(attribute_set) && ( count > 1 ))
    {
        /* return invalid number (count is too big) */
        return RTEMS_INVALID_NUMBER;
    }

    /* prevents deletion */
    _Thread_Disable_dispatch();

    /* allocate the semaphore */
    the_semaphore = _Semaphore_Allocate();

    /* check if the semaphore was allocated */
    if(!the_semaphore)
    {
        /* re-enable dispatch and return error */
        _Thread_Enable_dispatch();

        /* return too many semaphores created */
        return RTEMS_TOO_MANY;
    }

#if defined(RTEMS_MULTIPROCESSING)
    /* if the semaphore is global and could not allocate and open remotely*/
    if(_Attributes_Is_global(attribute_set) &&
       !( _Objects_MP_Allocate_and_open(&_Semaphore_Information , name ,
                                        the_semaphore->Object.id , FALSE) ))
    {
        /* then undo previous steps */

        /* free the semaphore */
        _Semaphore_Free(the_semaphore);

        /* re-enable thread dispatch */
        _Thread_Enable_dispatch();

        /* and return too many semaphores created */
        return RTEMS_TOO_MANY;
    }
#endif

    /* semaphore was correctly allocated, continue creating it */

    /* set the semaphore attribute set */
    the_semaphore->attribute_set = attribute_set;

    /* If it is not a counting semaphore, then it is either a
     * simple binary semaphore or a more powerful mutex style binary
     * semaphore */
    if(!_Attributes_Is_counting_semaphore(attribute_set))
    {
        /* semaphore is NOT counting */

        /* check if the semaphore is inherit priority */
        if(_Attributes_Is_inherit_priority(attribute_set))
        {
            /* set mutex discipline to priority inheritance protocol */
            the_mutex_attributes.discipline = CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT;
        }
            /* check if it is ceiling priority */
        else if(_Attributes_Is_priority_ceiling(attribute_set))
        {
            /* set mutex discipline to priority ceuling protocol */
            the_mutex_attributes.discipline = CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING;
        }
            /* check if the queueing policy is priority */
        else if(_Attributes_Is_priority(attribute_set))
        {
            /* set mutex queueing discipline to priority */
            the_mutex_attributes.discipline = CORE_MUTEX_DISCIPLINES_PRIORITY;
        }
            /* is not inherit, nor ceiling, not priority queueing, so can only
             * be FIFO queueing */
        else
        {
            /* set mutex queueing discipline to FIFO */
            the_mutex_attributes.discipline = CORE_MUTEX_DISCIPLINES_FIFO;
        }

        /* check if the semaphore is binary */
        if(_Attributes_Is_binary_semaphore(attribute_set))
        {
            /* mutex is binary */

            /* initialize the lock nesting behavior for binary semaphores */
            the_mutex_attributes.lock_nesting_behavior = CORE_MUTEX_NESTING_ACQUIRES;

            /* set that the only the owner can release it */
            the_mutex_attributes.only_owner_release = TRUE;

        }
        else
        {
            /* mutex is not binary, so can only be simple binary */

            /* initialize the lock nesting behavior for non-binary semaphores */
            the_mutex_attributes.lock_nesting_behavior = CORE_MUTEX_NESTING_BLOCKS;

            /* set that any task can release the mutex */
            the_mutex_attributes.only_owner_release = FALSE;
        }

        /* set the mutex priority ceiling */
        the_mutex_attributes.priority_ceiling = priority_ceiling;

        /* check the internal count */
        if(count == 1)
        {
            /* mutex initially unlocked */
            lock = CORE_MUTEX_UNLOCKED;
        }
        else
        {
            /* mutex initially locked */
            lock = CORE_MUTEX_LOCKED;
        }

        /* initialize the mutex */
        _CORE_mutex_Initialize(&the_semaphore->Core_control.mutex ,
                               &the_mutex_attributes ,
                               lock ,
                               (ptrSemaphore_Control) the_semaphore);
    }
    else
    {
        /* else is a counting semaphore */

        /* check the queueing discipline */
        if(_Attributes_Is_priority(attribute_set))
        {
            /* set the semaphore discipline to priority queueing */
            the_semaphore_attributes.discipline = CORE_SEMAPHORE_DISCIPLINES_PRIORITY;
        }
        else
        {
            /* set the semaphore discipline to FIFO queueing */
            the_semaphore_attributes.discipline = CORE_SEMAPHORE_DISCIPLINES_FIFO;
        }

        /* this effectively disables limit checking. */
        the_semaphore_attributes.maximum_count = 0xFFFFFFFF;

        /* the following are just to make Purify happy */

        the_mutex_attributes.lock_nesting_behavior = CORE_MUTEX_NESTING_ACQUIRES;
        the_mutex_attributes.priority_ceiling = PRIORITY_MINIMUM;

        /* initialize the semaphore */
        _CORE_semaphore_Initialize(&the_semaphore->Core_control.semaphore ,
                                   &the_semaphore_attributes ,
                                   count);
    }

    /* open an object */
    _Objects_Open(&_Semaphore_Information ,
                  &the_semaphore->Object ,
                  (Objects_Name) name);

    /* copy the assign identifier to the output variable id */
    *id = the_semaphore->Object.id;

#if defined(RTEMS_MULTIPROCESSING)
    /* if semaphore is global */
    if(_Attributes_Is_global(attribute_set))
    {
        /* send a packet to the remote note */
        _Semaphore_MP_Send_process_packet(SEMAPHORE_MP_ANNOUNCE_CREATE ,
                                          the_semaphore->Object.id ,
                                          name ,
                                          0 /* Not used */);
    }
#endif

    /* re-enable thread dispatch */
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
