/**
 *  @file
 *  cache.h
 *
 *  @brief libcpu Cache Manager Support 
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
 *  The functions declared in this file are implemented for
 *  each processor in the cache.c file under libcpu/CPU/.
 *  They provide the processor specific actions to take for
 *  implementing most of the RTEMS Cache Manager directives,
 *  and should only ever be called by these directives.
 *
 *  The API for the RTEMS Cache Manager can be found in
 *  c/src/exec/rtems/include/rtems/rtems/cache.h
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  4414    | 21/09/2009  | mcoutinho    | IPR 614
 *  6116    | 18/02/2010  | mcoutinho    | IPR 1818
 *  8183    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9877 $ | $Date: 2011-03-18 18:39:36 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SHARED_BETWEEN_CPUS Shared between CPUs
 *  @{
 */

#ifndef __LIBCPU_CACHE_h
#define __LIBCPU_CACHE_h

#include <rtems/score/types.h>
#include <rtems/score/cpu.h>
#include <bsp.h>

#if (HAS_INSTRUCTION_CACHE == TRUE)

/**
 *  @brief invalidate the entire cache instruction set
 *
 *  This function is responsible for performing an instruction cache
 *  invalidate. It invalidates the entire instruction cache.
 */
void rtems_cache_invalidate_entire_instruction(void);

#endif


#endif

/* end of include file */

/**  
 *  @}
 */
