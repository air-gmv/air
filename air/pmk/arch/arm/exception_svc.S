/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file exception_svc.S
 * \author lumm
 * \brief SVC handler
 *
 * saves return status and global registers in system stack and branches
 * to svc routine
 */

#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <asm_offsets.h>
#include <armv7.h>
#include <air.h>

    .extern arm_svc_handler

    .arm
global(exception_svc)
    srsfd   sp!, #ARM_PSR_SYS
    cpsid   aif, #ARM_PSR_SYS

    push    {r0-r3, lr}
    add     r0, sp, #28             // the sp needs to be the 1st value on push
    push    {r0}                    // the sp needs to be the 1st value on push

    ldr     r0, [sp, #24]           // pref ret addr (lr)
    ldr     r1, [sp, #28]           // spsr

    tst     r1, #ARM_PSR_T
    ldrneh  r0, [r0, #-2]
    bicne   r0, r0, #0xff00
    ldreq   r0, [r0, #-4]
    biceq   r0, r0, #0xff000000

    /* if rett, then it is returning from the POS HM (special case) */
    cmp     r0, #(AIR_SYSCALL_ARM_RETT)
    beq     arm_rett

    mrc     p15, 0, r1, c13, c0, 4  // get Per_CPU core
    mov     r2, sp                  // arm_supervisor_stack_frame_t

    BL2C    arm_svc_handler

    add     sp, #4
    pop     {r0-r3, lr}
    rfefd   sp!


arm_rett:
    mrc     p15, 0, r0, c13, c0, 4  // get Per_CPU core
    BL2C    arm_syscall_rett

    pop     {r0-r4, lr}
    add     sp, #8
    rfefd   sp!
