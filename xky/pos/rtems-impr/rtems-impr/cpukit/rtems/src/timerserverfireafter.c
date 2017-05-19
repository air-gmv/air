/**
 *  @file
 *  timerserverfireafter.c
 *
 *  @brief fire a timer after a specified number of clock ticks (in a task
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


rtems_status_code rtems_timer_server_fire_after(
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


    /* check if the timer server has been initialized */
    if(!_Timer_Server)
    {
        /* return incorrect state error */
        return RTEMS_INCORRECT_STATE;
    }

    /* check if the routine is not NULL */
    if(!routine)
    {
        /* return invalid address error */
        return RTEMS_INVALID_ADDRESS;
    }

    /* check if the number of clock ticks is valid */
    if(ticks == 0)
    {
        /* return invalid number error */
        return RTEMS_INVALID_NUMBER;
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

            /* remove the timer watchdog */
            (void) _Watchdog_Remove(&the_timer->Ticker);

            /* enter critical section */
            _ISR_Disable(level);

            /* check to see if the watchdog has just been inserted by a
             * higher priority interrupt.  If so, abandon this insert */
            if(the_timer->Ticker.state != WATCHDOG_INACTIVE)
            {
                /* leave critical section */
                _ISR_Enable(level);

                /* enable thread dispatch */
                _Thread_Enable_dispatch();

                /* return success */
                return RTEMS_SUCCESSFUL;
            }

            /* now we now the timer was not rescheduled by an interrupt
             * so we can atomically initialize it as in use */

            /* set the timer class as interval (wait for number of clock
                     * ticks to elapse) */
            the_timer->the_class = TIMER_INTERVAL_ON_TASK;

            /* initialize the timer watchdog */
            _Watchdog_Initialize(&the_timer->Ticker , routine , id , user_data);

            /* set the number of ticks to wake up */
            the_timer->Ticker.initial = ticks;

            /* leave critical section */
            _ISR_Enable(level);

            /* remove previous timer server clock ticks timer */
            _Timer_Server_stop_ticks_timer();

            /* process the timer server ticks chain */
            _Timer_Server_process_ticks_chain();

            /* insert the timer watchdog on the timer ticks chain */
            _Watchdog_Insert(&_Timer_Ticks_chain , &the_timer->Ticker);

            /* reset the timer server clock ticks timer */
            _Timer_Server_reset_ticks_timer();

            /* enable thread dispatch */
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
