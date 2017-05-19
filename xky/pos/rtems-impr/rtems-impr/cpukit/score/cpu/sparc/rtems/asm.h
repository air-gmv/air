/**
 *  @file
 *  asm.h
 *
 *  @brief Attempt to address the problems
 *  caused by incompatible flavors of assemblers and
 *  toolsets.  It primarily addresses variations in the
 *  use of leading underscores on symbols and the requirement
 *  that register names be preceded by a %.
 *
 *  NOTE: The spacing in the use of these macros
 *        is critical to them working as advertised.
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
 *  This file is based on similar code found in newlib available
 *  from ftp.cygnus.com.  The file which was used had no copyright
 *  notice.  This file is freely distributable as long as the source
 *  of the file is noted.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  7148    | 09/04/2010  | mcoutinho    | IPR 1942
 *  8205    | 16/06/2010  | mcoutinho    | IPR 451
 *  9872    | 18/03/2011  | aconstantino | SPR 2819
 *  $Rev: 9929 $ | $Date: 2011-03-23 12:02:26 +0000 (Wed, 23 Mar 2011) $| $Author: sfaustino $ | SPR 2819
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

#ifndef _RTEMS_ASM_H
#define _RTEMS_ASM_H

/**
 *  @brief Indicate we are in an assembly file and get the basic CPU definitions.
 */
#ifndef ASM
#define ASM
#endif

#include <rtems/score/cpu.h>

/*
 *  Recent versions of GNU cpp define variables which indicate the
 *  need for underscores and percents.  If not using GNU cpp or
 *  the version does not support this, then you will obviously
 *  have to define these as appropriate.
 */

/* XXX __USER_LABEL_PREFIX__ and __REGISTER_PREFIX__ do not work on gcc 2.7.0 */
/* XXX The following ifdef magic fixes the problem but results in a warning   */
/* XXX when compiling assembly code.                                          */

#ifndef __USER_LABEL_PREFIX__
#define __USER_LABEL_PREFIX__ _
#endif

#ifndef __REGISTER_PREFIX__
#define __REGISTER_PREFIX__
#endif

#include <rtems/concat.h>

/**
 *  @brief use the right prefix for global labels.
 */
#define SYM(x) CONCAT1 (__USER_LABEL_PREFIX__, x)

/**
 *  @brief use the right prefix for registers.
 */
#define REG(x) CONCAT1 (__REGISTER_PREFIX__, x)

/*
 *  define macros for all of the registers on this CPU
 *
 *  EXAMPLE:     #define d0 REG (d0)
 */

/*
 *  Define macros to handle section beginning and ends.
 */

/**
 *  @brief begin code declaration
 **/
#define BEGIN_CODE_DCL .text

/**
 *  @brief end code declaration
 **/
#define END_CODE_DCL

/**
 *  @brief begin data declaration
 **/
#define BEGIN_DATA_DCL .data

/**
 *  @brief end data declaration
 **/
#define END_DATA_DCL

/**
 *  @brief begin code
 **/
#define BEGIN_CODE .text

/**
 *  @brief end code
 **/
#define END_CODE

/**
 *  @brief begin data
 **/
#define BEGIN_DATA

/**
 *  @brief end data
 **/
#define END_DATA

/**
 *  @brief begin bss
 **/
#define BEGIN_BSS

/**
 *  @brief end bss
 **/
#define END_BSS

/**
 *  @brief end declaration
 **/
#define END

/*
 *  Following must be tailor for a particular flavor of the C compiler.
 *  They may need to put underscores in front of the symbols.
 */


/**
 *  @brief public symbol (is global symbol)
 **/
#define PUBLIC(sym) .globl SYM (sym)


/**
 *  @brief extern symbol (is a global symbol)
 **/
#define EXTERN(sym) .globl SYM (sym)


/**
 *  @brief Entry for traps which jump to a programmer-specified trap handler.
 */
#define TRAP(_vector, _handler)  \
  mov   %psr, %l0 ; \
  sethi %hi(_handler), %l4 ; \
  jmp   %l4+%lo(_handler); \
  mov   _vector, %l3


/**
 *  @brief reset trap that avoids supervisor instructions
 */
#define RTRAP(_vector, _handler)  \
  mov   %g0, %l0 ; \
  sethi %hi(_handler), %l4 ; \
  jmp   %l4+%lo(_handler); \
  mov   _vector, %l3;


/**
 * @brief memory requirements for the remote debugger
 */
#define RDB_MON_MEM 32 * 1024 /* 32 KiB */


/**
 * @brief initial stack size (in bytes)
 */
#define STACK_SIZE 16 * 1024

#endif

/**
 *  @}
 */

/**
 *  @}
 */

