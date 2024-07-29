/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file start.S
 * \author lumm
 * \brief Boot code for ARM.
 */

/* Are we in assembly file. yes we are */
#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <asm_offsets.h>
#include <armv7.h>
#include <cpu.h>

    .extern exception_undef
    .extern exception_svc
    .extern exception_pref_abort
    .extern exception_data_abort
    .extern exception_irq
    .extern exception_fiq

#if __ARM_ARCH >= 7
    .arm
    .syntax unified

    /**< Start entry */
global(pmk_trap_table)
    b       reset
    b       exception_undef
    b       exception_svc
    b       exception_pref_abort
    b       exception_data_abort
    nop
    b       exception_irq
    b       exception_fiq

#ifdef __security_extensions__
global(monitor_exceptions)
    nop
    nop
    b       monitor_smc
    b       monitor_pref_abort
    b       monitor_data_abort
    nop
    b       monitor_irq
    b       monitor_fiq

global(nonsecure_exceptions)
    nop
    b       nonsecure_undef
    b       nonsecure_svc
    b       nonsecure_pref_abort
    b       nonsecure_data_abort
    nop
    b       nonsecure_irq
    b       nonsecure_fiq

#ifdef __virtualization_extensions__
global(hyp_exceptions)
    nop
    b       hyp_undef
    b       hyp_hvc
    b       hyp_pref_abort
    b       hyp_data_abort
    b       hyp_trap
    b       hyp_irq
    b       hyp_fiq
#endif /* __virtualization_extensions__ */
#endif /* __security_extensions__ */

global(start)
global(_start)
reset:
    /* TODO Some boards require register initialization */

    /* Save r1 and r2 to preserve bootloader params */
    mov     r5, r1
    mov     r6, r2

#if PMK_SMP
    /* Reading the MPIDR into r7. Not available before ARMv7 */
    mrc     p15, 0, r7, c0, c0, 5
    and     r7, #0xff

    /* TODO We might add fdt */

    /* TODO Some CPUs start in hypervisor mode */
#else
    mov     r7, #0
#endif /* PMK_SMP */

    cmp     r7, #0

#ifdef PMK_ARM_QEMU
    bne     qemu_reset_state
#else 
    bne     setup_stack
#endif

clear_bss:
    ldr     r1, =air_kernel_bss_start
    ldr     r2, =air_kernel_bss_end
    mov     r3, #0
    mov     r4, #0
1:
    cmp     r2, r1
    bls     setup_stack
    stmia   r1!, {r3, r4}
    b       1b

    /**< Setup processor modes */
setup_stack:
    /* Calculate stack offset */
    ldr     r1, =air_stack
    mov     r2, #stack_size
    
    //add     r7, r1, r2, lsl r7 // r7 = &air_stack + stack_size * 2^cpu_number
    add     r3, r7, #1 // r3 = cpu_number + 1
    mul     r2, r2, r3 // r2 = stack_size * (cpu_number + 1)
    add     r7, r1, r2 // r7 = &air_stack + stack_size * (cpu_number + 1)

    mrs     r4, cpsr /* save original processor status value */

    /**< Set SVC mode */
    cpsid   aif, #(ARM_PSR_SVC)
    mov     sp, r7
    sub     r7, #svc_stack_size

    /**< Set FIQ mode */
    cpsid   if, #(ARM_PSR_FIQ)
    mov     sp, r7
    sub     r7, #fiq_stack_size

    /* TODO some boards require fiq registers initialization */

    /**< Set IRQ mode */
    cpsid   if, #(ARM_PSR_IRQ)
    mov     sp, r7
    sub     r7, #irq_stack_size

    /**< Set ABT mode */
    cpsid   if, #(ARM_PSR_ABT)
    mov     sp, r7
    sub     r7, #abt_stack_size

    /**< Set UND mode */
    cpsid   if, #(ARM_PSR_UND)
    mov     sp, r7
    sub     r7, #und_stack_size

    /**< Set SYS mode */
    cpsid   aif, #(ARM_PSR_SYS)
    mov     sp, r7
    sub     r7, #sys_stack_size

    /**< Set MON mode */
    cpsid   if, #(ARM_PSR_MON)
    mov     sp, r7
    sub     r7, #mon_stack_size

    /**< Change back to SVC mode */
    cpsid   aif, #(ARM_PSR_SVC)

    /**< Enable VFPv3. TODO Other versions might need additional setup */
#if PMK_FPU_SUPPORT
    /* read CPACR */
    mrc     p15, 0, r8, c1, c0, 2
    /* enable CP10 and CP11 */
//  orr     r8, #(1 << 20)
//  orr     r8, #(1 << 22)
    orr     r8, #(0b0101 << 20)
    /* clear ASEDIS and D32DIS. writes to D32DIS ignored for VFP-D16 */
    bic     r8, #(3 << 30)
    /* write CPACR */
    mcr     p15, 0, r8, c1, c0, 2

    /* TODO set C10 and C11 through NSACR (security extensions) */

    isb

    /* Enable FPU */
    mov     r8, #(1 << 30)
    vmsr    fpexc, r8

    /* TODO some boards require vfp registers initialization */
#endif /* PMK_FPU_SUPPORT */

//  mov     r0, r4 /* original cpsr */
//  mov     r1, r5 /* machine type number or ~0 for DT boot */
//  mov     r2, r6 /* physical address of ATAGs or DTB */
    BL2C    bsp_start_hook

    /* Reading the MPIDR into r8. Not available before ARMv7 */
    mrc     p15, 0, r8, c0, c0, 5
    and     r8, #0xff
    cmp     r8, #0

    mov     r0, #0
    bne     secondary_core_init

    /**< Core 0 init */
    BL2C    pmk_init
    b       twiddle

    /**< Secondary cores init */
secondary_core_init:
    BL2C    pmk_core_initialize

    /* never returns here */
twiddle:
    bl      bsp_restart_core
    b       twiddle

qemu_reset_state:           // Emulate secondary cores behavior 
    isb sy                  // Memory barriers to ensure data is good
    dsb sy
    dmb sy
    mov r0, #0xfffffff0     // r0 -> 0xfffffff0
    ldr r1,[r0]             // Read addr #0xfffffff0 to r1
    isb sy                  // Memory barriers to ensure data is good
    dsb sy
    dmb sy
    cmp r1, #0              // Check if value in memory is still 0
    bne setup_stack         // If not 0 then continue booting process
    b  qemu_reset_state     // If it is still 0 continue loop
#endif /* __ARM_ARCH >= 7 */
