/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file asm.h
 * @author lumm
 * @brief Assembly macros to improve readability.
 */

#ifndef ARM_ASM_H
#define ARM_ASM_H

#ifdef ASM

/** @brief Macro for symbol definitions. */
#define SYM(x)      x

/** @brief Macro for register definitions. */
#define REG(x)      x

/** @brief Macro for global symbol definitions. */
#define GLOBAL(x)   .align 0x4; .globl SYM(x)

/** @brief Define macros for all the registers. */
#define r0      REG(r0)
#define r1      REG(r1)
#define r2      REG(r2)
#define r3      REG(r3)
#define r4      REG(r4)
#define r5      REG(r5)
#define r6      REG(r6)
#define r7      REG(r7)
#define r8      REG(r8)
#define r9      REG(r9)
#define r10     REG(r10)
#define r11     REG(r11)
#define r12     REG(r12)
#define sp      REG(r13)
#define lr      REG(r14)
#define pc      REG(r15)

#define cpsr    REG(CPSR)
#define spsr    REG(SPSR)

#define FUNC(label) \
    .align 4; \
    .globl label; \
    label:

#endif /* ASM */

#endif /* ARM_ASM_H */
