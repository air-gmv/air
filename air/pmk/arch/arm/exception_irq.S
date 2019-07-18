/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file exception_fiq.S
 * @author lumm
 * @brief FIQ exception handler
 */

/* Are we in assembly file */
#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <armv7.h>
#include <asm_offsets.h>

    .extern arm_irq_default_handler

/* 13 global registers */
#define ARM_EXCEPTION_FRAME_REGISTER_SP_OFFSET (13 * 4) //=52
/* 40 - 13 = 7 registers containing pre-exception registers */
#define MORE_CONTEXT_SIZE \
    (ARM_EXCEPTION_FRAME_SIZE - ARM_EXCEPTION_FRAME_REGISTER_SP_OFFSET)

/* Save global registers and branch to save context */

FUNC(exception_irq)
    /* TODO IM NOT SURE IF THE IRQ AND FIQ ARE DISABLED WHEN ENTERING ONE */
    cpsid   if

    /* Get current core */
    mrc     p15, 0, r3, c0, c0, 5

    /* Get core control struct from shared area */
    mov     r4, #sizeof_pmk_core_ctrl_t
    mul     r3, r4
    ldr     r4, =air_shared_area
    ldr     r4, [r4, #offsetof_pmk_sharedarea_t_core]
    add     r8, r3 /* pmk_core_ctrl_t */

    /* Get core context from core control */
    ldr     r9, [r8, #offsetof_pmk_core_ctrl_t_context] /* arm_core_context_t */
    ldr     r4, [r9, #offsetof_arm_core_context_t_trash]
    cmp     r4, #(ARM_CORE_CONTEXT_TRASHED)
    bne     arm_exception_irq_save_context


arm_exception_irq_trashed_context:
    /* should be coming from idle. reset to hypervisor isr */
    mov     r5, #1
    str     r5, [r9, #offsetof_arm_core_context_t_isr_nesting_level]

    /* to guarantee no access to unwanted memory happens */
    sub     sp, #(ARM_EXCEPTION_FRAME_SIZE)

    b       arm_exception_irq_handler


arm_exception_irq_save_context:
    sub     sp, #MORE_CONTEXT_SIZE
    push    {r0-r12}
    mov     r4, #6 /* IRQ id */

    /* Save more context */
    mov     r2, lr
    mrs     r3, spsr
    mrs     r7, cpsr
    orr     r5, r3, #(ARM_PSR_I | ARM_PSR_F)
    bic     r5, #ARM_PSR_T

    add     r6, sp, #ARM_EXCEPTION_FRAME_REGISTER_SP_OFFSET

    /* if exception taken from user mode, can't go there and return */
//  tst     r5, #0xf
    and     r0, r5, #ARM_PSR_MODE_MASK
    teq     r0, #ARM_PSR_USR
    bne     more_context /* Z set if user */


user_mode:
    mov     r0, #ARM_PSR_SYS
    bfi     r5, r0, #0, #5  /* Change to SYS mode */

/*
    stm     r6!, {sp,lr}^
    stm     r6, {r2-r4}
*/

more_context:
    msr     cpsr, r5
    mov     r0, sp
    mov     r1, lr
    msr     cpsr, r7
    stm     r6, {r0-r4}


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
#endif /* VFP_D32 */
1:
#endif /* PMK_FPU_SUPPORT */


arm_exception_irq_handler:
    /* Save current sp for nested irqs */
    str     sp, [r9, #offsetof_arm_core_context_t_isf_pointer]

    /* Argument for high level handler */
    mov     r0, sp
    mov     r1, r8 /* pmk_core_ctrl_t */

    /* Call high level handler */
    SWITCH_FROM_ARM_TO_THUMB    r1
    bl      arm_irq_default_handler
    SWITCH_FROM_THUMB_TO_ARM

    /******************* TODO DO REST **********************/
    bx      lr


