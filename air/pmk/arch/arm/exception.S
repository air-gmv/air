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

global(exception_undef)

    push    {r12, r14}
    mrs     r12, spsr
    tst     r12, #ARM_PSR_T
    subne   lr, #2                          //thumb
    subeq   lr, #4                          //arm
    srsdb   sp, #ARM_PSR_IRQ
    pop     {r12, r14}

    mov     r14, #ARM_EXCEPTION_UNDEF
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

global(exception_svc)

    srsdb   sp, #ARM_PSR_IRQ
/*----*/
    push    {r0,r1}
    mrs     r1, spsr
    tst     r1, #ARM_PSR_T
    ldrneh  r0, [lr, #-2]           //thumb
    bicne   r0, r0, #0xff00
    ldreq   r0, [lr, #-4]           //arm
    biceq   r0, r0, #0xff000000

    cmp     r0, #(AIR_SYSCALL_ARM_RETT)
    pop     {r0,r1}

    mov     r14, #ARM_EXCEPTION_SWI
    stmdb   sp, {r12, r14}
    sub     r12, sp, #8
    cpsid   aif, #ARM_PSR_IRQ
/**/
    beq     arm_rett
/**/
    sub     sp, #24
    push    {r0-r11}
    add     r7, sp, #48

    ldm     r12, {r5, r6}
    stmia   r7!, {r5, r6}

    ldr     r0, [r7, #12]                   // spsr

    b       save_previous

arm_rett:
    push    {r0, r1, lr}
    ldr     r0, [sp, #80]
    and     r0, #(ARM_PSR_MODE_MASK)
    cmp     r0, #(ARM_PSR_SVC)      //if previous mode was svc, how do we know there was a context switch?
    beq     arm_return
    mrs     r0, cpsr
    mov     r1, r0
    orr     r0, #(ARM_PSR_SYS) //change to system mode
    msr     cpsr, r0
    mov     r0, sp             //get user sp
    msr     cpsr, r1
    ldr     r1, [sp, #68]       //get return sp
    cmp     r0, r1              //if return sp is different from
    bne     arm_return    //user sp, there was a context switch
    mrc     p15, 0, r0, c13, c0, 4  // get Per_CPU core
    BL2C    arm_syscall_rett
    pop     {r0, r1, lr}
    b       restore
//TODO: check if there are pending interrupts before returning to the partition!

arm_return:
    mrc     p15, 0, r0, c13, c0, 4  // get Per_CPU core
    BL2C    arm_syscall_rett
    pop     {r0, r1, lr}
    b       restore

global(exception_pref_abort)

    sub     lr, #4
    srsdb   sp, #ARM_PSR_IRQ

    mov     r14, #ARM_EXCEPTION_PREF_ABORT
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

    sub     lr, #8
    srsdb   sp, #ARM_PSR_IRQ

    mov     r14, #ARM_EXCEPTION_DATA_ABORT
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
    mov     r14, #ARM_EXCEPTION_IRQ

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
    beq     c_handler

#if PMK_FPU_SUPPORT
    vmrs    r4, fpexc
    cmp     r4, #0
    beq     c_handler
    vmrs    r5, fpscr
    ldr     r6, [r2, #offsetof_arm_core_context_t_vfp_context]
    stm     r6!, {r4, r5}
    vstmia  r6!, {d0-d15}
    vstmia  r6, {d16-d31}
#endif

    /*
     * r0: ISF pointer
     * r1: pmk_core_ctrl_t
     */
c_handler:
    mov     r0, sp
    cpsid   aif, #ARM_PSR_SVC

    BL2C    arm_exception_handler

    cpsid   aif, #ARM_PSR_IRQ

/*trcpse*/
    cmp     r0, 0
    bne     virtual
/**/

restore:
    add     sp, #56                         // usr_sp
    pop     {r0-r3}

    mov     lr, r2                          // return address
    msr     spsr, r3                        // return psr

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

    sub     sp, #72
    pop     {r0-r12}
    dmb
    add     sp, #20
    b       return

/*trcpse*/
virtual:
    mov     lr, r0
    mrs     r0, spsr
    bic     r0, #(ARM_PSR_T)
    bic     r0, #(0xF)                     // return to the virtual trap table in user mode
    msr     spsr, r0

    //sub     sp, #72
    pop     {r0-r12}
    dmb
    sub     sp, #52

return:
    subs    pc, lr, #0
