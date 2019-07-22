/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file start.S
 * @author lumm
 * @brief Boot code for ARM.
 */

/* Are we in assembly file. yes we are */
#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <armv7.h>
#include <cpu.h>
#include <bsp.h>

#if __ARM_ARCH >= 7
    .arm

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

    /* Calculate stack offset */
    mov     r1, #total_stack_size

#ifdef PMK_SMP
    /* Reading the MPIDR into r7. Not available before ARMv7 */
    mrc     p15, 0, r7, c0, c0, 5
    and     r7, #0xff

    /* TODO Save per cpu info */

    /* TODO We might add fdt */

    /* TODO Some CPUs start in hypervisor mode */
    add     r7, #1
    mul     r1, r7
#endif /* PMK_SMP */

    ldr     r2, =air_stack
    add     r7, r1, r2

    /**< Setup processor modes */
setup_stack:
    mrs     r4, cpsr /* save original processor status value */

    /**< Set SVC mode */
    cpsid   if, #(ARM_PSR_SVC)
    ldr     sp, =svc_stack

    /**< Set FIQ mode */
    cpsid   aif, #(ARM_PSR_FIQ)
    ldr     sp, =fiq_stack

    /* TODO some boards require fiq registers initialization */

    /**< Set IRQ mode */
    cpsid   aif, #(ARM_PSR_IRQ)
    ldr     sp, =irq_stack

    /**< Set ABT mode */
    cpsid   aif, #(ARM_PSR_ABT)
    ldr     sp, =abt_stack

    /**< Set UND mode */
    cpsid   aif, #(ARM_PSR_UND)
    ldr     sp, =und_stack

    /**< Set SYS mode */
    cpsie   aif, #(ARM_PSR_SYS)
    ldr     sp, =sys_stack

    /**< Set MON mode */
    cpsid   aif, #(ARM_PSR_MON)
    ldr     sp, =mon_stack

    /**< Change back to SVC mode */
    cpsie   aif, #(ARM_PSR_SVC)

    /**< Enable VFPv3. TODO Other versions might need additional setup */
#ifdef PMK_FPU_SUPPORT
    /* read CPACR */
    mrc     p15, 0, r0, c1, c0, 2
    /* enable CP10 and CP11 */
    orr     r0, r0, #(1 << 20)
    orr     r0, r0, #(1 << 22)
    /* clear ASEDIS and D32DIS. writes to D32DIS ignored for VFP-D16 */
    bic     r0, r0, #(3 << 30)
    /* write CPACR */
    mcr     p15, 0, r0, c1, c0, 2

    /* TODO set C10 and C11 through NSACR (security extensions) */

    isb

    /* Enable FPU */
    mov     r0, #(1 << 30)
    vmsr    FPEXC, r0

    /* TODO some boards require vfp registers initialization */
#endif /* PMK_FPU_SUPPORT */

    ldr     lr, =bsp_start_hook_done
#ifdef __thumb__
    orr     lr, #1
#endif

    SWITCH_FROM_ARM_TO_THUMB    r0
//  mov     r0, r4 /* original cpsr */
//  mov     r1, r5 /* machine type number or ~0 for DT boot */
//  mov     r2, r6 /* physical address of ATAGs or DTB */

    bl      bsp_start_hook

bsp_start_hook_done:

    SWITCH_FROM_THUMB_TO_ARM

    /* Reading the MPIDR into r8. Not available before ARMv7 */
    mrc     p15, 0, r8, c0, c0, 5
    and     r8, #0xff
    cmp     r8, #0

    mov     r0, #0
    bne     secondary_core_init

    SWITCH_FROM_ARM_TO_THUMB    r3
    bl      pmk_init

    /**< Secondary cores init */
global(secondary_core_init)

    bl      pmk_core_initialize

    /* never returns here */

#endif /* __ARM_ARCH >= 7 */
