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

    .extern arm_svc_handler

/* Save global registers and branch to save context */

FUNC(exception_svc)
    stmfd   sp!, {r0-r5, r12, lr}

    /* get pmk_core_ctrl_t */
    mrc     p15, 0, r3, c0, c0, 5
    mov     r2, #(sizeof_pmk_core_ctrl_t)
    mul     r2, r2, r3
    ldr     r3, =air_shared_area
    ldr     r3, [r3, #(offsetof_pmk_sharedarea_t_core)]
    add     r2, r2, r3

    mov     r1, sp
    mrs     r0, spsr
    push    {r0, r12}   /* store 2 regs to maintain 8-byte-alignment */

    tst     r0, #ARM_PSR_T
    ldrneh  r0, [lr, #-2]
    bicne   r0, r0, #0xff00
    ldreq   r0, [lr, #-4]
    biceq   r0, r0, #0xff000000

    bl      arm_svc_handler

    pop     {r0, r3}
    msr     spsr, r0
    ldmfd   sp!, {r0-r5, r12, pc}^
