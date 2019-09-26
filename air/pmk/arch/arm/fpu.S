/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file fpu.S
 * \author lumm
 * \brief FPU lazy switching
 */

#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <armv7.h>
#include <asm_offsets.h>

    .thumb
    .syntax unified

global(arm_restore_fpu)
#if PMK_FPU_SUPPORT
    mrc     p15, 0, r1, c1, c0, 2
    orr.w   r1, #(0b1010 << 20)
    mcr     p15, 0, r1, c1, c0, 2

    ldm     r0!, {r2-r3}
    vmsr    fpexc, r2

    tst.w   r2, #(1 << 30)
    ittt    ne
    vmsrne  fpscr, r3
    vldmiane r0!, {d0-d15}
    vldmiane r0, {d16-d31}
#endif

#ifdef __thumb__
    orr     lr, #1
#endif
    bx      lr
