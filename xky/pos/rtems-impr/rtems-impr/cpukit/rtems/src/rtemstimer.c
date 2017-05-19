/**
 *  @file
 *  rtemstimer.c
 *
 *  @brief initialize the Timer Manager
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
 *  4480    | 23/09/2009  | mcoutinho    | IPR 559
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  7099    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8311    | 21/06/2010  | mcoutinho    | IPR 451
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

/*
 * Define here these global variables because there is a dependency of the
 * TOD Handler with them. They are also declared in the no-timer.c file.
 */


Objects_Information _Timer_Information;


Thread_Control *_Timer_Server = NULL;


Chain_Control _Timer_Ticks_chain;


Chain_Control _Timer_Seconds_chain;


void _Timer_Manager_initialization(
                                   uint32_t maximum_timers
                                   )
{
    /* initialize the information for the timer manager */
    _Objects_Initialize_information(&_Timer_Information , /* object information table */
                                    OBJECTS_CLASSIC_API , /* object API */
                                    OBJECTS_RTEMS_TIMERS , /* object class */
                                    maximum_timers , /* maximum objects of this class */
                                    sizeof ( Timer_Control ) , /* size of this object's control block */
                                    FALSE , /* TRUE if the name is a string */
                                    RTEMS_MAXIMUM_NAME_LENGTH /* maximum length of an object name */
#if defined(RTEMS_MULTIPROCESSING)
        ,
                                    FALSE , /* TRUE if this is a global object class */
                                    NULL /* Proxy extraction support callout */
#endif
        );

    /* Initialize the pointer to the Timer Server TCB to NULL indicating
     * that task-based timer support is not initialized */
    _Timer_Server = NULL;
}

/**  
 *  @}
 */

/**
 *  @}
 */
