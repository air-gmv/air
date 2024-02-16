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
/*
    push    {r0, r1}
    mrs     r1, cpsr
    bic     r0, r1, #ARM_PSR_MODE_MASK
    orr     r0, #ARM_PSR_IRQ
    msr     cpsr, r0     // change into IRQ mode to check the contexts in the IRQ stack


svc_is_empty:
// check if the sp is pointing to a vacant space (if not, search for one)
// to avoid corrupting contexts that have yet to be restored
    sub     r0, sp, #72
#if PMK_FPU_SUPPORT
    sub     r0, #264
#endif
    ldr     r0, [r0, #52]                   //exception name
    cmp     r0, #0
    //beq     svc_cont //(easier to debug)
    subne   sp, #72
#if PMK_FPU_SUPPORT
    subne   sp, #264
#endif
    bne     svc_is_empty

//svc_cont: //(easier to debug)
    msr     cpsr, r1     //restore the svc cpsr

    pop     {r0, r1}    //restore original registers

 */   /*--Now we can save the svc context--*/

    srsdb   sp, #ARM_PSR_IRQ  // save return state (lr and spsr) in the IRQ stack
/*
    push    {r0,r1}
    mrs     r1, spsr
    tst     r1, #ARM_PSR_T
    ldrneh  r0, [lr, #-2]           //thumb
    bicne   r0, r0, #0xff00
    ldreq   r0, [lr, #-4]           //arm
    biceq   r0, r0, #0xff000000

    cmp     r0, #(AIR_SYSCALL_ARM_RETT)  /* Checks if svc is air_syscall_rett
    pop     {r0,r1}*/

    mov     r14, #AIR_ARM_EXCEPTION_SWI
    stmdb   sp, {r12, r14}
    sub     r12, sp, #8
    cpsid   aif, #ARM_PSR_IRQ

    //beq     arm_rett    // if svc is the return syscall, we can skip storing the context

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

arm_rett:
    mov     r2, sp
#if PMK_FPU_SUPPORT
    add     r2, 264
