/**
 *  @file
 *  watchdogtickle.c
 *
 *  @brief announce the passage of a clock tick to the Watchdog Handler
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


void _Watchdog_Tickle(
                      Chain_Control *header
                      )
{
    /* interrupt level */
    ISR_Level level;

    /* watchdog iterator to activate */
    Watchdog_Control *the_watchdog;

    /* watchdog iterator to activate state */
    Watchdog_States watchdog_state;


    /* See the comment in watchdoginsert.c and watchdogadjust.c
     * about why it's safe not to declare header a pointer to
     * volatile data - till, 2003/7 */

    /* enter critical section */
    _ISR_Disable(level);

    /* check if the header is empty */
    if(_Chain_Is_empty(header))
    {
        /* it is, so leave */
        goto leave;
    }

    /* header is not empty */

    /* get the first watchdog of the chain header */
    the_watchdog = _Watchdog_First(header);

    /* For some reason, on rare occasions the_watchdog->delta_interval
     * of the head of the watchdog chain is 0.  Before this test was
     * added, on these occasions an event (which usually was supposed
     * to have a timeout of 1 tick would have a delta_interval of 0, which
     * would be decremented to 0xFFFFFFFF by the unprotected
     * "the_watchdog->delta_interval--;" operation.
     * This would mean the event would not timeout, and also the chain would
     * be blocked, because a timeout with a very high number would be at the
     * head, rather than at the end.
     * The test "if (the_watchdog->delta_interval != 0)"
     * here prevents this from occuring.
     * We were not able to categorically identify the situation that causes
     * this, but proved it to be true empirically.  So this check causes
     * correct behaviour in this circumstance.
     * The belief is that a race condition exists whereby an event at the head
     * of the chain is removed (by a pending ISR or higher priority task)
     * during the _ISR_Flash( level ); in _Watchdog_Insert, but the watchdog
     * to be inserted has already had its delta_interval adjusted to 0, and
     * so is added to the head of the chain with a delta_interval of 0.
     * Steven Johnson - 12/2005 (gcc-3.2.3 -O3 on powerpc) */

    /* if the watchdog delta interval is not 0 */
    if(the_watchdog->delta_interval != 0)
    {
        /* then decrease it */
        the_watchdog->delta_interval--;

        /* if the watchdog delta interval is not 0 */
        if(the_watchdog->delta_interval != 0)
        {
            /* then dont activate any watchdog */

            /* just leave */
            goto leave;
        }
    }

    do
    {
        /* remove the watchdog and get its state */
        watchdog_state = _Watchdog_Remove(the_watchdog);

        /* leave critical section */
        _ISR_Enable(level);

        /* check the removed watchdog state */
        switch(watchdog_state)
        {
            /* watchdog is active */
            case WATCHDOG_ACTIVE:

                /* call the watchdog routine */
                ( *the_watchdog->routine )( the_watchdog->id , the_watchdog->user_data );

                /* and leave */
                break;

                /* watchdog is inactive */
            case WATCHDOG_INACTIVE:

                /* This state indicates that the watchdog is not on any chain.
                 * Thus, it is NOT on a chain being tickled.  This case should
                 * never occur */
                break;

                /* watchdog is being inserted */
            case WATCHDOG_BEING_INSERTED:

                /* This state indicates that the watchdog is in the process of
                 * BEING inserted on the chain.  Thus, it can NOT be on a chain
                 * being tickled.  This case should never occur */
                break;

                /* watchdog is being removed */
            case WATCHDOG_REMOVE_IT:
                break;

                /* invalid watchdog state */
            default:

                /* raise internal error */
                _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                         TRUE ,
                                         INTERNAL_ERROR_INVALID_WATCHDOG_STATE);
        }

        /* enter critical section */
        _ISR_Disable(level);

        /* get the first watchdog */
        the_watchdog = _Watchdog_First(header);

        /* continue until chain is empty or the delta interval is 0 */
    } while(!_Chain_Is_empty(header) &&
            ( the_watchdog->delta_interval == 0 ));

    /* leave */
leave:
                
    /* leave critical section */
    _ISR_Enable(level);
}

/**  
 *  @}
 */

/**
 *  @}
 */
