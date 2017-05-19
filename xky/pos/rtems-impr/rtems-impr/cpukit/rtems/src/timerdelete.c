/**
 *  @file
 *  timerdelete.c
 *
 *  @brief delete a timer
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  590     | 17/11/2008  | mcoutinho    | IPR 64
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  7124    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_TIMER Timer Manager
 *  @{
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/timer.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>


rtems_status_code rtems_timer_delete(
                                     Objects_Id id
                                     )
{
    /* timer to delete */
    Timer_Control *the_timer;

    /* timer to delete location */
    Objects_Locations location;


    /* get the timer */
    the_timer = _Timer_Get(id , &location);

    /* check the timer location */
    switch(location)
    {
#if defined(RTEMS_MULTIPROCESSING)
        
        /* timer is remote */
        case OBJECTS_REMOTE: 
            
            /* should never return this */

            /* return internal error */
            return RTEMS_INTERNAL_ERROR;
#endif

            /* timer id is invalid */
        case OBJECTS_ERROR:

            /* return invalid identifier error */
            return RTEMS_INVALID_ID;

            /* timer is local (nominal case) */
        case OBJECTS_LOCAL:

            /* close the timer object information */
            _Objects_Close(&_Timer_Information , &the_timer->Object);

            /* remove the watchdog */
            (void) _Watchdog_Remove(&the_timer->Ticker);

            /* free the timer memory */
            _Timer_Free(the_timer);

            /* enable dispatch (disabled by _Timer_Get) */
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
