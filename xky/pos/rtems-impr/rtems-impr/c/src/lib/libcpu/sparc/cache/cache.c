/**
 *  @file
 *  cache.c
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
 *  4414    | 21/09/2009  | mcoutinho    | IPR 614
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8183    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SPARC_CPUS SPARC CPUs
 *  @{
 */

#include <rtems.h>
#include <libcpu/cache_.h>
#include <bsp.h>


#if (HAS_INSTRUCTION_CACHE == TRUE)


void _CPU_cache_invalidate_entire_instruction(void)
{
    /* flush the instruction cache */
    asm volatile ("flush" );
}

#endif

/* end of file */

/**  
 *  @}
 */
