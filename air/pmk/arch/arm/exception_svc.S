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

/* Are we in assembly file. Yes we are */
#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <asm_offsets.h>
#include <armv7.h>
#include <air.h>

    .arm
    .extern arm_svc_handler

/* Save global registers and branch to save context */
global(exception_svc)
    push    {r0-r3, r12, lr}

    /* get pmk_core_ctrl_t */
    mrc     p15, 0, r0, c13, c0, 4

    mov     r2, sp
    mrs     r1, spsr

    push    {r1, r3}

    tst     r1, #ARM_PSR_T
    ldrneh  r1, [lr, #-2]
    bicne   r1, r1, #0xff00
    ldreq   r1, [lr, #-4]
    biceq   r1, r1, #0xff000000

    /* if return from trap, just exit peacefully, all is already taken care of */
    teq     r1, #(AIR_SYSCALL_ARM_RETT)
    beq     arm_rett

    BL2C    arm_svc_handler

    pop     {r1, r3}
    msr     spsr, r1

    ldmfd   sp!, {r0-r3, r12, pc}^


arm_rett:
    BL2C    arm_syscall_rett

    add     sp, #32
    cpsid   aif, #ARM_PSR_SYS
    rfefd   sp!
