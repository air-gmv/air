/*
 * Copyright (C) 2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file exception.S
 * \author lumm
 * \brief exception handlers
 *
 * saves return status and global registers in the IRQ stack and branches to hm routines in SVC mode
 * the IRQ stack is used exclusively as the ISF.
 *
 * More information about exception handling in ARM Compiler Software Development Guide, chapter 5
 *
 *
 */

#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <asm_offsets.h>
#include <armv7.h>
#include <air_arch.h>

    .extern arm_exception_handler

    .arm
    .syntax unified
                                            // -> means "points to"
global(exception_undef)

    push    {r12, r14}                      // Save r12 and r14 in the stack of current mode
    mrs     r12, spsr                       // Get spsr to r12
    tst     r12, #ARM_PSR_T                 // Test for thumb execution state
    subne   lr, #2                          // thumb
    subeq   lr, #4                          // arm
    srsdb   sp, #ARM_PSR_IRQ                // Save LR and SPSR in IRQ stack 
    pop     {r12, r14}                      // Restore back r12 and r14

    mov     r14, #AIR_ARM_EXCEPTION_UNDEF   // Save in r14 exception name
    stmdb   sp, {r12, r14}                  // Save in the stack r12 and 14 (decrease the stack pointer before)
    sub     r12, sp, #8                     // r12 -> ( r12, r14(exception name)) in the abort stack
    cpsid   aif, #ARM_PSR_IRQ               // Change processor state to IRQ mode and disable all interrupts

    sub     sp, #24

    push    {r0-r11}
    add     r7, sp, #48

    ldm     r12, {r5, r6}                   // Load what was pointed by r12 (r12, exception name) into r5 and r6
    stmia   r7!, {r5, r6}                   // Store r5 and r6 (r12, exception name) into addr pointed by r7 (IRQ stack) and r7 -> just after exception name 

    ldr     r0, [r7, #12]                   // spsr

    b       save_previous

global(exception_svc)

    srsdb   sp, #ARM_PSR_IRQ  // save return state (lr and spsr) in the IRQ stack

    mov     r14, #AIR_ARM_EXCEPTION_SWI
    stmdb   sp, {r12, r14}
    sub     r12, sp, #8
    cpsid   aif, #ARM_PSR_IRQ

    sub     sp, #24

    push    {r0-r11}                        // Save r0 to r11 into the stack
    add     r7, sp, #48                     // r7 -> r11, sp -> next context, r12 -> (r12, exception name)

    ldm     r12, {r5, r6}                   // Load what was pointed by r12 (r12, exception name) into r5 and r6
    stmia   r7!, {r5, r6}                   // Store r5 and r6 (r12, exception name) into addr pointed by r7 (IRQ stack) and r7 -> just after exception name

    ldr     r0, [r7, #12]                   // spsr
    
	b       save_previous

global(exception_pref_abort) //This is the exact same as exception_data_abort, can we make it a "function"?
    // When there is a prefetch abort LR points to the instruction at the address following the one that caused the abort, so we need to subtract 4 to get the one that caused the abort
    sub     lr, #4
    srsdb   sp, #ARM_PSR_IRQ

    mov     r14, #AIR_ARM_EXCEPTION_PREF_ABORT
    stmdb   sp, {r12, r14}
    sub     r12, sp, #8
    cpsid   aif, #ARM_PSR_IRQ

    sub     sp, #24

    push    {r0-r11}
    add     r7, sp, #48

    ldm     r12, {r5, r6}
    stmia   r7!, {r5, r6}

    ldr     r0, [r7, #12]                   // spsr

    b       save_previous

global(exception_data_abort)
    // When there is a data abort LR points points two instructions beyond the instruction that caused the abort, so we need to subtract 8 instead of 4 to get the one that caused the abort
    sub     lr, #8                          // Save instruction that led to data abort in LR of current supervisor mode
    srsdb   sp, #ARM_PSR_IRQ                // Save LR and PSR in IRQ stack

    mov     r14, #AIR_ARM_EXCEPTION_DATA_ABORT // Save in r14 exception name 
    stmdb   sp, {r12, r14}                  // Save in the stack r12 and 14 (decrease the stack pointer before) 
    sub     r12, sp, #8                     // r12 -> ( r12, r14(exception name)) in the abort stack
    cpsid   aif, #ARM_PSR_IRQ               // Change to IRQ mode and disable ALL interrupts

    sub     sp, #24

    push    {r0-r11}                        // Now push the gen purpose regs into the IRQ stack
    add     r7, sp, #48                     // r7 -> r11, sp -> next context, r12 ->(r12, exception name)

    ldm     r12, {r5, r6}                   // Load what was pointed by r12 (r12, exception name) into r5 and r6
    stmia   r7!, {r5, r6}                   // Store r5 and r6 (r12, exception name) into addr pointed by r7 (IRQ stack) and r7 -> just after exception name 

    ldr     r0, [r7, #12]                   // spsr

    b       save_previous

global(exception_fiq)

    stmdb   sp, {r12, r14}
    sub     r12, sp, #4
    mov     r14, #ARM_PSR_FIQ

    sub     lr, #8
    srsdb   sp!, #ARM_PSR_IRQ

    cpsid   aif, #ARM_PSR_IRQ

    sub     sp, #16

    push    {r0-r11}
    add     r7, sp, #48

    ldr     r11, [r12]
    stmia   r7!, {r11, r14}

    ldr     r0, [r7, #12]                   // spsr

    b       save_previous


global(exception_irq)

    sub     lr, #4

    srsdb   sp, #ARM_PSR_IRQ
    mov     r14, #AIR_ARM_EXCEPTION_IRQ

    sub     sp, #16
    push    {r0-r12, r14}
    add     r7, sp, #56
    mrs     r0, spsr

save_previous: /* requires the SPSR in r0 */

    /* prepare spsr to jump to previous mode */
    orr     r4, r0, #(ARM_PSR_EXC_MASK)
    bic     r4, #(ARM_PSR_T)
    and     r5, r4, #(ARM_PSR_MODE_MASK)
    teq     r5, #(ARM_PSR_USR)              // check if previous is USR mode
    orreq   r4, #(ARM_PSR_SYS)              // if true, change to SYS

    mrs     r5, cpsr
    msr     cpsr, r4
    mov     r1, sp
    mov     r2, lr
    msr     cpsr, r5

    stmia   r7, {r1, r2}

    mrc     p15, 0, r1, c13, c0, 4          // get Per_CPU core
    ldr     r2, [r1, #offsetof_pmk_core_ctrl_t_context]
    str     sp, [r2, #offsetof_arm_core_context_t_isf_pointer]
    ldr     r3, [r2, #offsetof_arm_core_context_t_trash]
    teq     r3, #1

#if PMK_FPU_SUPPORT
    vmrs    r4, fpexc
    orr     r5, r4, #(ARM_VFP_FPEXC_ENABLE)
    vmsr    fpexc, r5
    beq     c_handler
    vmrs    r5, fpscr
    ldr     r6, [r2, #offsetof_arm_core_context_t_vfp_context]
    stm     r6!, {r4, r5}
    vstmia  r6!, {d0-d15}
    vstmia  r6, {d16-d31}
#endif

    /*
     * r0: ISF pointer |  r1: pmk_core_ctrl_t | r2: core context
     */
c_handler:
    mov     r0, sp
    cpsid   aif, #ARM_PSR_SVC // change to SVC mode to perform C handler

    // Store SVC SP in CORE CONTEXT (R2 points to pmk_core_ctrl_t context)
    str     sp, [r2, #offsetof_arm_core_context_t_virt] // store svc_sp (first element of virt struct)

    // Branch into C exception Handler
    BL2C    arm_exception_handler

    // Restore SVC SP from CORE CONTEXT:
    mrc     p15, 0, r1, c13, c0, 4                                       // get Per_CPU core
    ldr     r1, [r1, #offsetof_pmk_core_ctrl_t_context]                  // get core context
    ldr     r2, [r1, #offsetof_arm_core_context_t_virt]                  // get svc sp (first element of virt struct)
    mov     sp, r2                                                       // change to this SVC SP

    //Go into IRQ mode to restore the context
    cpsid   aif, #ARM_PSR_IRQ

restore:
    add     sp, #56
    pop     {r0-r3}

    /* prepare spsr to jump to previous mode */
    orr     r4, r3, #(ARM_PSR_EXC_MASK)
    bic     r4, #(ARM_PSR_T)
    and     r5, r4, #(ARM_PSR_MODE_MASK)
    teq     r5, #(ARM_PSR_USR)              // check if previous is USR mode
    orreq   r4, #(ARM_PSR_SYS)              // if true, change to SYS

    mrs     r5, cpsr
    msr     cpsr, r4
    mov     sp, r0                          // previous mode sp
    mov     lr, r1                          // previous mode lr
    msr     cpsr, r5

    mrc     p15, 0, r0, c13, c0, 4          // get Per_CPU core
    ldr     r0, [r0, #offsetof_pmk_core_ctrl_t_context]
    str     sp, [r0, #offsetof_arm_core_context_t_isf_pointer]

    sub     sp, #72

#if PMK_FPU_SUPPORT
    vmrs    r4, fpexc
    orr     r4, #(ARM_VFP_FPEXC_ENABLE)
    vmsr    fpexc, r4
    ldr     r6, [r0, #offsetof_arm_core_context_t_vfp_context]
    ldmia   r6!, {r4, r5}
    vldmia  r6!, {d0-d15}
    vldmia  r6,  {d16-d31}
    vmsr    fpscr, r5
    vmsr    fpexc, r4
#endif

    pop     {r0-r12}
    dmb

    add     sp, #12

    rfe     sp!
