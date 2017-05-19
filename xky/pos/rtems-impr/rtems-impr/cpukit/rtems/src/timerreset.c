/**
 *  @file
 *  timerreset.c
 *
 *  @brief reset a timer
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


rtems_status_code rtems_timer_reset(
                                    Objects_Id id
                                    )
{
    /* timer to reset */
    Timer_Control *the_timer;

    /* timer to reset location */
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

            /* check the timer class */
            switch(the_timer->the_class)
            {
                    /* class is interval */
                case TIMER_INTERVAL:
                    /* remove the watchdog */
                    _Watchdog_Remove(&the_timer->Ticker);

                    /* and insert the watchdog on the watchdog ticks chain */
                    _Watchdog_Insert(&_Watchdog_Ticks_chain , &the_timer->Ticker);
                    break;

                    /* class is interval on timer server */
                case TIMER_INTERVAL_ON_TASK:

                    /* stop the timer server ticks timer */
                    _Timer_Server_stop_ticks_timer();

                    /* remove the timer watchdog */
                    _Watchdog_Remove(&the_timer->Ticker);

                    /* process the timer server ticks chain */
                    _Timer_Server_process_ticks_chain();

                    /* insert again the timer on the timer server watchdog ticks chain */
                    _Watchdog_Insert(&_Timer_Ticks_chain , &the_timer->Ticker);

                    /* reset timer server ticks timer */
                    _Timer_Server_reset_ticks_timer();

                    /* and leave */
                    break;

                    /* class is:
                     *   time of day or
                     *   time of day on timer server or
                     *   dormant */
                case TIMER_TIME_OF_DAY:
                case TIMER_TIME_OF_DAY_ON_TASK:
                case TIMER_DORMANT:

                    /* enable thread dispatch (disabled by _Timer_Get) */
                    _Thread_Enable_dispatch();

                    /* return not defined (for these class, cannot reset the
                     * timer) */
                    return RTEMS_NOT_DEFINED;

                    /* invalid class */
                default:

                    /* enable thread dispatch (disabled by _Timer_Get) */
                    _Thread_Enable_dispatch();

                    /* return internal error in timer class */
                    return RTEMS_INTERNAL_INVALID_TIMER_CLASS;
            }

            /* enable thread dispatch (disabled by _Timer_Get) */
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
