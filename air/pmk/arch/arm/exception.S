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

    sub     sp, #24                         // SP to point to r11 (6 * 4bytes = 24bytes)
#if PMK_FPU_SUPPORT
    sub     sp, #ARM_VFP_CONTEXT_SIZE       // FP context
#endif
    push    {r0-r11}                        // Pushes r0 to r11 into the stack
    add     r7, sp, #48                     // r7 -> r11, sp -> next context, r12 ->(r12, exception name)

    ldm     r12, {r5, r6}                   // Load what was pointed by r12 (r12, exception name) into r5 and r6
    stmia   r7!, {r5, r6}                   // Store r5 and r6 (r12, exception name) into addr pointed by r7 (IRQ stack) and r7 -> just after exception name 

#if PMK_FPU_SUPPORT
    add     r7, #ARM_VFP_CONTEXT_SIZE       // FP context
#endif

    ldr     r0, [r7, #12]                   // Save SPSR in r0 (needed for save_previous). 12 = 3*4bytes, ret_psr is 3 words after usr_spr0

    b       save_previous

global(exception_svc)

    push    {r0, r1}                        // Save the svc's r0 and r1 in stack of current mode
    mrs     r1, cpsr                        // Get cpsr to r1
    bic     r0, r1, #ARM_PSR_MODE_MASK      // r0 = r1 AND NOT #ARM_PSR_MODE_MASK 
    orr     r0, #ARM_PSR_IRQ                // Set the IRQ mode bit in r0 
    msr     cpsr, r0                        // Change into IRQ mode 

    // Cant we do cpsid aif, #ARM_PSR_IRQ ??? Instead of the above 3 instructions, we still need to save cpsr tho

svc_is_empty:
    // Must check if the sp is pointing to a vacant space (if not, search for one)
    // It's to avoid corrupting contexts that have yet to be restored

    sub     r0, sp, #sizeof_arm_interrupt_stack_frame_t // The "normal" context
#if PMK_FPU_SUPPORT
    sub     r0, #ARM_VFP_CONTEXT_SIZE       // FP context  (r0 -> start of the context)  
#endif
    ldr     r0, [r0, #52]                   // Exception name (13*4bytes = 52) 
    cmp     r0, #0                          // Check the exception name
    //beq     svc_cont //(easier to debug)
    subne   sp, #sizeof_arm_interrupt_stack_frame_t // If not 0 then there is smthg there, update the SP to try again 
#if PMK_FPU_SUPPORT
    subne   sp, #ARM_VFP_CONTEXT_SIZE       // FP context
#endif
    bne     svc_is_empty                    // If not empty then go again

    // Found an empty space, so save the svc context
    msr     cpsr, r1                        // Restore the svc cpsr (We are from now on on supervisor mode)
    pop     {r0, r1}                        // And restore original registers

    srsdb   sp, #ARM_PSR_IRQ                // Save return state (LR and SRSR) in the IRQ stack

    push    {r0,r1}                         // Save r0 and r1 in the stack
    mrs     r1, spsr                        // Get SPSR to r1   
    tst     r1, #ARM_PSR_T                  // Test for thumb execution state
    ldrneh  r0, [lr, #-2]                   // if thumb then load half word from lr-2
    bicne   r0, r0, #0xff00                 // and get "comment" field
    ldreq   r0, [lr, #-4]                   // if arm then load a word
    biceq   r0, r0, #0xff000000             // and get "comment" field

    // "Comment" field is the name used in the manual, it is the svc number
    // r0 now contains SVC number
    // r1 now contains pointer to stacked registers

    cmp     r0, #AIR_SYSCALL_ARM_RETT       // Check if it is the return syscall
    pop     {r0,r1}                         // Restore the svc's original r0 and r1

    mov     r14, #AIR_ARM_EXCEPTION_SWI     // Save in r14 exception name
    stmdb   sp, {r12, r14}                  // Save in the stack r12 and 14 (decrease the stack pointer before)
    sub     r12, sp, #8                     // r12 -> ( r12, r14(exception name)) in the supervisor stack
    cpsid   aif, #ARM_PSR_IRQ               // Change to IRQ mode and disable all interrupts

    beq     arm_rett                        // If svc is the return syscall, go to return
                                            // If not, continue to save context
    sub     sp, #24                         // SP to point to r11 (6 * 4bytes = 24bytes)
#if PMK_FPU_SUPPORT
    sub     sp, #ARM_VFP_CONTEXT_SIZE       // FP context
#endif

    push    {r0-r11}                        // Save r0 to r11 into the stack
    add     r7, sp, #48                     // r7 -> r11, sp -> next context, r12 -> (r12, exception name)

    ldm     r12, {r5, r6}                   // Load what was pointed by r12 (r12, exception name) into r5 and r6
    stmia   r7!, {r5, r6}                   // Store r5 and r6 (r12, exception name) into addr pointed by r7 (IRQ stack) and r7 -> just after exception name

#if PMK_FPU_SUPPORT
    add     r7, #ARM_VFP_CONTEXT_SIZE       // FP context
#endif

    ldr     r0, [r7, #12]                   // Save SPSR in r0 (needed for save_previous). 12 = 3*4bytes, ret_psr is 3 words after usr_sp

    b       save_previous

arm_rett:                                   // The return syscall (we are in supervisor mode)
    mov     r2, sp                          // r2 -> sp
#if PMK_FPU_SUPPORT
    add     r2, #ARM_VFP_CONTEXT_SIZE       // FP context
#endif
    ldr     r0, [r2, #68]                   // 68 = 17 * 4bytes, 17 is the number of fields in the struct with context, so (r2 + 68) -> ret_psr, r0 = ret_psr
    and     r0, #ARM_PSR_MODE_MASK          // Check the return mode
    cmp     r0, #ARM_PSR_USR                // Check USR
    cmpne   r0, #ARM_PSR_SYS                // Check SYS
    add     r4, sp, #0                      // r4 -> SP // Is this the same as mov r4, sp ?
    bne     arm_return                      // Return if return mode is not USR or SYS

    // if we use virtual ARM modes:
    mrc     p15, 0, r1, c13, c0, 4          // get Per_CPU core // Get the core number?
    ldr     r1, [r1, #offsetof_pmk_core_ctrl_t_context]
    ldr     r1, [r1, #144]                  //get virtual svc sp

/* //if we don't use virtual ARM modes:
    mrs     r0, cpsr
    mov     r1, r0
    orr     r1, #(ARM_PSR_SYS) //change to system mode
    msr     cpsr, r1
    mov     r1, sp             //get user sp
    msr     cpsr, r0
*/
    ldr     r0, [r2, #56]       //get return sp (svc) TODO: what if previous virtual mode is not svc?
    cmp     r1, r0              //if return sp is different from usr sp then we know
    beq     arm_return          //that a context switch took place in the POS IRQ handler

search_context_above:
    add     r4, #72
#if PMK_FPU_SUPPORT
    add     r4, #ARM_VFP_CONTEXT_SIZE
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
    sub     r4, #ARM_VFP_CONTEXT_SIZE
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

global(exception_pref_abort) //This is the exact same as exception_data_abort, can we make it a "function"?
    // When there is a prefetch abort LR points to the instruction at the address following the one that caused the abort, so we need to subtract 4 to get the one that caused the abort
    sub     lr, #4
    srsdb   sp, #ARM_PSR_IRQ

    mov     r14, #AIR_ARM_EXCEPTION_PREF_ABORT
    stmdb   sp, {r12, r14}
    sub     r12, sp, #8
    cpsid   aif, #ARM_PSR_IRQ

    sub     sp, #24
#if PMK_FPU_SUPPORT
    sub     sp, #ARM_VFP_CONTEXT_SIZE       // FP context
#endif

    push    {r0-r11}
    add     r7, sp, #48

    ldm     r12, {r5, r6}
    stmia   r7!, {r5, r6}

#if PMK_FPU_SUPPORT
    add     r7, #ARM_VFP_CONTEXT_SIZE
#endif

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

    sub     sp, #24                         // SP to point to r11 (6 * 4bytes = 24bytes)
#if PMK_FPU_SUPPORT
    sub     sp, #ARM_VFP_CONTEXT_SIZE       // FP context
#endif

    push    {r0-r11}                        // Now push the gen purpose regs into the IRQ stack
    add     r7, sp, #48                     // r7 -> r11, sp -> next context, r12 ->(r12, exception name)

    ldm     r12, {r5, r6}                   // Load what was pointed by r12 (r12, exception name) into r5 and r6
    stmia   r7!, {r5, r6}                   // Store r5 and r6 (r12, exception name) into addr pointed by r7 (IRQ stack) and r7 -> just after exception name 

#if PMK_FPU_SUPPORT
    add     r7, #ARM_VFP_CONTEXT_SIZE       // FP context
#endif

    ldr     r0, [r7, #12]                   // Save SPSR in r0 (needed for save_previous). 12 = 3*4bytes, ret_psr is 3 words after usr_sp

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
    sub     sp, #ARM_VFP_CONTEXT_SIZE
#endif

    push    {r0-r11}
    add     r7, sp, #48

    ldr     r11, [r12]
    stmia   r7!, {r11, r14}

#if PMK_FPU_SUPPORT
    add     r7, #ARM_VFP_CONTEXT_SIZE
#endif

    ldr     r0, [r7, #12]                   // spsr

    b       save_previous


global(exception_irq)

    sub     lr, #4                          // Save the instruction before the exception occured

    cpsid   aif, #ARM_PSR_FIQ               // Change to FIQ mode, since the FIQ stack is not being used.
    push    {r0-r3}                         // Store r0-r3 in the FIQ stack to not corrupt the IRQ stack
    cpsid   aif, #ARM_PSR_IRQ               // Go back to IRQ mode

    // Check if the previous mode is USR or SYS.
    mrs     r0, spsr
    and     r0, #ARM_PSR_MODE_MASK
    cmp     r0, #ARM_PSR_USR
    cmpne   r0, #ARM_PSR_SYS
    bne     irq_cont                        // If not, continue

    // If previous mode was USR or SYS check if this context has already been stored. If so, replace it. (Why?)
    mrs     r0, cpsr                        // Get CPSR to r0
    mov     r1, r0                          // Copy CPSR to r1
    orr     r1, #ARM_PSR_SYS                // Set system mode
    msr     cpsr, r1                        // Change to system mode
    mov     r1, sp                          // Get user sp (in system mode SP is the same as the user SP )
    msr     cpsr, r0                        // Change back to previous mode
                                            // Why did we do this?
    mov     r3, sp                          // r3 -> sp

    add     r3, #72                         // r3 -> context
#if PMK_FPU_SUPPORT
    add     r3, #ARM_VFP_CONTEXT_SIZE       // FP context
#endif

find_context:
    ldr     r0, [r3, #52]                   // Get exception name
    cmp     r0, #AIR_ARM_EXCEPTION_IRQ      // Check if it is IRQ?
    bgt     irq_cont
    cmpne   r0, 0                           // If not IRQ check if this context is empty?
    addne   r3, 72                          // If neither, keep looking in the contexts above

#if PMK_FPU_SUPPORT
    add     r3, #ARM_VFP_CONTEXT_SIZE       // FP context
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

   /*--Now we can save the irq context--*/

    srsdb   sp, #ARM_PSR_IRQ
    mov     r14, #AIR_ARM_EXCEPTION_IRQ

    sub     sp, #16
#if PMK_FPU_SUPPORT
    sub     sp, #ARM_VFP_CONTEXT_SIZE
#endif
    push    {r0-r12, r14}
    add     r7, sp, #56
#if PMK_FPU_SUPPORT
    add     r7, #ARM_VFP_CONTEXT_SIZE
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

    BL2C    arm_exception_handler

    cpsid   aif, #ARM_PSR_IRQ

    cmp     r0, 0
    bne     virtual


restore:
    add     sp, #56                         // usr_sp
#if PMK_FPU_SUPPORT
    add     sp, #ARM_VFP_CONTEXT_SIZE
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
    sub     sp,  #ARM_VFP_CONTEXT_SIZE
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
    add     sp, #ARM_VFP_CONTEXT_SIZE
#endif

    rfe     sp!

virtual:
    mov     lr, r0                          //puts return address in lr

    add     sp, #56
#if PMK_FPU_SUPPORT
    add     sp, #ARM_VFP_CONTEXT_SIZE
#endif

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

    msr     cpsr, r5

virtual_return:
    sub     sp, #72

#if PMK_FPU_SUPPORT
    vmrs    r4, fpexc
    orr     r4, #(ARM_VFP_FPEXC_ENABLE)
    vmsr    fpexc, r4
    sub     sp,  #ARM_VFP_CONTEXT_SIZE
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
    sub     sp, #52

    subs    pc, lr, #0
