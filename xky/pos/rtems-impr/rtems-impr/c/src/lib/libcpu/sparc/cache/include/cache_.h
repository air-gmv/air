/**
 *  @file
 *  cache_.h
 *
 *  @brief Cache Management Support Routines for the SPARC
 *
 *  COPYRIGHT (c) 2010.
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  8183    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SPARC_CPUS SPARC CPUs
 *  @{
 */

#ifndef __SPARC_CACHE_h
#define __SPARC_CACHE_h

#include <rtems/score/cpu.h>
#include <bsp.h>

#if (HAS_INSTRUCTION_CACHE == TRUE)

#define CPU_INSTRUCTION_CACHE_ALIGNMENT 0

/**
 *  @brief invalidate the generic CPU instruction cache
 */
void _CPU_cache_invalidate_entire_instruction(void);

#endif

#include <libcpu/cache.h>

#endif
/* end of include file */

/**
 *  @}
 */

