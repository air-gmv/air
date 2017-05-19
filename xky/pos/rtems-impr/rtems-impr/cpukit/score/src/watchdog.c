/**
 *  @file
 *  watchdog.c
 *
 *  @brief initialize the watchdog handler
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
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
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


volatile uint32_t _Watchdog_Sync_level;


volatile uint32_t _Watchdog_Sync_count;


volatile Watchdog_Interval _Watchdog_Ticks_since_boot;


Watchdog_Nanoseconds_since_last_tick_routine _Watchdog_Nanoseconds_since_tick_handler;


Chain_Control _Watchdog_Ticks_chain;


Chain_Control _Watchdog_Seconds_chain;


void _Watchdog_Handler_initialization(void)
{
    /* initialize the sync count */
    _Watchdog_Sync_count = 0;

    /* initialize the sync level */
    _Watchdog_Sync_level = 0;

    /* initialize the number of ticks since boot */
    _Watchdog_Ticks_since_boot = 0;

    /* initialize the watchdog clock ticks chain */
    _Chain_Initialize_empty(&_Watchdog_Ticks_chain);

    /* initialize the watchdog seconds chain */
    _Chain_Initialize_empty(&_Watchdog_Seconds_chain);
}

/**  
 *  @}
 */

/**
 *  @}
 */
