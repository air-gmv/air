/**
 *  @file
 *  ratemondelete.c
 *
 *  @brief delete a rate monotonic period
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
 *  578     | 17/11/2008  | mcoutinho    | IPR 64
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  7094    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_RATEMON Rate Monotonic Manager
 *  @{
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/ratemon.h>
#include <rtems/score/thread.h>


rtems_status_code rtems_rate_monotonic_delete(
                                              Objects_Id id
                                              )
{
    /* rate monotonic object to delete */
    Rate_monotonic_Control *the_period;

    /* rate monotonic object to delete location  */
    Objects_Locations location;


    /* get the period */
    the_period = _Rate_monotonic_Get(id , &location);

    /* check the rate monotonic object location */
    switch(location)
    {

#if defined(RTEMS_MULTIPROCESSING)
            /* rate monotonic object is remote */
        case OBJECTS_REMOTE:

            /* should never return this */

            /* return internal error */
            return RTEMS_INTERNAL_ERROR;
#endif

            /* rate monotonic object id is invalid */
        case OBJECTS_ERROR:

            /* return invalid indentifier */
            return RTEMS_INVALID_ID;

            /* rate monotonic object is local (nominal case) */
        case OBJECTS_LOCAL:

            /* close the period object information */
            _Objects_Close(&_Rate_monotonic_Information , &the_period->Object);

            /* remove the period watchdog */
            (void) _Watchdog_Remove(&the_period->Timer);
            the_period->state = RATE_MONOTONIC_INACTIVE;

            /* free the period */
            _Rate_monotonic_Free(the_period);

            /* enable dispatch (disabled by _Rate_monotonic_Get) */
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
