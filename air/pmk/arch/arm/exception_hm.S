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
 * TODO Need to see if it is entering in ARM of Thumb mode
 */

/* Are we in assembly file */
#ifdef ASM

#include <asm.h>
#include <armv7.h>
#include <asm_offsets.h>

    .extern arm_hm_handler

    .arm
/* Save global registers and branch to save context */
global(exception_fiq)
    sub     lr, #4
    srsfd   sp!, #ARM_PSR_SYS
    cpsid   aif, #ARM_PSR_SYS

    push    {r0-r3}
    mov     r0, #7
    mov     r1, #0
    mov     r2, #0

    /* Call high level handler */
    SWITCH_FROM_ARM_TO_THUMB    r3
    mrc     p15, 0, r3, c13, c0, 4
    bl      arm_hm_handler
    SWITCH_FROM_THUMB_TO_ARM

    /* Get Per_CPU core */
    mrc     p15, 0, r1, c13, c0, 4

    ldr     r2, [r1, #offsetof_pmk_core_ctrl_t_context]
    ldr     r3, [r2, #offsetof_arm_core_context_t_trash]

    teq     r3, #1
    /* context is trashed */
    beq     restore_trashed_context

    pop     {r0-r3}
    rfefd   sp!


global(exception_pref_abort)
    sub     lr, #8
    srsfd   sp!, #ARM_PSR_SYS
    cpsid   aif, #ARM_PSR_SYS

    push    {r0-r3}
    mov     r0, #3
    mrc     p15, 0, r1, c6, c0, 2
    mrc     p15, 0, r2, c5, c0, 1

    b       1f

global(exception_data_abort)
    sub     lr, #8
    srsfd   sp!, #ARM_PSR_SYS
    cpsid   aif, #ARM_PSR_SYS

    push    {r0-r3}
    mov     r0, #4
    mrc     p15, 0, r1, c6, c0, 0
    mrc     p15, 0, r2, c5, c0, 0

1:
    /* Call high level handler */
    SWITCH_FROM_ARM_TO_THUMB    r3
    mrc     p15, 0, r3, c13, c0, 4
    bl      arm_hm_handler
    SWITCH_FROM_THUMB_TO_ARM

    /* Get Per_CPU core */
    mrc     p15, 0, r1, c13, c0, 4

    ldr     r2, [r1, #offsetof_pmk_core_ctrl_t_context]
    ldr     r3, [r2, #offsetof_arm_core_context_t_trash]

    teq     r3, #1
    /* context is trashed */
    beq     restore_trashed_context

    pop     {r0-r3}
    rfefd   sp!


global(exception_undef)
    push    {r0, r1}
    mrs     r0, spsr
    tst     r0, #(ARM_PSR_T)
    subeq   lr, #4 //ARM
    subne   lr, #2 //Thumb
    pop     {r0, r1}
    srsfd   sp!, #ARM_PSR_SYS
    cpsid   aif, #ARM_PSR_SYS

    push    {r0-r3}

    mov     r0, #1
    ldr     r1, [sp, #16]
    ldr     r2, [sp, #20]

    /* Call high level handler */
    SWITCH_FROM_ARM_TO_THUMB    r3
    mrc     p15, 0, r3, c13, c0, 4
    bl      arm_hm_handler
    SWITCH_FROM_THUMB_TO_ARM

    /* Get Per_CPU core */
    mrc     p15, 0, r1, c13, c0, 4

    ldr     r2, [r1, #offsetof_pmk_core_ctrl_t_context]
    ldr     r3, [r2, #offsetof_arm_core_context_t_trash]

    teq     r3, #1
    /* context is trashed */
    beq     restore_trashed_context

    pop     {r0-r3}
    rfefd   sp!



restore_trashed_context:
    /* Get the idle ISF pointer */
    ldr     r3, [r2, #(offsetof_arm_core_context_t_idle_isf_pointer)]

    add     r4, r3, #(offsetof_arm_interrupt_stack_frame_t_usr_sp)

    ldmia   r4, {r8-r11}
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

    ldm     r4, {r0-r12}

#ifdef __thumb__
    orr     lr, #1
#endif
    subs    pc, lr, #4


#endif /* ASM */
