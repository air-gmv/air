/**
 *  @file
 *  no-rtmon.c
 *
 *  @brief initialize the Rate Monotonic Manager in case it has not been
 *  selected by the application.
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
 *  8183    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9877 $ | $Date: 2011-03-18 18:39:36 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup OPTIONAL_MANAGERS Optional Managers
 *  @{
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/interr.h>

#include <rtems/rtems/types.h>
#include <rtems/rtems/ratemon.h>


void _Rate_monotonic_Manager_initialization(uint32_t maximum_periods)
{
   /* does nothing */
}

/**  
 *  @}
 */
