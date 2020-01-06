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
#include <asm_offsets.h>
#include <armv7.h>

    .thumb
    .syntax unified
global(arm_restore_fpu)
#if PMK_FPU_SUPPORT
    push    {r4-r5}

    mrc     p15, 0, r4, c1, c0, 2
    orr.w   r4, #(0b1010 << 20)
    mcr     p15, 0, r4, c1, c0, 2

    isb

    ldm     r0!, {r4-r5}
    vmsr    fpexc, r4

    tst.w   r4, #(1 << 30)
    ittt    ne
    vmsrne  fpscr, r5
    vldmiane r0!, {d0-d15}
    vldmiane r0, {d16-d31}

    pop     {r4-r5}
#endif

    bx      lr
