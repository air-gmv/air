/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file arm.h
 * @author lumm
 * @brief Arm macros and typedef definitions.
 */

#ifndef ARM_ARMv7_H
#define ARM_ARMv7_H

#ifdef ASM

/**
 * @brief Program Status Register.
 * Each program mode has its own Saved PSR (SPSR). usr and sys shared theirs.
 */
#define PSR_N (1 << 31)             /* Negative condition flag */
#define PSR_Z (1 << 30)             /* Zero condition flag */
#define PSR_C (1 << 29)             /* Carry condition flag */
#define PSR_V (1 << 28)             /* Overflow condition flag */
#define PSR_Q (1 << 27)             /* Cumulative saturation */
#define PSR_J (1 << 24)             /* Jazelle or ThumbEE (T also 1) execution state */
#define PSR_GE_MASK (0xf << 16)     /* Greater than or Equals condition flags */
#define PSR_E (1 << 9)              /* Endianness bit (0-low, 1-big) */
#define PSR_A (1 << 8)              /* Asynchronous abort */
#define PSR_I (1 << 7)              /* IRQ mask */
#define PSR_F (1 << 6)              /* FIQ mask */
#define PSR_T (1 << 5)              /* Thumb execution state */
#define PSR_MODE_MASK (0x1f << 0)
#define PSR_USR 0x10
#define PSR_FIQ 0x11
#define PSR_IRQ 0x12
#define PSR_SVC 0x13
#define PSR_MON 0x16
#define PSR_ABT 0x17
#define PSR_HYP 0x1a
#define PSR_UND 0x1b
#define PSR_SYS 0x1f


/* @brief If the bit 0 of the bx reg is 0 it will remain in ARM, if 1 it will
 * change to Thumb. The .thumb directive is for the assembler to compile the
 * following instructions as Thumb. Same way for .arm.
 */
.macro SWITCH_FROM_ARM_TO_THUMB r
#if defined(__thumb__)
    add     \r, pc, #1
    bx      \r
.thumb
#endif
.endm

.macro SWITCH_FROM_THUMB_TO_ARM
#if defined(__thumb__)
.align 2
    bx      pc
.arm
#endif
.endm

#endif /* ASM */

#ifndef ASM
#if defined(__thumb__)
    #define ARM_SWITCH_REGISTERS air_u32_t arm_switch_reg
    #define ARM_SWITCH_BACK "add %[arm_switch_reg], pc, #1\nbx %[arm_switch_reg]\n.thumb\n"
    #define ARM_SWITCH_TO_ARM ".align 2\nbx pc\n.arm\n"
    #define ARM_SWITCH_OUTPUT [arm_switch_reg] "=&r" (arm_switch_reg)
    #define ARM_SWITCH_ADDITIONAL_OUTPUT , ARM_SWITCH_OUTPUT
#else
    #define ARM_SWITCH_REGISTERS
    #define ARM_SWITCH_BACK
    #define ARM_SWITCH_TO_ARM
    #define ARM_SWITCH_OUTPUT
    #define ARM_SWITCH_ADDITIONAL_OUTPUT
#endif /* defined(__thumb__) */

inline static void instruction_synchronization_barrier(void) {
    __isb(15);
}

inline static void data_synchronization_barrier(air_u32_t intrinsic) {
    __dsb(intrinsic);
}

inline static void data_memory_barrier(air_u32_t intrinsic) {
    __dmb(intrinsic);
}

#endif /* ASM */

#endif /* ARM_ARMv7_H */
