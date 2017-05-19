/**
 *  @file
 *  sptables.h
 *
 *  @brief executive's pre-initialized tables used when in a single processor
 *  configuration.
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
 *  297     | 15/10/2008  | mcoutinho    | IPR 69
 *  593     | 17/11/2008  | mcoutinho    | IPR 64
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_API Super API
 *  @{
 */

/**
 *  @addtogroup SUPER_API_CONFIG Configuration
 *  @{
 */

#ifndef _RTEMS_SPTABLES_H
#define _RTEMS_SPTABLES_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/config.h>
#include <rtems/fatal.h>
#include <rtems/init.h>
#include <rtems/io.h>
#include <rtems/score/sysstate.h>
#include <rtems/rtems/intr.h>
#include <rtems/rtems/clock.h>
#include <rtems/rtems/tasks.h>
#include <rtems/rtems/event.h>
#include <rtems/rtems/message.h>
#include <rtems/rtems/ratemon.h>
#include <rtems/rtems/sem.h>
#include <rtems/rtems/timer.h>

#ifdef RTEMS_MULTIPROCESSING
   /**
    *  @brief Multiprocessing Configuration Table
    */
   extern const rtems_multiprocessing_table _Initialization_Default_multiprocessing_table;
#endif

#ifdef __cplusplus
}
#endif

#endif

/**  
 *  @}
 */

/**  
 *  @}
 */

