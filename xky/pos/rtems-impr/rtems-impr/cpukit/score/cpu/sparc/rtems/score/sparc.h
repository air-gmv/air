/**
 *  @file
 *  sparc.h
 *
 *  @brief information pertaining to the SPARC processor family.
 *
 *  This file contains the information required to build
 *  RTEMS for a particular member of the "sparc" family.  It does
 *  this by setting variables to indicate which implementation
 *  dependent features are present in a particular member
 *  of the family.
 *
 *  Currently recognized feature flags:
 *
 *    + SPARC_HAS_FPU
 *        0 - no HW FPU
 *        1 - has HW FPU (assumed to be compatible w/90C602)
 *
 *    + SPARC_HAS_BITSCAN
 *        0 - does not have scan instructions
 *        1 - has scan instruction  (not currently implemented)
 *
 *    + SPARC_NUMBER_OF_REGISTER_WINDOWS
 *        8 is the most common number supported by SPARC implementations.
 *        SPARC_PSR_CWP_MASK is derived from this value.
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
 *  1905    | 16/03/2009  | mcoutinho    | IPR 60
 *  4691    | 07/10/2009  | mcoutinho    | IPR 462
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @defgroup SUPER_CORE_SPARC_CPU_SUPPORT Super Core SPARC CPU Support
 *
 *  @brief The Super Core SPARC CPU Support component contains the code specific to
 *  manage the SPARC CPU, like performing context switch, assembly code for
 *  multiplications, divisions, manage interruptions, etc.
 *  @{
 */

#ifndef _RTEMS_SCORE_SPARC_H
#define _RTEMS_SCORE_SPARC_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief XKY - SPARC virtualization system calls
 */
#include <xky.h>

   /**
    *  @brief determine if SPARC has bitscan instructions
    *
    *  Some higher end SPARCs have a bitscan instructions. It would
    *  be nice to take advantage of them.  Right now, there is no
    *  port to a CPU model with this feature and no (untested) code
    *  that is based on this feature flag.
    */
#define SPARC_HAS_BITSCAN                0

   /**
    *  @brief determine the number of register windows for SPARC
    *
    *  This should be OK until a port to a higher end SPARC processor
    *  is made that has more than 8 register windows.  If this cannot
    *  be determined based on multilib settings (v7/v8/v9), then the
    *  cpu_asm.S code that depends on this will have to move to libcpu.
    */
#define SPARC_NUMBER_OF_REGISTER_WINDOWS 8

   /**
    *  @brief determine if SPARC has hardware FPU
    *
    *  This should be determined based on some soft float derived
    *  cpp predefine but gcc does not currently give us that information.
    */
#define SPARC_HAS_FPU 1

#if SPARC_HAS_FPU
   /**
    *  @brief determine the CPU model name
    **/
#define CPU_MODEL_NAME "w/FPU"
#else
   /**
    *  @brief determine the CPU model name
    **/
#define CPU_MODEL_NAME "w/soft-float"
#endif

   /**
    *  @brief determine the name of the CPU family (SPARC)
    */
#define CPU_NAME "SPARC"

   /*
    *  Miscellaneous constants
    */

   /**
    *  PSR masks and starting bit positions
    *
    *  NOTE: Reserved bits are ignored.
    */
#if (SPARC_NUMBER_OF_REGISTER_WINDOWS == 8)
#define SPARC_PSR_CWP_MASK               0x07   /* bits  0 -  4 */
#elif (SPARC_NUMBER_OF_REGISTER_WINDOWS == 16)
#define SPARC_PSR_CWP_MASK               0x0F   /* bits  0 -  4 */
#elif (SPARC_NUMBER_OF_REGISTER_WINDOWS == 32)
#define SPARC_PSR_CWP_MASK               0x1F   /* bits  0 -  4 */
#else
#error "Unsupported number of register windows for this cpu"
#endif

