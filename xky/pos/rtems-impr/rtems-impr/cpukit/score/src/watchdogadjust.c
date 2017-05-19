/**
 *  @file
 *  watchdogadjust.c
 *
 *  @brief adjust the watchdog handler forwards or backwards
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
 *  @addtogroup ScoreWatchdog Watchdog Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/watchdog.h>


void _Watchdog_Adjust(
                      Chain_Control *header ,
                      Watchdog_Adjust_directions direction ,
                      Watchdog_Interval units
                      )
{
    /* interrupt level */
    ISR_Level level;


    /* enter critical section */
    _ISR_Disable(level);

    /* NOTE: It is safe NOT to make 'header' a pointer
     *       to volatile data (contrast this with watchdoginsert.c)
     *       because we call _Watchdog_Tickle() below and
     *       hence the compiler must not assume *header to remain
     *       unmodified across that call.
     *       Till Straumann, 7/2003 */

    /* if the header chain is not empty */
    if(!_Chain_Is_empty(header))
    {
        /* then check the direction */
        switch(direction)
        {

                /* backward direction */
            case WATCHDOG_BACKWARD:

                /* increase the first node interval by the specified units */
                _Watchdog_First(header)->delta_interval += units;

                /* and leave */
                break;

                /* forward direction */
            case WATCHDOG_FORWARD:

                /* decrease the chain units */
                while(units)
                {
                    /* go through every node on the chain and decrease the number of units
                     * until the number of units has been decreased to 0*/

                    /* if the first node has a delta interval larger than the units */
                    if(units < _Watchdog_First(header)->delta_interval)
                    {
                        /* decrease then the first node by units */
                        _Watchdog_First(header)->delta_interval -= units;

                        /* and leave */
                        break;
                    }
                    else
                    {
                        /* decrease the number of units by the first node delta interval */
                        units -= _Watchdog_First(header)->delta_interval;

                        /* set the delta interval of the first node to 1 */
                        _Watchdog_First(header)->delta_interval = 1;

                        /* leave critical section */
                        _ISR_Enable(level);

                        /* tickle the watchdog (at least, first node will be activated) */
                        _Watchdog_Tickle(header);

                        /* enter critical section */
                        _ISR_Disable(level);

                        /* if chain is empty */
                        if(_Chain_Is_empty(header))
                        {
                            /* and leave */
                            break;
                        }

                        /* continue to search until units is 0 */
                    }
                }

                /* and leave */
                break;

                /* invalid wathdog direction */
            default:

                /* raise internal error */
                _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                         TRUE ,
                                         INTERNAL_ERROR_INVALID_WATCHDOG_DIRECTION);
        }
    }

    /* leave critical section */
    _ISR_Enable(level);
}

/**  
 *  @}
 */

/**
 *  @}
 */
