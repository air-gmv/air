/**
 *  @file
 *  timerfirewhen.c
 *
 *  @brief fire a timer at a given time wall (in an ISR context)
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


rtems_status_code rtems_timer_fire_when(
                                        Objects_Id id ,
                                        rtems_time_of_day *wall_time ,
                                        rtems_timer_service_routine_entry routine ,
                                        void *user_data
                                        )
{
    /* timer to fire when */
    Timer_Control *the_timer;

    /* timer to fire when location */
    Objects_Locations location;

    /* number of seconds since epoch to wake up the timer */
    rtems_interval seconds;


    /* check if the time of day has been set */
    if(!_TOD_Is_set)
    {
        /* return not defined error */
        return RTEMS_NOT_DEFINED;
    }

    /* check if the wall time is valid */
    if(!_TOD_Validate(wall_time))
    {
        /* return invalid clock error */
        return RTEMS_INVALID_CLOCK;
    }

    /* check if the timer routine is valid */
    if(!routine)
    {
        /* return invalid address error */
        return RTEMS_INVALID_ADDRESS;
    }

    /* convert wall_time to number of seconds since epoch */
    seconds = _TOD_To_seconds(wall_time);

    /* check if the wall_time has elapsed */
    if(seconds <= _TOD_Seconds_since_epoch)
    {
        /* return invalid clock error */
        return RTEMS_INVALID_CLOCK;
    }

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
            /* remove previous watchdog */
            (void) _Watchdog_Remove(&the_timer->Ticker);

            /* initialize the timer and watchdog */
            the_timer->the_class = TIMER_TIME_OF_DAY;

            /* initialize the timer watchdog */
            _Watchdog_Initialize(&the_timer->Ticker , routine , id , user_data);

            /* insert the watchdog in the seconds chain */
            _Watchdog_Insert_seconds(&the_timer->Ticker ,
                                     seconds - _TOD_Seconds_since_epoch);

            /*enable the dispatch (disabled by _Timer_Get) */
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
