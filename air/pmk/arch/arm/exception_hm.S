/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file exception_hm.S
 * \author lumm
 * \brief HM exception handlers
 *
 * saves return status and global registers in system stack and branches
 * to hm routines
 *
 * \note
 * the HM handler must not use FPU registers, or these must be saved aswell
 */

#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <armv7.h>
#include <asm_offsets.h>

    .extern arm_hm_handler

    .arm
global(exception_undef)
    push    {r3, r4}
    mrs     r3, spsr
    tst     r3, #(ARM_PSR_T)
    subeq   lr, #4                  // LR-4 if ARM
    subne   lr, #2                  // LR-2 if Thumb
    pop     {r3, r4}

    srsfd   sp!, #ARM_PSR_SYS
    srsfd   sp!, #ARM_PSR_SYS       // duplicate for pos hm
    cpsid   aif, #ARM_PSR_SYS

    push    {r0-r12, lr}

    mov     r0, #ARM_EXCEPTION_UNDEF
    ldr     r1, [sp, #60]           // spsr
    ldr     r2, [sp, #56]           // preferred return addr

    b       high_level_handler


global(exception_pref_abort)
    sub     lr, #8
    srsfd   sp!, #ARM_PSR_SYS
    srsfd   sp!, #ARM_PSR_SYS       // duplicate for pos hm
    cpsid   aif, #ARM_PSR_SYS

    push    {r0-r12, lr}

    mov     r0, #ARM_EXCEPTION_PREF_ABORT
    mrc     p15, 0, r1, c5, c0, 1   // IFSR
    mrc     p15, 0, r2, c6, c0, 2   // IFAR

    b       high_level_handler


global(exception_data_abort)
    sub     lr, #8
    srsfd   sp!, #ARM_PSR_SYS
    srsfd   sp!, #ARM_PSR_SYS       // duplicate for pos hm
    cpsid   aif, #ARM_PSR_SYS

    push    {r0-r12, lr}

    mov     r0, #ARM_EXCEPTION_DATA_ABORT
    mrc     p15, 0, r1, c5, c0, 0   // DFSR
    mrc     p15, 0, r2, c6, c0, 0   // DFAR

    b       high_level_handler


global(exception_fiq)
    sub     lr, #4
    srsfd   sp!, #ARM_PSR_SYS
    srsfd   sp!, #ARM_PSR_SYS       // duplicate for pos hm
    cpsid   aif, #ARM_PSR_SYS

    push    {r0-r12, lr}
    mov     r0, #ARM_EXCEPTION_FIQ
    mov     r1, #0
    mov     r2, #0


high_level_handler:
    ldr     r3, [sp, #56]           // preferred return addr

    /* Call high level handler */
    BL2C    arm_hm_handler

    mrc     p15, 0, r1, c13, c0, 4  // get Per_CPU core

    ldr     r2, [r1, #offsetof_pmk_core_ctrl_t_context]
    ldr     r3, [r2, #offsetof_arm_core_context_t_trash]

    cmp     r3, #1                  // is context trashed
    beq     return_to_idle

    cmp     r0, #0                  // is ret addr NULL
    beq     return_to_partition

    /* Return to hm exception */
    str     r0, [sp, #56]           // pref ret_addr location
    ldr     r0, [sp, #60]
    bic     r0, #ARM_PSR_T          // toggle thumb->ARM
    str     r0, [sp, #60]

    pop     {r0-r12, lr}
    rfefd   sp!


    /* if the partition halts or restarts, it returns as idle */
return_to_idle:
    ldr     r3, [r2, #(offsetof_arm_core_context_t_idle_isf_pointer)]

    add     r4, r3, #(offsetof_arm_interrupt_stack_frame_t_ret_addr)
    ldm     r4, {r5, r6}

    add     sp, #72                 // clears stack
    push    {r5, r6}                // emulates srs with idle lr and spsr

    rfefd   sp!

    /* returns the partition to normal execution */
return_to_partition:
    pop     {r0-r12, lr}
    add     sp, #8                  // due to the duplication
    rfefd   sp!