#define SPARC_PSR_ET_MASK   0x00000020   /* bit   5 */
#define SPARC_PSR_PS_MASK   0x00000040   /* bit   6 */
#define SPARC_PSR_S_MASK    0x00000080   /* bit   7 */
#define SPARC_PSR_PIL_MASK  0x00000F00   /* bits  8 - 11 */
#define SPARC_PSR_EF_MASK   0x00001000   /* bit  12 */
#define SPARC_PSR_EC_MASK   0x00002000   /* bit  13 */
#define SPARC_PSR_ICC_MASK  0x00F00000   /* bits 20 - 23 */
#define SPARC_PSR_VER_MASK  0x0F000000   /* bits 24 - 27 */
#define SPARC_PSR_IMPL_MASK 0xF0000000   /* bits 28 - 31 */

#define SPARC_PSR_CWP_BIT_POSITION   0   /* bits  0 -  4 */
#define SPARC_PSR_ET_BIT_POSITION    5   /* bit   5 */
#define SPARC_PSR_PS_BIT_POSITION    6   /* bit   6 */
#define SPARC_PSR_S_BIT_POSITION     7   /* bit   7 */
#define SPARC_PSR_PIL_BIT_POSITION   8   /* bits  8 - 11 */
#define SPARC_PSR_EF_BIT_POSITION   12   /* bit  12 */
#define SPARC_PSR_EC_BIT_POSITION   13   /* bit  13 */
#define SPARC_PSR_ICC_BIT_POSITION  20   /* bits 20 - 23 */
#define SPARC_PSR_VER_BIT_POSITION  24   /* bits 24 - 27 */
#define SPARC_PSR_IMPL_BIT_POSITION 28   /* bits 28 - 31 */

#ifndef ASM

   /**
    *  @brief get the PSR : replaced in GMV PMK by system call
    */
/*#define sparc_get_psr( _psr ) \
  do { \
     (_psr) = 0; \
     asm volatile( "rd %%psr, %0" :  "=r" (_psr) : "0" (_psr) ); \
  } while ( 0 )*/

   /**
    *  @brief get the TBR : replaced in GMV PMK by system call
    */
/*#define sparc_get_tbr( _tbr ) \
  do { \
     (_tbr) = 0; \
     asm volatile( "rd %%tbr, %0" :  "=r" (_tbr) : "0" (_tbr) ); \
  } while ( 0 )*/

   /**
    *  @brief disable all interrupts
    *
    *  @return returns the former SPAR PIL register value
    *  @note GMV - removed for the system call
    */
   /* uint32_t sparc_disable_interrupts(void); */

   /**
    *  @brief enable interrupts to a specified level
    *
    *  @param[in] pil the PIL level to restore the CPU context to
    *  @note GMV - removed for the system call
    */
    /* void sparc_enable_interrupts(uint32_t pil);*/

   /**
    *  @brief get interrupt level
    *
    *  @return The PIL level to restore the CPU context to
    */
	uint32_t pmk_sparc_get_int_level(void);
   /**
    *  @brief get trap base register (tbr) contents
    *
    *  @return the psr register contents
    */
	uint32_t pmk_sparc_get_tbr(void);
   
   /**
    *  @brief flash interrupts (enable and disable the interrupts)
    **/
#define sparc_flash_interrupts( _level ) \
  do { \
    register uint32_t   _ignored = 0; \
    \
    xky_sparc_enable_interrupts( (_level) ); \
    _ignored = xky_sparc_disable_interrupts(); \
  } while ( 0 )

   /**
    *  @brief get the interrupt level
    **/
#define sparc_get_interrupt_level( _level ) \
  do { \
    register uint32_t   _psr_level = 0; \
    \
    _psr_level = xky_sparc_get_psr(); \
    (_level) = \
      (_psr_level & SPARC_PSR_PIL_MASK) >> SPARC_PSR_PIL_BIT_POSITION; \
  } while ( 0 )

#endif

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_SPARC_H */

/**
 *  @}
 */

/**
 *  @}
 */