#endif
    ldr     r0, [r2, #68]
    and     r0, #(ARM_PSR_MODE_MASK)
    cmp     r0, #(ARM_PSR_USR)      //context switch happens in user or system mode
    cmpne   r0, #(ARM_PSR_SYS)
    add     r4, sp, #0
/*    bne     arm_return

    // if we use virtual ARM modes:
    mrc     p15, 0, r1, c13, c0, 4          // get Per_CPU core
    ldr     r1, [r1, #offsetof_pmk_core_ctrl_t_context]
    ldr     r1, [r1, #144]                  //get virtual svc sp

/* //if we don't use virtual ARM modes:
    mrs     r0, cpsr
    mov     r1, r0
    orr     r1, #(ARM_PSR_SYS) //change to system mode
    msr     cpsr, r1
    mov     r1, sp             //get user sp
    msr     cpsr, r0

    ldr     r0, [r2, #56]       //get return sp (svc) TODO: what if previous virtual mode is not svc?
    cmp     r1, r0              //if return sp is different from usr sp then we know
    beq     arm_return          //that a context switch took place in the POS IRQ handler

search_context_above:
    add     r4, #72
#if PMK_FPU_SUPPORT
    add     r4, #264
    ldr     r0, [r4, #320]
#else
    ldr     r0, [r4, #56]
#endif
    cmp     r0, #0
    addeq   r4, sp, #0
    beq     search_context_below
    ldr     r2, [r4, #52]
    cmp     r2, #AIR_ARM_EXCEPTION_IRQ
    bne     search_context_above
    cmp     r0, r1
    bne     search_context_above
    b       arm_return

search_context_below:
    sub     r4, #72
#if PMK_FPU_SUPPORT
    sub     r4, #264
    ldr     r0, [r4, #320]
#else
    ldr     r0, [r4, #56]
#endif
    cmp     r0, #0
    moveq   r4, sp
    beq     arm_return   // No context found. Restore previous.
    ldr     r2, [r4, #52]
    cmp     r2, #AIR_ARM_EXCEPTION_IRQ
    bne     search_context_below
    cmp     r0, r1
    bne     search_context_below

*/
arm_return:
    mrc     p15, 0, r0, c13, c0, 4  // get Per_CPU core
    BL2C    arm_syscall_rett
    mov     sp, r4
    b       restore
    //TODO: check if there are pending interrupts before returning to the partition!

/*no_context:                       //for debugging
    mov     r0, r1
    mov     r1, sp
    BL2C    print_nocontext
    b       arm_return*/

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
/*
    cpsid   aif, #ARM_PSR_FIQ //since the FIQ stack is not being used,
    push    {r0-r3}          //store these values there so as to not corrupt the IRQ stack
    cpsid   aif, #ARM_PSR_IRQ

// check if the previous mode is USR or SYS.
    mrs     r0, spsr
    and     r0, #(ARM_PSR_MODE_MASK)
    cmp     r0, #(ARM_PSR_USR)
    cmpne   r0, #(ARM_PSR_SYS)
    bne     irq_cont //if not, continue without changing SP

// if it is, check if this context has already been stored. If so, replace it
    mrs     r0, cpsr
    mov     r1, r0
    orr     r1, #(ARM_PSR_SYS) //change to system mode
    msr     cpsr, r1
    mov     r1, sp             //get user sp
    msr     cpsr, r0

    mov     r3, sp

    add     r3, #72
#if PMK_FPU_SUPPORT
    add     r3, #264
#endif

find_context:
    ldr     r0, [r3, #52]       //get exception name
    cmp     r0, #AIR_ARM_EXCEPTION_IRQ  // is IRQ?
    bgt     irq_cont
    cmpne   r0, 0               // is context empty?
    addne   r3, 72              // if neither, keep looking in the contexts above

#if PMK_FPU_SUPPORT
    add     r3, #264
#endif

    bne     find_context

    ldr     r0, [r3, #56]
    cmp     r0, 0              //if we've reached an empty context, stop looking
    beq     irq_cont           //continue without changing the SP
    cmp     r1, r0
    add     r3, 72
    bne     find_context
    mov     sp, r3            //context found! move the SP to the new position


irq_cont:
    cpsid   aif, #ARM_PSR_FIQ
    pop     {r0-r3}
    cpsid   aif, #ARM_PSR_IRQ

 */  /*--Now we can save the irq context--*/

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
    str     sp, [r2, #152]

    // Branch into C exception Handler
    BL2C    arm_exception_handler

    // Restore SVC SP from CORE CONTEXT:
    mrc     p15, 0, r1, c13, c0, 4          // get Per_CPU core
    ldr     r1, [r1, #offsetof_pmk_core_ctrl_t_context]
    ldr     r2, [r1, #152]                  // get svc sp
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

    mov   r0, 0
    str   r0, [sp, #52]

    pop     {r0-r12}
    dmb
    add     sp, #12

#if PMK_FPU_SUPPORT
    add     sp, #264
#endif

    rfe     sp!

virtual:
    mov     lr, r0                          //puts return address in lr

    add     sp, #56
#if PMK_FPU_SUPPORT
    add     sp, #264
#endif

    mrs     r0, spsr
    str     r0, [r1, #164]

    pop     {r0-r3}                         // usr_sp, usr_lr, ret_addr, ret_psr

    bic     r3, #(ARM_PSR_T)
    orr     r4, r3, #(ARM_PSR_EXC_MASK)
    bic     r3, #(0xF)                      // return to the virtual trap table in user mode
    msr     spsr, r3                        // return psr

    and     r5, r4, #(ARM_PSR_MODE_MASK)
    cmp     r5, #(ARM_PSR_USR)              // check if previous is USR mode
    cmpne   r5, #(ARM_PSR_SYS)
    bne     virtual_return
    orr     r4, #(ARM_PSR_SYS)              // if true, change to SYS

    mrs     r5, cpsr
    msr     cpsr, r4

/*  //if we don't use virtual ARM modes:
    mov     sp, r0                          //restore previous user sp
*/
    //if we use virtual ARM modes:
    mrc     p15, 0, r0, c13, c0, 4          // get Per_CPU core
    ldr     r0, [r0, #offsetof_pmk_core_ctrl_t_context]
    ldr     sp, [r0, #148]                  // use virtual irq sp
    add     lr, r2, #0

    msr     cpsr, r5

virtual_return:
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

        add     sp, #20

#if PMK_FPU_SUPPORT
    add     sp, #264
#endif

    subs    pc, lr, #0
