/**
 *  @file
 *  semobtain.c
 *
 *  @brief obtain a semaphore
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
 *  5247    | 30/10/2009  | mcoutinho    | IPR 828
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  7105    | 09/04/2010  | mcoutinho    | IPR 1931
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


rtems_status_code rtems_semaphore_obtain(
                                         rtems_id id ,
                                         uint32_t option_set ,
                                         rtems_interval timeout
                                         )
{
    /* the semaphore to obtain */
    register Semaphore_Control *the_semaphore;

    /* the location of the semaphore */
    Objects_Locations location;

    /* the interrupt level */
    ISR_Level level;


    /* get the semaphore */
    the_semaphore = _Semaphore_Get_interrupt_disable(id , &location , &level);

    /* check the semaphore location */
    switch(location)
    {
#if defined(RTEMS_MULTIPROCESSING)
            /* semaphore is remote */
        case OBJECTS_REMOTE:
            
            /* send a multiprocessing packet to the remote node
             * to obtain the semaphore */
            return _Semaphore_MP_Send_request_packet(SEMAPHORE_MP_OBTAIN_REQUEST ,
                                                     id ,
                                                     option_set ,
                                                     timeout);
#endif

            /* semaphore id is invalid */
        case OBJECTS_ERROR:

            /* return invalid id */
            return RTEMS_INVALID_ID;

            /* semaphore is local (nominal case) */
        case OBJECTS_LOCAL:

            /* check if it is a counting semaphore */
            if(!_Attributes_Is_counting_semaphore(the_semaphore->attribute_set))
            {
                /* seize the mutex */
                _CORE_mutex_Seize(&the_semaphore->Core_control.mutex ,
                                  id ,
                                  !_Options_Is_no_wait(option_set) ,
                                  timeout ,
                                  level);

                /* return translated mutex return code */
                return _Semaphore_Translate_core_mutex_return_code(_Thread_Executing->Wait.return_code);
            }
            else
            {
                /* seize the semaphore */
                _CORE_semaphore_Seize_isr_disable(&the_semaphore->Core_control.semaphore ,
                                                  id ,
                                                  !_Options_Is_no_wait(option_set) ,
                                                  timeout ,
                                                  &level);

                /* return translated semaphore return code */
                return _Semaphore_Translate_core_semaphore_return_code(_Thread_Executing->Wait.return_code);
            }

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
