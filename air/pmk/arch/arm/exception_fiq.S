/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file exception_fiq.S
 * \author lumm
 * \brief FIQ (timer) exception handler
 *
 * saves the general-purpose registers from USR, the prefered return address
 * and SPSR ahead of context save/restore to avoid any corruption
 *
 * \note
 * r10, r0: used arm_interrupt_stack_frame_t
 * r12: original arm_interrupt_stack_frame_t
 * r9:  arm_core_context_t
 * r8:  pmk_core_ctrl_t
 */

#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <asm_offsets.h>
#include <armv7.h>

    .extern arm_scheduler

    .arm
global(exception_fiq)
    mrc     p15, 0, r8, c13, c0, 4  // get Per_CPU core

    ldr     r9, [r8, #offsetof_pmk_core_ctrl_t_context]
    ldr     r10, [r9, #offsetof_arm_core_context_t_isf_pointer]
    mov     r12, r10

    ldr     r11, [r9, #offsetof_arm_core_context_t_trash]

    cmp     r11, #1                 // trashed context, no point in saving
    beq     arm_fiq_handler

arm_save_context:
    stmia   r10!, {r0-r7}
    mov     r0, r10
    mov     r7, #AIR_ARM_EXCEPTION_FIQ
    cpsid   aif, #ARM_PSR_SYS
    stmia   r0!, {r7-r14}
    cpsid   aif, #ARM_PSR_FIQ

    mov     r1, sp
    mov     sp, r0
    srsia   sp, #ARM_PSR_FIQ
    mov     sp, r1

#if PMK_FPU_SUPPORT
    vmrs    r2, fpexc
    vmrs    r3, fpscr
    ldr     r4, [r9, #(offsetof_arm_core_context_t_vfp_context)]
    stmia   r4!, {r2, r3}
    vstmia  r4!, {d0-d15}
    vstmia  r4, {d16-d31}

    mrc     p15, 0, r8, c1, c0, 2
    bic     r8, #(0b1010 << 20)
    mcr     p15, 0, r8, c1, c0, 2
#endif /* PMK_FPU_SUPPORT */

arm_fiq_handler:
    mov     r0, r12
    mov     r1, r8

    cpsid   aif, #ARM_PSR_SVC

    /* Call high level handler */
    BL2C    arm_isr_handler

    cpsid   aif, #ARM_PSR_FIQ

    ldr     r11, [r9, #offsetof_arm_core_context_t_trash]
    cmp     r11, #1
    addeq   r12, #64                // jump to Saved Return Status
    beq     return

arm_restore_context:
    ldmia   r12!, {r0-r7}
    mov     r10, r0
    mov     r11, r7
    mov     r0, r12
    cpsid   aif, #ARM_PSR_SYS
    ldmia   r0!, {r7-r14}
    cpsid   aif, #ARM_PSR_FIQ
    mov     r12, r0
    mov     r0, r10
    mov     r7, r11

return:
    rfeia   r12
