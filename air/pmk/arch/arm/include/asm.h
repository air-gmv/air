/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file asm.h
 * \author lumm
 * \brief Assembly macros to improve readability.
 *
 * The .thumb directive is for the assembler to compile the
 * following instructions as Thumb. Same way for .arm.
 */

#ifndef ASM_H_
#define ASM_H_

#ifdef ASM

#define global(label) \
    .globl label; \
    label:

/**
 * \brief ARM assembly jump to C macro
 *
 * If the bit 0 of the bx reg is 0 it will remain in ARM, if 1 it will
 * change to Thumb.
 */
.macro BL2C label
#if defined(__thumb__)
    blx     \label
#else
    bl      \label
#endif
.endm

/**
 * \brief ARM assembly is trashed
 */
.macro GET_CTX r, per_cpu
    mrc     p15, 0, \per_cpu, c13, c0, 4
    ldr     \r, [\per_cpu, #offsetof_pmk_core_ctrl_t_context]
.endm

/**
 * \brief ARM assembly is trashed
 */
.macro IS_TRASHED r, ctx
    ldr     \r, [ctx, #offsetof_arm_core_context_t_trash]
    cmp     \r, #1
.endm

#if PMK_FPU_SUPPORT
    #if defined(__ARM_NEON)
        #define VFP_D32
    #elif defined(__ARM_FP)
        #define VFP_D16
    #else
        #error "FPU support not supported"
    #endif
#endif

#endif /* ASM */
#endif /* ASM_H_ */
