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
#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <asm_offsets.h>
#include <armv7.h>

    .extern svc_handler

#define offsetof_arm_interrupt_stack_frame_t_ctx \
        (sizeof_arm_interrupt_stack_frame_t - \
        offsetof_arm_interrupt_stack_frame_t_orig_sp)

/* Save global registers and branch to save context */

FUNC(exception_svc)
    sub     sp, #(offsetof_arm_interrupt_stack_frame_t_ctx)
    push    {r0-r12}

    add     r10, sp, offsetof_arm_interrupt_stack_frame_t_orig_sp

    /* Save more context */
    mov     r2, lr
    mrs     r3, spsr
    mov     r4, #2 /* SWI id */
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

    /* todo considering that its running on arm32 in user */
    ldr     r6, [lr, #-4]
    bic     r6, r6, #0xff000000

    stm     r10!, {r0-r6}

    /* Argument for high level handler (value in r0-r3 are used as arguments) */
    mov     r0, sp

    /* Call high level handler */
    SWITCH_FROM_ARM_TO_THUMB    r1
    bl      svc_handler
    SWITCH_FROM_THUMB_TO_ARM

    cpsid   if

    // TODO http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0203j/Cacdfeci.html

    movs    pc, lr
