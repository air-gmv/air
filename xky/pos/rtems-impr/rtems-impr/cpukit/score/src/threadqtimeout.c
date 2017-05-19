/**
 *  @file
 *  threadqtimeout.c
 *
 *  @brief announce that a timeout occurs to a specified object
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
 *  623     | 17/11/2008  | mcoutinho    | IPR 64
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreThreadQ Thread Queue Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tqdata.h>
#include <rtems/score/threadq.inl>


void _Thread_queue_Timeout(
                           Objects_Id id ,
                           void *ignored
                           )
{
    /* thread to timeout */
    Thread_Control *the_thread;

    /* thread location */
    Objects_Locations location;


    /* get the thread */
    the_thread = _Thread_Get(id , &location);

    /* check the thread location */
    switch(location)
    {

            /* thread id is invalid */
        case OBJECTS_ERROR:
            /* impossible */

#if defined(RTEMS_MULTIPROCESSING)

            /* thread is remote */
        case OBJECTS_REMOTE:
            /* impossible */

#endif
            /* just leave */
            break;

            /* thread is local (nominal case) */
        case OBJECTS_LOCAL:

            /* process the thread queue timeout */
            _Thread_queue_Process_timeout(the_thread);

            /* unnest thread dispatch */
            _Thread_Unnest_dispatch();

            /* and leave */
            break;

            /* invalid object location */
        default:

            /* raise internal error */
            _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                     TRUE ,
                                     INTERNAL_ERROR_INVALID_OBJECT_LOCATION);
    }
}

/**  
 *  @}
 */

/**
 *  @}
 */
