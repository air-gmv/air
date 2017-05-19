/**
 *  @file
 *  watchdoginsert.c
 *
 *  @brief insert a watchdog
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
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  9293    | 03/01/2011  | mcoutinho    | IPR 451
 *  9293    | 03/01/2011  | mcoutinho    | IPR 2812
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


void _Watchdog_Insert(
                      Chain_Control *header ,
                      Watchdog_Control *the_watchdog
                      )
{
    /* interrupt level */
    ISR_Level level;

    /* watchdog after */
    Watchdog_Control *after;

    /* interrupt nesting level */
    uint32_t insert_isr_nest_level;

    /* delta interval */
    Watchdog_Interval delta_interval;


    /* enter critical section */
    _ISR_Disable(level);

    /* get interrupt nesting level */
    insert_isr_nest_level = _ISR_Nest_level;


    /*  Check to see if the watchdog has not just been inserted by a
     * higher priority interrupt */
    if(the_watchdog->state != WATCHDOG_INACTIVE)
    {
        /* leave critical section */
        _ISR_Enable(level);

        /* and return */
        return;
    }

    /* set the watchdog state to "being inserted" */
    the_watchdog->state = WATCHDOG_BEING_INSERTED;

    /* increment the watchdog synchronization count */
    _Watchdog_Sync_count++;

    /* restart label, while inserting, a new interrupt can occur, which can
     * oblige a new restart of the algorithm */
restart:

    /* set the delta interval */
    delta_interval = the_watchdog->initial;

    /* We CANT use _Watchdog_First() here, because a TICK interrupt
     * could modify the chain during the _ISR_Flash() below. Hence,
     * the header is pointing to volatile data. The _Watchdog_First()
     * INLINE routine (but not the macro - note the subtle difference)
     * casts away the 'volatile'...
     * Also, this is only necessary because we call no other routine
     * from this piece of code, hence the compiler thinks it's safe to
     * cache *header!!
     * Till Straumann, 7/2003 (gcc-3.2.2 -O4 on powerpc) */

    /* check the place where to insert the node (chronogical order) */
    for(after = (Watchdog_Control *) ( ( volatile Chain_Control * )header )->first;
        ;
        after = _Watchdog_Next(after))
    {

        /* if the delta interval is 0 or there is no next watchdog */
        if(delta_interval == 0 || !_Watchdog_Next(after))
        {
            /* then the search has finished */
            break;
        }

        /* check if the watchdog should be placed before the "after" watchdog */
        if(delta_interval < after->delta_interval)
        {
            /* update the watchdog on the chain delta interval */
            after->delta_interval -= delta_interval;

            /* watchdog has been inserted, finish */
            break;
        }

        /* still not this watchdog, search next */

        /* decrease the delta interval to search for */
        delta_interval -= after->delta_interval;

        /* If you experience problems comment out the _ISR_Flash line.
         * 3.2.0 was the first release with this critical section redesigned.
         * Under certain circumstances, the PREVIOUS critical section algorithm
         * used around this flash point allowed interrupts to execute
         * which violated the design assumptions.  The critical section
         * mechanism used here WAS redesigned to address this */

        /* allow interrupts here */
        _ISR_Flash(level);

        /* if an interrupt occurred which changed the state of the watchdog
         * Note: the watchdog could have changed inside an interrupt, so
         * the state needs to be re-read */
        if(the_watchdog->state != WATCHDOG_BEING_INSERTED)
        {
            /* goto exit */
            goto exit_insert;
        }

        /* while inserting, a new interrupt can occur, which can
         * oblige a new restart of the algorithm */

        /* check if an interrupt occur in the flash instruction */

        /* if an interrupt occur in the flash instruction */
        if(_Watchdog_Sync_level > insert_isr_nest_level)
        {
            /* update watchdog level with isr nest level */
            _Watchdog_Sync_level = insert_isr_nest_level;

            /* then have to restart the algorithm */
            goto restart;
        }
    }

    /* watchdog has been inserted */

    /* activate the watchdog */
    _Watchdog_Activate(the_watchdog);

    /* set the watchdog delta interval */
    the_watchdog->delta_interval = delta_interval;

    /* insert the watchdog on the chain */
    _Chain_Insert_unprotected(after->Node.previous , &the_watchdog->Node);

    /* set the watchdog start time */
    the_watchdog->start_time = _Watchdog_Ticks_since_boot;

    /* exit */
exit_insert:

    /* set the sycn level */
    _Watchdog_Sync_level = insert_isr_nest_level;

    /* decrease the sync count */
    _Watchdog_Sync_count--;

    /* leave critical section */
    _ISR_Enable(level);
}

/**  
 *  @}
 */

/**
 *  @}
 */
