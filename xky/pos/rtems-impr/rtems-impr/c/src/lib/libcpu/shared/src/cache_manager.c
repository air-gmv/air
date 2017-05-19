/**
 *  @file
 *  cache_manager.c
 *
 *  @brief Cache manager
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
 *  3974    | 23/07/2009  | mcoutinho    | IPR 613
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6116    | 18/02/2010  | mcoutinho    | IPR 1818
 *  8183    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SHARED_BETWEEN_CPUS Shared between CPUs
 *  @{
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/score/types.h>
#include <libcpu/cache.h>
#include <libcpu/cache_.h>

/* if the CPU has instruction cache */
#if (HAS_INSTRUCTION_CACHE == TRUE)


void rtems_cache_invalidate_entire_instruction(void)
{
    /* Call the CPU-specific routine */
    _CPU_cache_invalidate_entire_instruction();
}

#endif
/**
 *  @}
 */
