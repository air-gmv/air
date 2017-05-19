/**
 *  @file
 *  timerfireafter.c
 *
 *  @brief fire a timer after a specified number of clock ticks (in an ISR
 *  context)
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


rtems_status_code rtems_timer_fire_after(
                                         Objects_Id id ,
                                         rtems_interval ticks ,
                                         rtems_timer_service_routine_entry routine ,
                                         void *user_data
                                         )
{
    /* timer to fire after */
    Timer_Control *the_timer;

    /* timer to fire after location */
    Objects_Locations location;

    /* interrupt level */
    ISR_Level level;


    /* check if ticks is valid */
    if(ticks == 0)
    {
        /* return invalid number of clock ticks */
        return RTEMS_INVALID_NUMBER;
    }

    /* check if the timer routine is valid */
    if(!routine)
    {
        /* return invalid routine entry point address */
        return RTEMS_INVALID_ADDRESS;
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

            /* remove any previous timer watchdogs */
            (void) _Watchdog_Remove(&the_timer->Ticker);

            /* enter critical zone */
            _ISR_Disable(level);

            /* check to see if the watchdog has just been inserted by a
             * higher priority interrupt.  If so, abandon this insert */
            if(the_timer->Ticker.state != WATCHDOG_INACTIVE)
            {

                /* leave critical zone */
                _ISR_Enable(level);

                /* re-enable thread dispatch */
                _Thread_Enable_dispatch();

                /* return success */
                return RTEMS_SUCCESSFUL;
            }

            /* now we know the timer was not rescheduled by an interrupt
             * so we can atomically initialize it as in use */

            /* set the timer class to interval (fire after some clock ticks) */
            the_timer->the_class = TIMER_INTERVAL;

            /* initialize the watchdog */
            _Watchdog_Initialize(&the_timer->Ticker , routine , id , user_data);

            /* leave critical zone */
            _ISR_Enable(level);

            /* insert the watchdog on the clock ticks chain */
            _Watchdog_Insert_ticks(&the_timer->Ticker , ticks);

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
