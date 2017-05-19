/**
 *  @file
 *  semflush.c
 *
 *  @brief flush a semaphore
 *
 *  This package is the implementation of the flush directive
 *  of the Semaphore Manager.
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
 *  581     | 17/11/2008  | mcoutinho    | IPR 64
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  7103    | 09/04/2010  | mcoutinho    | IPR 1931
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
    #define SEND_OBJECT_WAS_DELETED _Semaphore_MP_Send_object_was_deleted
#endif


rtems_status_code rtems_semaphore_flush(
                                        rtems_id id
                                        )
{
    /* semaphore to flush */
    register Semaphore_Control *the_semaphore;

    /* semaphore to flush location */
    Objects_Locations location;


    /* get the semaphore */
    the_semaphore = _Semaphore_Get(id , &location);

    /* check the semaphore location */
    switch(location)
    {

#if defined(RTEMS_MULTIPROCESSING)
 /* cannot flush remote semaphore */

            /* semaphore is remote */
        case OBJECTS_REMOTE:

            /* re-enable thread dispatch */
            _Thread_Dispatch();

            /* return invalid operation on remote semaphore */
            return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

            /* semaphore id is invalid */
        case OBJECTS_ERROR:

            /* return invalid identifier error */
            return RTEMS_INVALID_ID;

            /* semaphore is local (nominal case) */
        case OBJECTS_LOCAL:

            /* check if it is a counting semaphore */
            if(!_Attributes_Is_counting_semaphore(the_semaphore->attribute_set))
            {
                /* flush the mutex */
                _CORE_mutex_Flush(&the_semaphore->Core_control.mutex ,
#if defined(RTEMS_MULTIPROCESSING)
                    SEND_OBJECT_WAS_DELETED ,
#endif
                    CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT);
            }
            else
            {
                /* flush the counting semaphore */
                _CORE_semaphore_Flush(&the_semaphore->Core_control.semaphore ,
#if defined(RTEMS_MULTIPROCESSING)
                    SEND_OBJECT_WAS_DELETED ,
#endif
                    CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT);
            }

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
