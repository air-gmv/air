/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file fpu.S
 * @author lumm
 * @brief FPU lazy switching
 */

/* Are we in assembly file */
#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <armv7.h>
#include <asm_offsets.h>

    .arm
global(arm_restore_fpu)
#if PMK_FPU_SUPPORT
    SWITCH_FROM_THUMB_TO_ARM

    mrc     p15, 0, r1, c1, c0, 2
    orr     r1, #(0b1010 << 20)
    mcr     p15, 0, r1, c1, c0, 2

    ldr     r1, [r0, #(offsetof_pmk_core_ctrl_t_context)]
    ldr     r0, [r1, #(offsetof_arm_core_context_t_isf_pointer)]
    ldr     r1, [r0, #(offsetof_arm_interrupt_stack_frame_t_vfp_context)]
    ldm     r1!, {r2-r3}
    vmsr    fpexc, r2
    tst     r2, #(1 << 30)
    vmsrne  fpscr, r3
    vldmiane r1!, {d0-d15}
    vldmiane r1, {d16-d31}
#endif

#ifdef __thumb__
    orr     lr, #1
#endif
    bx      lr
