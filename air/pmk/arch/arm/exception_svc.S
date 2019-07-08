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

    .extern svc_handler

/* 13 global registers */
#define ARM_EXCEPTION_FRAME_REGISTER_SP_OFFSET (13 * 4) /* =52 */
/* 40 - 13 = 7 registers containing pre-exception registers */
#define MORE_CONTEXT_SIZE \
    (ARM_EXCEPTION_FRAME_SIZE - ARM_EXCEPTION_FRAME_REGISTER_SP_OFFSET)

/* Save global registers and branch to save context */

ENTRY(exception_svc)
    sub     sp, #MORE_CONTEXT_SIZE
    push    {r0-r12}

    /* Save more context */
    mov     r2, lr
    mrs     r3, spsr
    mrs     r8, cpsr
    orr     r5, r3, #(ARM_PSR_I | ARM_PSR_F)
    bic     r5, #ARM_PSR_T

    add     r7, sp, #ARM_EXCEPTION_FRAME_REGISTER_SP_OFFSET

    /* if exception taken from user mode, can't go there and return */
/*  tst     r5, #0xf */
    and     r0, r5, #ARM_PSR_MODE_MASK
    teq     r0, #ARM_PSR_USR
    beq     user_mode   /* Z set if user */

    b       more_context

user_mode:
    mov     r0, #ARM_PSR_SYS
    bfi     r5, r0, #0, #5  /* Change to SYS mode */

/*
    stm     r7!, {sp,lr}^
    stm     r7, {r2-r4}
*/
more_context:

    msr     cpsr, r5
    mov     r0, sp
    mov     r1, lr

    msr     cpsr, r8
    mov     r4, #2              /* SVC id */
    mov     r5, #0              /* VFP context pointer */
    ldr     r6, [lr, #-4]       /* SVC instruction */
    and     r6, r6, #0xffffff   /* Retrieve the service being requested */
    stm     r7, {r0-r6}



    /* Argument for high level handler (value in r0-r3 are used as arguments) */
    mov     r0, sp


    /* Clear VFP context pointer (done before) */
//  add     r3, sp, #ARM_EXCEPTION_FRAME_VFP_CONTEXT_OFFSET
//  mov     r1, #0
//  str     r1, [r3]

#ifdef PMK_FPU_SUPPORT
    /* Ensure that the FPU is enabled */
    vmrs    r1, FPEXC
    tst     r1, #(1 << 30)
    beq     1f

    /* Passes the stack pointer to after the VFP context TODO why the -4 if full descending*/
    sub     sp, #(ARM_VFP_CONTEXT_SIZE + 4)


    /* Save VFP context */
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

    /* just to be sure */
    mov     r1, #0
    mov     r2, #0
    mov     r3, #0
    /* Call high level handler */
    SWITCH_FROM_ARM_TO_THUMB    r1
    bl      svc_handler

#endif /* ASM */
