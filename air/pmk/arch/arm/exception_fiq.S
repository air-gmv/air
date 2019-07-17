/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file exceptions_fiq.S
 * @author lumm
 * @brief FIQ exception handler
 */

/* Are we in assembly file */
#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <asm_offsets.h>
#include <armv7.h>

    .extern arm_irq_handler

/**
 * FIQ Handler
 *  r10: isf_pointer
 *  r11: pmk_core_ctrl_t
 *  r12: arm_core_context_t
 */
    .arm
FUNC(exception_fiq)

    /* Get current core */
    mrc     p15, 0, r8, c0, c0, 5

    /* Get core control struct from shared area */
    mov     r9, #(sizeof_pmk_core_ctrl_t)
    mul     r8, r9
    ldr     r9, =air_shared_area
    ldr     r9, [r9, #(offsetof_pmk_sharedarea_t_core)]
    add     r10, r9, r8 //pmk_core_ctrl_t

    /* Get core context from core control */
    ldr     r11, [r10, #(offsetof_pmk_core_ctrl_t_context)] //arm_core_context_t

    /* Get the ISF pointer */
    ldr     r12, [r11, #(offsetof_arm_core_context_t_isf_pointer)]

    /* Check if context trashed (idle) */
    ldr     r8, [r11, #(offsetof_arm_core_context_t_trash)]
    cmp     r8, #(ARM_CORE_CONTEXT_TRASHED)
    bne     arm_fiq_save_context


arm_fiq_trashed_context:
    /* reset to hypervisor isr nesting level */
    mov     r8, #1
    str     r8, [r11, #(offsetof_arm_core_context_t_isr_nesting_level)]

    b       arm_fiq_isr_handler_from_trashed


arm_fiq_save_context:
    stm     r12!, {r0-r12}^
    mov     r2, lr
    mrs     r3, spsr
    mov     r4, #7 /* FIQ id */
    mov     r5, #0 /* Clear vfp_ptr */

    orr     r6, r3, #(ARM_PSR_I | ARM_PSR_F)
    bic     r6, #ARM_PSR_T

    /* test if previous mode was user */
    and     r7, r3, #(ARM_PSR_MODE_MASK)
    teq     r7, #(ARM_PSR_USR)
    bne     save_non_user_mode

save_change_to_sys:
    mov     r7, #ARM_PSR_SYS
    bfi     r6, r7, #0, #5 /* Change to SYS mode */

save_non_user_mode:
    mrs     r7, cpsr

    msr     cpsr, r6
    mov     r0, sp
    mov     r1, lr
    msr     cpsr, r7

    mov     r6, #0
    stm     r12!, {r0-r6}

arm_fiq_isr_handler:
    /* Save current isf for nested irqs */
    sub     r12, r12, #(sizeof_arm_interrupt_stack_frame_t)
    str     r12, [r11, #(offsetof_arm_core_context_t_isf_pointer)]

arm_fiq_isr_handler_from_trashed:
    /* Arguments for high level handler */
    mov     r0, r12 /* isf_pointer */
    mov     r1, r10 /* pmk_core_ctrl_t */


    /************************ Call high level handler  ************************/
    SWITCH_FROM_ARM_TO_THUMB    r2
    bl      arm_irq_handler
    SWITCH_FROM_THUMB_TO_ARM
    /********************* Return from high level handler *********************/

    cpsid   if

    /* Get current core */
    mrc     p15, 0, r8, c0, c0, 5

    /* Get core control struct from shared area */
    mov     r9, #(sizeof_pmk_core_ctrl_t)
    mul     r8, r9
    ldr     r9, =air_shared_area
    ldr     r9, [r9, #(offsetof_pmk_sharedarea_t_core)]
    add     r10, r9, r8 //pmk_core_ctrl_t

    /* Get core context from core control */
    ldr     r11, [r10, #(offsetof_pmk_core_ctrl_t_context)] //arm_core_context_t

    /* Check if context trashed (idle) */
    ldr     r8, [r11, #(offsetof_arm_core_context_t_trash)]
    cmp     r8, #(ARM_CORE_CONTEXT_TRASHED)
    bne     arm_fiq_restore_context

arm_fiq_restore_trashed_context:
    /* Get the idle ISF pointer */
    ldr     r12, [r11, #(offsetof_arm_core_context_t_idle_isf_pointer)]
    b       arm_fiq_restore_trashed_context

arm_fiq_restore_context:
    /* Get the ISF pointer */
    ldr     r12, [r11, #(offsetof_arm_core_context_t_isf_pointer)]

arm_fiq_restore_context_from_trashed:
    ldm     r12!, {r0-r12}^

    ldm     r12, {r8-r11}
    mov     lr, r10
    orr     r10, r11, #(ARM_PSR_I | ARM_PSR_F)
    bic     r10, #ARM_PSR_T
    and     r12, r11, #(ARM_PSR_MODE_MASK)
    teq     r12, #(ARM_PSR_USR)
    bne     restore_non_user_mode

restore_change_to_sys:
    mov     r12, #ARM_PSR_SYS
    bfi     r10, r12, #0, #5

restore_non_user_mode:
    mrs     r12, cpsr

    msr     cpsr, r10
    mov     sp, r8
    mov     lr, r9
    msr     cpsr, r12

    msr     spsr, r11
    subs    pc, lr, #4
