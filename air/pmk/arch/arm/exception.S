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

    mov     r14, #AIR_ARM_EXCEPTION_UNDEF
    stmdb   sp, {r12, r14}
    sub     r12, sp, #8
    cpsid   aif, #ARM_PSR_IRQ

    sub     sp, #24
#if PMK_FPU_SUPPORT
    sub     sp, #264
#endif
    push    {r0-r11}
    add     r7, sp, #48

    ldm     r12, {r5, r6}
    stmia   r7!, {r5, r6}

#if PMK_FPU_SUPPORT
    add     r7, #264
#endif

    ldr     r0, [r7, #12]                   // spsr

    b       save_previous

global(exception_svc)

    srsdb   sp, #ARM_PSR_IRQ  // save return state (lr and spsr) in the IRQ stack

    mov     r14, #AIR_ARM_EXCEPTION_SWI
    stmdb   sp, {r12, r14}
    sub     r12, sp, #8
    cpsid   aif, #ARM_PSR_IRQ

    sub     sp, #24
#if PMK_FPU_SUPPORT
    sub     sp, #264
#endif

    push    {r0-r11}
    add     r7, sp, #48

    ldm     r12, {r5, r6}
    stmia   r7!, {r5, r6}

#if PMK_FPU_SUPPORT
    add     r7, #264
#endif

    ldr     r0, [r7, #12]                   // spsr
    
	b       save_previous

global(exception_pref_abort)

    sub     lr, #4
    srsdb   sp, #ARM_PSR_IRQ

    mov     r14, #AIR_ARM_EXCEPTION_PREF_ABORT
    stmdb   sp, {r12, r14}
    sub     r12, sp, #8
    cpsid   aif, #ARM_PSR_IRQ

    sub     sp, #24
#if PMK_FPU_SUPPORT
    sub     sp, #264
#endif

    push    {r0-r11}
    add     r7, sp, #48

    ldm     r12, {r5, r6}
    stmia   r7!, {r5, r6}

#if PMK_FPU_SUPPORT
    add     r7, #264
#endif

    ldr     r0, [r7, #12]                   // spsr

    b       save_previous

global(exception_data_abort)

    sub     lr, #8
    srsdb   sp, #ARM_PSR_IRQ

    mov     r14, #AIR_ARM_EXCEPTION_DATA_ABORT
    stmdb   sp, {r12, r14}
    sub     r12, sp, #8
    cpsid   aif, #ARM_PSR_IRQ

    sub     sp, #24
#if PMK_FPU_SUPPORT
    sub     sp, #264
#endif

    push    {r0-r11}
    add     r7, sp, #48

    ldm     r12, {r5, r6}
    stmia   r7!, {r5, r6}

#if PMK_FPU_SUPPORT
    add     r7, #264
#endif

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
#if PMK_FPU_SUPPORT
    sub     sp, #264
#endif

    push    {r0-r11}
    add     r7, sp, #48

    ldr     r11, [r12]
    stmia   r7!, {r11, r14}

#if PMK_FPU_SUPPORT
    add     r7, #264
#endif

    ldr     r0, [r7, #12]                   // spsr

    b       save_previous


global(exception_irq)

    sub     lr, #4

    srsdb   sp, #ARM_PSR_IRQ
    mov     r14, #AIR_ARM_EXCEPTION_IRQ

    sub     sp, #16
#if PMK_FPU_SUPPORT
    sub     sp, #264
#endif
    push    {r0-r12, r14}
    add     r7, sp, #56
#if PMK_FPU_SUPPORT
    add     r7, #264
#endif
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
    orr     r4, #(ARM_VFP_FPEXC_ENABLE)
    vmsr    fpexc, r4
    beq     c_handler
    vmrs    r5, fpscr
    add     r6, sp, #56
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

    // Store SVC SP in CORE CONTEXT (R1 points to pmk_core_ctrl_t)
    ldr     r2, [r1, #offsetof_pmk_core_ctrl_t_context]
    str     sp, [r2, #offsetof_arm_core_context_t_virt] // store svc_sp (first element of virt struct)

    // Branch into C exception Handler
    BL2C    arm_exception_handler

    // Restore SVC SP from CORE CONTEXT:
    mrc     p15, 0, r1, c13, c0, 4          // get Per_CPU core
    ldr     r1, [r1, #offsetof_pmk_core_ctrl_t_context]
    ldr     r2, [r1, #offsetof_arm_core_context_t_virt]                  // get svc sp (first element of virt struct)
    mov     sp, r2

    //Go into IRQ mode to restore the context
    cpsid   aif, #ARM_PSR_IRQ

    cmp     r0, 0 // check if there is a virtual tbr installed
    bne     virtual // if there is a virtual tbr, handle as a virtual exception


restore:
    add     sp, #56                         // usr_sp
#if PMK_FPU_SUPPORT
    add     sp, #264
#endif

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

    mrc     p15, 0, r0, c13, c0, 4          // get Per_CPU core
    ldr     r0, [r0, #offsetof_pmk_core_ctrl_t_context]
    str     sp, [r0, #offsetof_arm_core_context_t_isf_pointer]

    b       exc_return

virtual:
    mov     lr, r0                          //puts return address in lr

    add     sp, #56
#if PMK_FPU_SUPPORT
    add     sp, #264
#endif

    pop     {r0-r3}                         // usr_sp, usr_lr, ret_addr, ret_psr

    sub     r0, sp, #16
    str     lr, [r0, #8]

    mov     r6, r3
    bic     r3, #(ARM_PSR_T)
    orr     r4, r3, #(ARM_PSR_EXC_MASK)
    bic     r3, #(0xF)                      // return to the virtual trap table in user mode
    str     r3, [r0, #12]
    msr     spsr, r3                        // return psr

    and     r5, r4, #(ARM_PSR_MODE_MASK)
    cmp     r5, #(ARM_PSR_USR)              // check if previous is USR mode
    cmpne   r5, #(ARM_PSR_SYS)
    bne     exc_return
    orr     r4, #(ARM_PSR_SYS)              // if true, change to SYS

    mrs     r5, cpsr
    msr     cpsr, r4

/*  //if we don't use virtual ARM modes:
    mov     sp, r0                          //restore previous user sp
*/
    //if we use virtual ARM modes:
    mrc     p15, 0, r0, c13, c0, 4          // get Per_CPU core
    ldr     r0, [r0, #offsetof_pmk_core_ctrl_t_context]
    add     r0, r0, #offsetof_arm_core_context_t_virt
    ldr     sp, [r0, #offsetof_arm_core_pos_virt_t_sp_irq]                  // use virtual irq sp
    add     lr, r2, #0
    str     r6, [r0, #offsetof_arm_core_pos_virt_t_usr_spsr]

    msr     cpsr, r5

exc_return:
    sub     sp, #72

#if PMK_FPU_SUPPORT
    vmrs    r4, fpexc
    orr     r4, #(ARM_VFP_FPEXC_ENABLE)
    vmsr    fpexc, r4
    sub     sp,  #264
    add     r6,  sp, #56
    ldmia   r6!, {r4, r5}
    vldmia  r6!, {d0-d15}
    vldmia  r6,  {d16-d31}
    vmsr    fpscr, r5
    orr     r4, #(ARM_VFP_FPEXC_ENABLE)
    vmsr    fpexc, r4
#endif

    pop     {r0-r12}
    dmb
    //sub     sp, #52

    add     sp, #12

#if PMK_FPU_SUPPORT
    add     sp, #264
#endif

    rfe     sp!
