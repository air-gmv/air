/**
 *  @file
 *  timercreate.c
 *
 *  @brief create a timer
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
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
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


rtems_status_code rtems_timer_create(
                                     rtems_name name ,
                                     Objects_Id *id
                                     )
{
    /* timer to create */
    Timer_Control *the_timer;


    /* check if the timer name is valid */
    if(!rtems_is_name_valid(name))
    {
        /* return invalid name error */
        return RTEMS_INVALID_NAME;
    }

    /* check if the id address is valid */
    if(!id)
    {
        /* return invalid address error */
        return RTEMS_INVALID_ADDRESS;
    }

    /* to prevent deletion */
    _Thread_Disable_dispatch();

    /* allocate the timer memory */
    the_timer = _Timer_Allocate();

    /* if could not allocate */
    if(!the_timer)
    {
        /* re-enable thread dispatch */
        _Thread_Enable_dispatch();

        /* and return too many error */
        return RTEMS_TOO_MANY;
    }

    /* initialize the timer class to dormant */
    the_timer->the_class = TIMER_DORMANT;

    /* initialize the timer watchdog information */
    _Watchdog_Initialize(&the_timer->Ticker , NULL , 0 , NULL);

    /* initialize the timer object information */
    _Objects_Open(&_Timer_Information ,
                  &the_timer->Object ,
                  (Objects_Name) name);

    /* set the out id */
    *id = the_timer->Object.id;

    /* re-enable thread dispatch */
    _Thread_Enable_dispatch();

    /* return success */
    return RTEMS_SUCCESSFUL;
}

/**  
 *  @}
 */

/**
 *  @}
 */
