/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file exception_irq.S
 * @author lumm
 * @brief IRQ exception handler
 */

/* Are we in assembly file */
#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <armv7.h>
#include <asm_offsets.h>

    .extern arm_irq_default_handler

/* Save global registers and branch to save context
*  r10: isf_pointer as sp
*  r11: isf_pointer
*  r12: context
*  lr:  core_ctrl
*/
    .arm
global(exception_irq)
    push    {r10-r12, lr}

    /* Get Per_CPU core */
    mrc     p15, 0, lr, c13, c0, 4

    ldr     r12, [lr, #offsetof_pmk_core_ctrl_t_context]
    ldr     r11, [r12, #offsetof_arm_core_context_t_isf_pointer]
    ldr     r10, [r12, #offsetof_arm_core_context_t_trash]
    teq     r10, #1
    bne     arm_save_context

arm_save_trashed:
    mov     r0, r11 /* isf_pointer */
    mov     r1, lr /* pmk_core_ctrl_t */

    pop     {r10-r12, lr}
    b       arm_irq_isr_handler

arm_save_context:
    mov     r10, r11
    stmia   r10!, {r0-r9}
    pop     {r0-r3}
    stmia   r10!, {r0, r1}

    mov     r5, r3 /* ret_addr */
    mrs     r6, spsr /* ret_psr */
    mov     r7, #6 /* exception_name */

    orr     r8, r6, #(ARM_PSR_I | ARM_PSR_F)
    bic     r8, #ARM_PSR_T

    /* test if previous mode was user */
    and     r9, r6, #(ARM_PSR_MODE_MASK)
    teq     r9, #(ARM_PSR_USR)
    bne     arm_save_non_user_mode

arm_save_change_to_sys:
    mov     r9, #ARM_PSR_SYS
    bfi     r8, r9, #0, #5 /* Change to SYS mode */

arm_save_non_user_mode:
    mrs     r9, cpsr

    msr     cpsr, r8
    mov     r3, sp /* usr_sp */
    mov     r4, lr /* usr_lr */
    msr     cpsr, r9

    stmia   r10, {r2-r7} /* includes r12 - exception_name */

#if PMK_FPU_SUPPORT
    vmrs    r2, fpexc
    tst     r2, #(1 << 30)
    beq     1f
    vmrs    r3, fpscr
    ldr     r4, [r11, #(offsetof_arm_interrupt_stack_frame_t_vfp_context)]
    stm     r4!, {r2, r3}
    vstmia  r4!, {d0-d15}
    vstmia  r4, {d16-d31}

1:
    mrc     p15, 0, r8, c1, c0, 2
    bic     r8, #(0b1010 << 20)
    mcr     p15, 0, r8, c1, c0, 2
#endif /* PMK_FPU_SUPPORT */

    mov     r0, r11 /* isf_pointer */
    mov     r1, lr /* pmk_core_ctrl_t */

arm_irq_isr_handler:
    /************************ Call high level handler  ************************/
    BL2C    arm_isr_handler
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

/*
    ldr     r1, [r7, #(offsetof_arm_interrupt_stack_frame_t_vfp_context)]
    ldm     r1!, {r2-r3}
    vmsr    fpexc, r2
    vmsr    fpscr, r3
    vldmia  r1!, {d0-d15}
    vldmia  r1, {d16-d31}
*/

arm_restore_context:
    add     r12, r7, #(offsetof_arm_interrupt_stack_frame_t_usr_sp)

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
