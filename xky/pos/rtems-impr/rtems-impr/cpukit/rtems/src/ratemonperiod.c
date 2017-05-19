/**
 *  @file
 *  ratemonperiod.c
 *
 *  @brief rate monotonic period
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
 *  577     | 17/11/2008  | mcoutinho    | IPR 64
 *  577     | 17/11/2008  | mcoutinho    | IPR 70
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5317    | 02/11/2009  | mcoutinho    | IPR 831
 *  7097    | 09/04/2010  | mcoutinho    | IPR 1931
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


rtems_status_code rtems_rate_monotonic_period(
                                              Objects_Id id ,
                                              rtems_interval length
                                              )
{
    /* rate monotonic object to wait for period */
    Rate_monotonic_Control *the_period;

    /* rate monotonic object to wait for period location */
    Objects_Locations location;

    /* the value that will be returned by this function */
    rtems_status_code res;

    /* state of the rate monotonic object */
    rtems_rate_monotonic_period_states local_state;

    /* interrupt level */
    ISR_Level level;


    /* get the period */
    the_period = _Rate_monotonic_Get(id , &location);

    /* check the rate monotonic object location */
    switch(location)
    {

#if defined(RTEMS_MULTIPROCESSING)
        case OBJECTS_REMOTE: /* should never return this */
            return RTEMS_INTERNAL_ERROR;
#endif

        case OBJECTS_ERROR:
            return RTEMS_INVALID_ID;

            /* rate monotonic object is local (nominal case) */
        case OBJECTS_LOCAL:

            /* check if the executing thread is the owner */
            if(!_Thread_Is_executing(the_period->owner))
            {
                /* re-enable thread dispatch (disabled by _Rate_monotonic_Get) */
                _Thread_Enable_dispatch();

                /* return not owner of resource error */
                return RTEMS_NOT_OWNER_OF_RESOURCE;
            }

            /* check if the application is requesting the status of the period */
            if(length == RTEMS_PERIOD_STATUS)
            {
                /* check the period state */
                switch(the_period->state)
                {
                        /* state inactive (initial state) */
                    case RATE_MONOTONIC_INACTIVE:

                        /* set the return value to not defined */
                        res = RTEMS_NOT_DEFINED;

                        /* and leave */
                        break;

                        /* state active */
                    case RATE_MONOTONIC_ACTIVE:

                        /* set the return value to success */
                        res = RTEMS_SUCCESSFUL;

                        /* and leave */
                        break;

                        /* state expired */
                    case RATE_MONOTONIC_EXPIRED:

                        /* set the return value to timeout */
                        res = RTEMS_TIMEOUT;

                        /* and leave */
                        break;

                        /* period state is invalid (internal error) */
                    default:

                        /* set the return code to invalid period state */
                        res = RTEMS_INTERNAL_INVALID_PERIOD_STATE;

                        /* and leave */
                        break;
                }

                /* enable dispatch (disabled by _Rate_monotonic_Get) */
                _Thread_Enable_dispatch();

                /* return the status according to the state 
                 * note: this could be done faster with a vector */
                return (res );
            }

            /* enter critical section */
            _ISR_Disable(level);

            /* check the period state */
            switch(the_period->state)
            {
                    /* if it is inactive (never activated) */
                case RATE_MONOTONIC_INACTIVE:

                    /* leave critical section */
                    _ISR_Enable(level);

                    /* set the state to active */
                    the_period->state = RATE_MONOTONIC_ACTIVE;

                    /* initialize the watchdog */
                    _Watchdog_Initialize(&the_period->Timer ,
                                         _Rate_monotonic_Timeout ,
                                         id ,
                                         NULL);

                    the_period->next_length = length;

                    /* insert the period into the watchdog clock tick chain */
                    _Watchdog_Insert_ticks(&the_period->Timer , length);

                    /* enable dispatch (disabled by _Rate_monotonic_Get) */
                    _Thread_Enable_dispatch();

                    /* return success (does not block if the state was inactive) */
                    return RTEMS_SUCCESSFUL;


                    /* case period is active */
                case RATE_MONOTONIC_ACTIVE:

                    /* This tells the _Rate_monotonic_Timeout that this task is
                     * in the process of blocking on the period and that we
                     * may be changing the length of the next period */

                    the_period->state = RATE_MONOTONIC_OWNER_IS_BLOCKING;
                    the_period->next_length = length;

                    /* leave critical section */
                    _ISR_Enable(level);

                    /* set the thread wait information */
                    _Thread_Executing->Wait.id = the_period->Object.id;

                    /* set the thread state to waiting for period */
                    _Thread_Set_state(_Thread_Executing , STATES_WAITING_FOR_PERIOD);

                    /* Did the watchdog timer expire while we were actually blocking
                     * on it (two instructions above)? */

                    /* enter critical section */
                    _ISR_Disable(level);

                    /* save the period state */
                    local_state = the_period->state;

                    /* and place it as active */
                    the_period->state = RATE_MONOTONIC_ACTIVE;

                    /* leave critical section */
                    _ISR_Enable(level);

                    /* If it did, then we want to unblock ourself and continue as
                     * if nothing happen.  The period was reset in the timeout routine */

                    /* check if the state was expired while blocking */
                    if(local_state == RATE_MONOTONIC_EXPIRED_WHILE_BLOCKING)
                    {
                        /* clear the state waiting for period */
                        _Thread_Clear_state(_Thread_Executing , STATES_WAITING_FOR_PERIOD);
                    }

                    /* enable dispatch (disabled by _Rate_monotonic_Get) */
                    _Thread_Enable_dispatch();

                    /* return success */
                    return RTEMS_SUCCESSFUL;

                    /* case period has expired */
                case RATE_MONOTONIC_EXPIRED:

                    /* leave critical section */
                    _ISR_Enable(level);

                    /* set the period state to active (again) */
                    the_period->state = RATE_MONOTONIC_ACTIVE;

                    the_period->next_length = length;

                    /* insert the period into the tick watchdog chain */
                    _Watchdog_Insert_ticks(&the_period->Timer , length);

                    /* enable dispatch (disabled by _Rate_monotonic_Get) */
                    _Thread_Enable_dispatch();

                    /* and return timeout error */
                    return RTEMS_TIMEOUT;


                    /* default clause: the period state is invalid */
                default:

                    /* re-enable interrupts */
                    _ISR_Enable(level);

                    /* enable dispatch (disabled by _Rate_monotonic_Get) */
                    _Thread_Enable_dispatch();

                    /* and return invalid period state */
                    return RTEMS_INTERNAL_INVALID_PERIOD_STATE;
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
