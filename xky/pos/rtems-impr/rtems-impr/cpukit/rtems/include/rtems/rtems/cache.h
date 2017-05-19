/**
 *  @file
 *  cache.h
 *
 *  @brief definition of the API to the RTEMS Cache, which are divided into data
 *  cache and instruction cache functions.
 *
 *  Data cache functions are only meaningful if a data cache is supported.
 *  Instruction cache functions are only meaningful if an instruction cache is
 *  supported.
 *
 *  The functions below are implemented with CPU dependent support routines
 *  implemented as part of libcpu. In the event that a CPU does not support a
 *  specific function, the CPU dependent routine does nothing (but does exist).
 *
 *  At this point, the RTEMS Cache makes no considerations, and provides no
 *  support for BSP specific issues such as a secondary cache. In such a system,
 *  the CPU dependent routines would have to be modified, or a BSP layer added
 *  to this Manager.
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
 *  3971    | 23/07/2009  | mcoutinho    | IPR 613
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6116    | 18/02/2010  | mcoutinho    | IPR 1818
 *  8321    | 21/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup RTEMS_API RTEMS API
 *  @{
 */

/**
 *  @addtogroup RTEMS_API_COMMON Common
 *  @{
 */

#ifndef _RTEMS_RTEMS_CACHE_H
#define _RTEMS_RTEMS_CACHE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/system.h>
#include <rtems/score/types.h>

   /* These functions will ONLY do something if the
    * libcpu support includes data cache routines AND
    * the CPU model supports data caching */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

/**  
 *  @}
 */

/**
 *  @}
 */
