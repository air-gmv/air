/**
 *  @file
 *  timerident.c
 *
 *  @brief identify a timer based on its name
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


rtems_status_code rtems_timer_ident(
                                    rtems_name name ,
                                    Objects_Id *id
                                    )
{
    /* status of the object name to id */
    Objects_Name_or_id_lookup_errors status;

    
    /* get object id from the:
     *   timer object table
     *   with the specified name
     *   if multiprocessing, then with the node
     *   and place the result in the pointed by id */
    status = _Objects_Name_to_id(&_Timer_Information ,
                                 (Objects_Name) name ,
#if defined(RTEMS_MULTIPROCESSING)
        OBJECTS_SEARCH_LOCAL_NODE ,
#endif
        id);

    /* return the translated error */
    return _Status_Object_name_errors_to_status[ status ];
}

/**  
 *  @}
 */

/**
 *  @}
 */
