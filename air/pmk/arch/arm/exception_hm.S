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

    push    {r0-r4, lr}
    mov     r0, #7
    mov     r1, #0
    mov     r2, #0

    /* Call high level handler */
    mrc     p15, 0, r3, c13, c0, 4
    BL2C    arm_hm_handler

    /* Get Per_CPU core */
    mrc     p15, 0, r1, c13, c0, 4

    ldr     r2, [r1, #offsetof_pmk_core_ctrl_t_context]
    ldr     r3, [r2, #offsetof_arm_core_context_t_trash]
    /* context is trashed */
    teq     r3, #1
    beq     restore_trashed_context

    /* return to hm exception */
    teq     r0, #0
    streq   r0, [sp, #24] // lr location

    pop     {r0-r4, lr}
    rfefd   sp!


global(exception_pref_abort)
    sub     lr, #8
    srsfd   sp!, #ARM_PSR_SYS
    srsfd   sp!, #ARM_PSR_SYS // duplicate for pos hm
    cpsid   aif, #ARM_PSR_SYS

    push    {r0-r4, lr}
    mov     r0, #3
    mrc     p15, 0, r1, c6, c0, 2
    mrc     p15, 0, r2, c5, c0, 1

    b       1f

global(exception_data_abort)
    sub     lr, #8
    srsfd   sp!, #ARM_PSR_SYS
    srsfd   sp!, #ARM_PSR_SYS // duplicate for pos hm
    cpsid   aif, #ARM_PSR_SYS

    push    {r0-r4, lr}
    mov     r0, #4
    mrc     p15, 0, r1, c6, c0, 0
    mrc     p15, 0, r2, c5, c0, 0

1:
    ldr     r3, [sp, #24] // preferred return addr

    /* Call high level handler */
    BL2C    arm_hm_handler

    /* Get Per_CPU core */
    mrc     p15, 0, r1, c13, c0, 4

    ldr     r2, [r1, #offsetof_pmk_core_ctrl_t_context]
    ldr     r3, [r2, #offsetof_arm_core_context_t_trash]

    /* context is trashed */
    teq     r3, #1
    beq     restore_trashed_context

    /* return to hm exception */
    teq     r0, #0
    beq     2f
    str     r0, [sp, #24] // lr location
    ldr     r0, [sp, #28] // toggle thumb->ARM
    bic     r0, #ARM_PSR_T
    str     r0, [sp, #28]
    pop     {r0-r4, lr}
    rfefd   sp!

2:
    pop     {r0-r4, lr}
    add     sp, #8 // due to the duplication
    rfefd   sp!


global(exception_undef)
    push    {r0, r1}
    mrs     r0, spsr
    tst     r0, #(ARM_PSR_T)
    subeq   lr, #4 //ARM
    subne   lr, #2 //Thumb
    pop     {r0, r1}

    srsfd   sp!, #ARM_PSR_SYS
    srsfd   sp!, #ARM_PSR_SYS // duplicate for pos hm
    cpsid   aif, #ARM_PSR_SYS

    push    {r0-r4,lr}

    mov     r0, #1
    ldr     r3, [sp, #24] // preferred return addr
    ldr     r2, [sp, #28] // spsr
    mov     r1, #0

    /* Call high level handler */
    BL2C    arm_hm_handler

    /* Get Per_CPU core */
    mrc     p15, 0, r1, c13, c0, 4

    ldr     r2, [r1, #offsetof_pmk_core_ctrl_t_context]
    ldr     r3, [r2, #offsetof_arm_core_context_t_trash]

    /* context is trashed */
    teq     r3, #1
    beq     restore_trashed_context

    /* return to hm exception */
    teq     r0, #0
    beq     2f
    str     r0, [sp, #24] // lr location
    ldr     r0, [sp, #28] // toggle thumb->ARM
    bic     r0, #ARM_PSR_T
    str     r0, [sp, #28]
    pop     {r0-r4, lr}
    rfefd   sp!

2:
    pop     {r0-r4, lr}
    add     sp, #8 // due to the duplication
    rfefd   sp!


/* if the partition halts or restarts, it returns as idle */
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
