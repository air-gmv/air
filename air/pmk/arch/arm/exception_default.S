/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file exceptions.S
 * @author lumm
 * @brief Exception handlers
 */

/* Are we in assembly file */
#ifdef ASM

#include <asm.h>
#include <armv7.h>

    .extern exception_default_handler

/* 13 global registers */
#define ARM_EXCEPTION_FRAME_REGISTER_SP_OFFSET (13 * 4) //=52
/* 40 - 13 = 7 registers containing pre-exception registers */
#define MORE_CONTEXT_SIZE \
    (ARM_EXCEPTION_FRAME_SIZE - ARM_EXCEPTION_FRAME_REGISTER_SP_OFFSET)

/* Save global registers and branch to save context */
FUNC(exception_undef)
    sub     sp, #MORE_CONTEXT_SIZE
    stmdb   sp!, {r0-r12}
    mov     r4, #1
    b       save_context

FUNC(exception_pref_abort)
    sub     sp, #MORE_CONTEXT_SIZE
    stmdb   sp!, {r0-r12}
    mov     r4, #3
    b       save_context

FUNC(exception_data_abort)
    sub     sp, #MORE_CONTEXT_SIZE
    stmdb   sp!, {r0-r12}
    mov     r4, #4
    b       save_context

FUNC(exception_fiq_interrupt)
    sub     sp, #MORE_CONTEXT_SIZE
    stmdb   sp!, {r0-r12}
    mov     r4, #7

/*
 * Don't enable FIQs yet. Set the FIQ disable bit in the SPSR
 * (which we'll load into the CPSR in save_context).
 */
    mrs     r2, spsr
    orr     r2, #ARM_PSR_F
    msr     spsr_c, r2

save_context:
    /* Save more context */
    mov     r2, lr
    mrs     r3, spsr
    mrs     r7, cpsr
    orr     r5, r3, #ARM_PSR_I
    bic     r5, #ARM_PSR_T
    msr     cpsr, r5
    mov     r0, sp
    mov     r1, lr
    msr     cpsr, r7
    mov     r5, #0
    add     r6, sp, #ARM_EXCEPTION_FRAME_REGISTER_SP_OFFSET
    stm     r6, {r0-r5}

    /* Argument for high level handler */
    mov     r0, sp

    /* Clear VFP context pointer */
    add     r3, sp, #ARM_EXCEPTION_FRAME_VFP_CONTEXT_OFFSET
    mov     r1, #0
    str     r1, [r3]

#ifdef PMK_FPU_SUPPORT
    /* Ensure that the FPU is enabled */
    vmrs    r1, FPEXC
    tst     r1, #(1 << 30)
    beq     1f

    /* Save VFP context */
    sub     sp, #(ARM_VFP_CONTEXT_SIZE + 4)
    add     r4, sp, #4
    bic     r4, r4, #7
    str     r4, [r3]
    vmrs    r2, FPSCR
    stmia   r4!, {r1-r2}
    vstmia  r4!, {d0-d15}
#ifdef VFP_D32
    vstmia  r4!, {d16-d31}
#else
    mov     r1, #0
    mov     r2, #0
    adds    r3, r4, #128
2:
    stmia   r4!, {r1-r2}
    cmp     r4, r3
    bne     2b
#endif
1:
#endif /* PMK_FPU_SUPPORT */

    /* Call high level handler */
    SWITCH_FROM_ARM_TO_THUMB    r1
    bl      exception_default_handler

#endif /* ASM */
