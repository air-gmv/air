/**
 *  @file
 *  no-timer.c
 *
 *  @brief initialize the Timer Manager in case it has not been selected by the
 *  application.
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
 *  4479    | 23/09/2009  | mcoutinho    | IPR 559
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8309    | 21/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9877 $ | $Date: 2011-03-18 18:39:36 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup OPTIONAL_MANAGERS Optional Managers
 *  @{
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>
#include <rtems/score/interr.h>

#include <rtems/rtems/types.h>
#include <rtems/rtems/timer.h>

/*
 * Define here these global variables because there is a dependency of the
 * TOD Handler with them. They are also declared in the rtemstimer.c file.
 */

Thread_Control *_Timer_Server = NULL;


Chain_Control _Timer_Ticks_chain;


Chain_Control _Timer_Seconds_chain;


void _Timer_Manager_initialization(uint32_t maximum_timers)
{
   /* does nothing */
}


/**  
 *  @}
 */
