/**
 *  @file
 *  ratemoncreate.c
 *
 *  @brief create a rate monotonic object
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
 *  575     | 17/11/2008  | mcoutinho    | IPR 70
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  7093    | 09/04/2010  | mcoutinho    | IPR 1931
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


rtems_status_code rtems_rate_monotonic_create(
                                              rtems_name name ,
                                              Objects_Id *id
                                              )
{
    /* period to be created */
    Rate_monotonic_Control *the_period;


    /*  check if name is valid */
    if(!rtems_is_name_valid(name))
    {
        /* return invalid name */
        return RTEMS_INVALID_NAME;
    }

    /* check if id address is valid */
    if(!id)
    {
        /* return invalid address */
        return RTEMS_INVALID_ADDRESS;
    }

    /* disable dispatch to prevent deletion */
    _Thread_Disable_dispatch();

    /* allocate the period */
    the_period = _Rate_monotonic_Allocate();

    /* check if the period was allocated */
    if(!the_period)
    {
        /* enable the dispatch (undo previous step) */
        _Thread_Enable_dispatch();

        /* too many periods were created */
        return RTEMS_TOO_MANY;
    }

    /* set the period owner (only one onwer and must the calling thread) */
    the_period->owner = _Thread_Executing;

    /* set the initial period state to inactive */
    the_period->state = RATE_MONOTONIC_INACTIVE;

    /* initialize the watchdog */
    _Watchdog_Initialize(&the_period->Timer , NULL , 0 , NULL);

    /* initialize the rate monotonic object information */
    _Objects_Open(&_Rate_monotonic_Information ,
                  &the_period->Object ,
                  (Objects_Name) name);

    /* set the output identifier for the user */
    *id = the_period->Object.id;

    /* enable the dispatch */
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
