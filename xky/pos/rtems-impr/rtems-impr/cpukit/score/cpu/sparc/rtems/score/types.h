/**
 *  @file
 *  types.h
 *
 *  @brief type definitions pertaining to the SPARC processor family.
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
 *  6116    | 18/02/2010  | mcoutinho    | IPR 1818
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup SUPER_CORE_SPARC_CPU_SUPPORT Super Core SPARC CPU Support
 *  @{
 */

#ifndef _RTEMS_SCORE_TYPES_H
#define _RTEMS_SCORE_TYPES_H

#ifndef ASM

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

   /**
    *  @brief sparc ISR return type
    **/
   typedef void sparc_isr;

   /**
    *  @brief sparc isr entry handler
    **/
   typedef void ( *sparc_isr_entry )(void);

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* _RTEMS_SCORE_TYPES_H */

/**
 *  @}
 */

/**
 *  @}
 */
