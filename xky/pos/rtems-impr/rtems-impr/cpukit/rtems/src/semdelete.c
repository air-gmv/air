/**
 *  @file
 *  semdelete.c
 *
 *  @brief delete a semaphore
 *
 *  This package is the implementation of the Semaphore Manager.
 *  This manager utilizes standard Dijkstra counting semaphores to provide
 *  synchronization and mutual exclusion capabilities.
 *
 *  Directives provided are:\n
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
 *  580     | 17/11/2008  | mcoutinho    | IPR 64
 *  3760    | 13/07/2009  | mcoutinho    | IPR 553
 *  3898    | 21/07/2009  | mcoutinho    | IPR 99
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  7102    | 09/04/2010  | mcoutinho    | IPR 1931
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

#if defined(RTEMS_MULTIPROCESSING)
    #define SEMAPHORE_MP_OBJECT_WAS_DELETED _Semaphore_MP_Send_object_was_deleted
#endif


rtems_status_code rtems_semaphore_delete(
                                         rtems_id id
                                         )
{
    /* semaphore to delete */
    register Semaphore_Control *the_semaphore;

    /* semaphore to delete location */
    Objects_Locations location;


    /* get the semaphore */
    the_semaphore = _Semaphore_Get(id , &location);

    /* check the semaphore location */
    switch(location)
    {

#if defined(RTEMS_MULTIPROCESSING)

            /* semaphore is remote */
        case OBJECTS_REMOTE:

            /* re-enable thread dispatch */
            _Thread_Dispatch();

            /* return that cannot delete remote semaphore */
            return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

            /* semaphore id is invalid */
        case OBJECTS_ERROR:

            /* return invalid id error */
            return RTEMS_INVALID_ID;

            /* semaphore is local (nominal case) */
        case OBJECTS_LOCAL:

            /* check if it is not counting */
            if(!_Attributes_Is_counting_semaphore(the_semaphore->attribute_set))
            {
                /* check if it a locked simple binary */
                if(_CORE_mutex_Is_locked(&the_semaphore->Core_control.mutex) &&
                   !_Attributes_Is_simple_binary_semaphore(the_semaphore->attribute_set))
                {
                    /* do not allow locked simple binary semaphores to be deleted */

                    /* re-enable thread dispatch */
                    _Thread_Enable_dispatch();

                    /* return resource in use */
                    return RTEMS_RESOURCE_IN_USE;
                }

                /* get the mutex */
                CORE_mutex_Control *mutex = &the_semaphore->Core_control.mutex;

                /* if the mutex is locked (and not simple binary) */
                if(_CORE_mutex_Is_locked(mutex))
                {
                    /* then extract it from the chain of hold mutexes of the
                     * holder thread */
                    _Chain_Extract(&mutex->Node);

                    /* and reset the node */
                    mutex->Node.next = NULL;
                    mutex->Node.previous = NULL;
                }

                /* flush the mutex */
                _CORE_mutex_Flush(&the_semaphore->Core_control.mutex ,
#if defined(RTEMS_MULTIPROCESSING)
                    SEMAPHORE_MP_OBJECT_WAS_DELETED ,
#endif
                    CORE_MUTEX_WAS_DELETED);
            }
            else
            {
                /* is counting so is a core semaphore instead of a core mutex */

                /* flush the core semaphore */
                _CORE_semaphore_Flush(&the_semaphore->Core_control.semaphore ,
#if defined(RTEMS_MULTIPROCESSING)
                    SEMAPHORE_MP_OBJECT_WAS_DELETED ,
#endif
                    CORE_SEMAPHORE_WAS_DELETED);
            }

            /* close the object information of the semaphore */
            _Objects_Close(&_Semaphore_Information , &the_semaphore->Object);

            /* free the resources of the semaphore */
            _Semaphore_Free(the_semaphore);

#if defined(RTEMS_MULTIPROCESSING)
            /* check if semaphore is global */
            if(_Attributes_Is_global(the_semaphore->attribute_set))
            {
                /* close multiprocessing object information */
                _Objects_MP_Close(&_Semaphore_Information , the_semaphore->Object.id);

                /* send a message to announce the semaphore was deleted */
                _Semaphore_MP_Send_process_packet(SEMAPHORE_MP_ANNOUNCE_DELETE ,
                                                  the_semaphore->Object.id ,
                                                  0 , /* Not used */
                                                  0 /* Not used */);
            }
#endif
            /* enable dispatch (disabled by _Semaphore_Get) */
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
