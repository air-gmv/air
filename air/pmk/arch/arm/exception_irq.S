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

    /* Save global registers and branch to save context */

    .arm
global(exception_irq)
    push    {r10-r12, lr}

    /* Get Per_CPU core */
    mrc     p15, 0, lr, c13, c0, 4

    ldr     r12, [lr, #offsetof_pmk_core_ctrl_t_context]
    ldr     r11, [r12, #offsetof_arm_core_context_t_trash]
    ldr     r12, [r12, #offsetof_arm_core_context_t_isf_pointer]
    teq     r11, #1
    bne     arm_save_context

arm_save_trashed:
    mov     r0, r12 /* isf_pointer */
    mov     r1, lr  /* pmk_core_ctrl_t */
    pop     {r10-r12, lr}
//  add     lr, #4
    b       arm_isr_handler

arm_save_context:
    mov     r10, r12
    mov     r11, lr

    stmia   r12!, {r0-r9}
    mov     r0, r10 /* isf_pointer */
    mov     r1, r11 /* pmk_core_ctrl_t */
    pop     {r10, r11}
    stmia   r12!, {r10, r11}
    mov     r10, r12
    pop     {r12, lr}

    mov     r2, r12
    mov     r5, lr
    mrs     r6, spsr
    mov     r7, #0

    orr     r8, r6, #(ARM_PSR_I | ARM_PSR_F)
    bic     r8, #ARM_PSR_T

    /* test if previous mode was user */
    and     r9, r6, #(ARM_PSR_MODE_MASK)
    teq     r9, #(ARM_PSR_USR)
    bne     arm_save_non_user_mode

arm_save_change_to_sys:
    mov     r9, #ARM_PSR_SYS
    bfi     r8, r9, #0, #5    /* Change to SYS mode */

arm_save_non_user_mode:
    mrs     r9, cpsr

    msr     cpsr, r8
    mov     r3, sp
    mov     r4, lr
    msr     cpsr, r9

    stmia   r10, {r2-r7}

#ifdef PMK_FPU_SUPPORT
    vmrs    r2, fpexc
    vmrs    r3, fpscr
    ldr     r4, [r0, #(offsetof_arm_interrupt_stack_frame_t_vfp_context)]
    stm     r4!, {r2, r3}
    vstmia  r4!, {d0-d15}
    vstmia  r4, {d16-d31}
#endif /* PMK_FPU_SUPPORT */

arm_isr_handler:
    /************************ Call high level handler  ************************/
    SWITCH_FROM_ARM_TO_THUMB    r2
    bl      arm_irq_handler
    SWITCH_FROM_THUMB_TO_ARM
    /********************* Return from high level handler *********************/

    /* Get Per_CPU core */
    mrc     p15, 0, r12, c13, c0, 4

    ldr     r11, [r12, #(offsetof_pmk_core_ctrl_t_context)]
    ldr     r10, [r11, #(offsetof_arm_core_context_t_trash)]
    teq     r10, #1
    bne     arm_restore_not_trashed

arm_restore_trashed:
    /* Get the idle ISF pointer */
    ldr     r7, [r11, #(offsetof_arm_core_context_t_idle_isf_pointer)]
    b       arm_restore_context

arm_restore_not_trashed:
    /* Get the ISF pointer */
    ldr     r7, [r11, #(offsetof_arm_core_context_t_isf_pointer)]

arm_restore_context:
#ifdef PMK_FPU_SUPPORT
    ldr     r1, [r7, #(offsetof_arm_interrupt_stack_frame_t_vfp_context)]
    ldm     r1!, {r2-r3}
    tst     r2, #(ARM_VFP_FPEXC_ENABLE)
    beq     1f

    vldmia  r1!, {d0-d15}
    vldmia  r1, {d16-d31}
    vmsr    fpexc, r2
    vmsr    fpscr, r3
1:
#endif
    add     r12, r7, #(offsetof_arm_interrupt_stack_frame_t_orig_sp)

    ldmia   r12, {r8-r11}
    mov     lr, r10
    orr     r10, r11, #(ARM_PSR_I | ARM_PSR_F)
    bic     r10, #ARM_PSR_T
    and     r12, r11, #(ARM_PSR_MODE_MASK)
    teq     r12, #(ARM_PSR_USR)
    bne     arm_restore_non_user_mode

arm_restore_change_to_sys:
    mov     r12, #ARM_PSR_SYS
    bfi     r10, r12, #0, #5

arm_restore_non_user_mode:
    mrs     r12, cpsr

    msr     cpsr, r10
    mov     sp, r8
    mov     lr, r9
    msr     cpsr, r12

    msr     spsr, r11

    ldm     r7, {r0-r12}

#ifdef __thumb__
    orr     lr, #1
#endif
    subs    pc, lr, #4
