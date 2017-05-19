/**
 *  @file
 *  ratemon.c
 *
 *  @brief initialize the Rate Monotonic Manager
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
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  7091    | 09/04/2010  | mcoutinho    | IPR 1931
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


Objects_Information _Rate_monotonic_Information;


void _Rate_monotonic_Manager_initialization(
                                            uint32_t maximum_periods
                                            )
{
    /* initialize the information for the rate monotonic manager */
    _Objects_Initialize_information(&_Rate_monotonic_Information , /* object information table */
                                    OBJECTS_CLASSIC_API , /* object API */
                                    OBJECTS_RTEMS_PERIODS , /* object class */
                                    maximum_periods , /* maximum objects of this class */
                                    sizeof ( Rate_monotonic_Control ) , /* size of this object's control block */
                                    FALSE , /* TRUE if the name is a string */
                                    RTEMS_MAXIMUM_NAME_LENGTH /* maximum length of an object name */
#if defined(RTEMS_MULTIPROCESSING)
        ,
                                    FALSE , /* TRUE if this is a global object class */
                                    NULL /* Proxy extraction support callout */
#endif
        );
}

/**  
 *  @}
 */

/**
 *  @}
 */
