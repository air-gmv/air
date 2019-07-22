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

    .arm
    .extern arm_svc_handler

/* Save global registers and branch to save context */
global(exception_svc)
    push    {r0-r3, r12, lr}

    /* get pmk_core_ctrl_t */
    mrc     p15, 0, r2, c13, c0, 4

    mov     r1, sp
    mrs     r0, spsr

    push    {r0, r3}

    tst     r0, #ARM_PSR_T
    ldrneh  r0, [lr, #-2]
    bicne   r0, r0, #0xff00
    ldreq   r0, [lr, #-4]
    biceq   r0, r0, #0xff000000

    SWITCH_FROM_ARM_TO_THUMB    r3
    bl      arm_svc_handler
    SWITCH_FROM_THUMB_TO_ARM

    pop     {r0, r3}
    msr     spsr, r0

    ldmfd   sp!, {r0-r3, r12, pc}^
