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

/* Define we are in assembly file */
#ifndef ASM
#define ASM
#endif

#include <asm.h>

    .arm
    /**< Start entry */
ENTRY(_start)

    /* Some boards require register initialization */

    /* Save r1 and r2 to preserve bootloader params */
    mov     r5, r1
    mov     r6, r2

#ifdef PMK_SMP
    /* Reading the MPIDR into r7. Not available before ARMv7 */
    mrc     p15, 0, r7, c0, c0, 5
    and     r7, #0xff
//  cmp     r7, #0 /* 0 if single core */
//
//#if PMK_MAX_CORES > 1
//  bne     stack_setup
//#else
//  /* Only continue for ID = 0 */
//  bne     wait_for_event
//#endif

    /* Some boards might support fdt */

    /* Some CPUs start in hypervisor mode */

    /**< Calculate stack offset */
    ldr     r1, =bsp_stack_size /* TODO need to compute this somewhere */
    mul     r1, r7
#endif /* PMK_SMP */

modes:
    mrs     r4, cpsr /* save original processor status value */

    /**< Set SVC mode */
    mov     r0, #(PSR_SVC | PSR_I | PSR_F)
    msr     cpsr, r0
    ldr     sp, =bsp_stack_svc_end
#ifdef PMK_SMP
    add     sp, r1
#endif

    /**< Set FIQ mode */
    mov     r0, #(PSR_FIQ | PSR_I | PSR_F)
    msr     cpsr, r0
    ldr     sp, =bsp_stack_fiq_end
#ifdef PMK_SMP
    add     sp, r1
#endif

    /* some boards require fiq registers initialization */

    /**< Set IRQ mode */
    mov     r0, #(PSR_IRQ | PSR_I | PSR_F)
    msr     cpsr, r0
    ldr     sp, =bsp_stack_irq_end
#ifdef PMK_SMP
    add     sp, r1
#endif

    /**< Set ABT mode */
    mov     r0, #(PSR_ABT | PSR_I | PSR_F)
    msr     cpsr, r0
    ldr,    =bsp_stack_abt_end
#ifdef PMK_SMP
    add     sp, r1
#endif

    /**< Set UND mode */
    mov     r0, #(PSR_UND | PSR_I | PSR_F)
    msr     cpsr, r0
    ldr,    =bsp_stack_abt_end
#ifdef PMK_SMP
    add     sp, r1
#endif

    /**< Enable VFPv3. Other versions might need additional setup */
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

    mov     r0, #(1 << 30)
    vmsr    FPEXC, r0

    /* some boards require vfp registers initialization */
#endif /* PMK_FPU_SUPPORT */

    ldr     lr, =bsp_start_hook_0_done
    mov     r0, r4 /* original cpsr */
    mov     r1, r5 /* machine type number or ~0 for DT boot */
    mov     r2, r6 /* physical address of ATAGs or DTB */

    b       bsp_start_hook_0
