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

/* Are we in assembly file */
#ifdef ASM

#include <asm.h>
#include <bsp.h>

    /* External symbols */
    .extern stack_svc_end
    .extern stack_fiq_end
    .extern stack_irq_end
    .extern stack_abt_end
    .extern stack_und_end
#ifdef PMK_SMP
    .extern stack_total_size
#endif
    .extern bsp_start_hook
    .extern pmk_init

    .extern exception_undef
    .extern exception_swi
    .extern exception_pref_abort
    .extern exception_data_abort
    .extern exception_irq_interrupt
    .extern exception_fiq_interrupt

    .globl vector_table_begin
    .globl vector_table_end
    .globl vector_table_size
    .globl start_hook_done

    .section ".start_text", "ax"

#if __ARM_ARCH >= 7

    .arm
vector_table_begin:
    ldr     pc, handler_addr_reset
    ldr     pc, handler_addr_undef
    ldr     pc, handler_addr_swi
    ldr     pc, handler_addr_pref_abort
    ldr     pc, handler_addr_data_abort

    .word   0x0 /* unused. Seems important for LPC2106 */

    ldr     pc, handler_addr_irq
    ldr     pc, handler_addr_fiq

handler_addr_undef:
    .word   exception_undef

handler_addr_swi:
    .word   exception_swi

handler_addr_pref_abort:
    .word   exception_pref_abort

handler_addr_data_abort:
    .word   exception_data_abort

handler_addr_irq:
    .word   exception_irq_interrupt

handler_addr_fiq:
    .word   exception_fiq_interrupt

bsp_start_vector_table_end:


    /**< Start entry */
ENTRY(_start)
    /* TODO Some boards require register initialization */

    /* Save r1 and r2 to preserve bootloader params */
    mov     r5, r1
    mov     r6, r2

#ifdef PMK_SMP
    /* Reading the MPIDR into r7. Not available before ARMv7 */
    mrc     p15, 0, r7, c0, c0, 5
    and     r7, #0xff

    /* TODO Save per cpu info */

    /* TODO We might add fdt */

    /* TODO Some CPUs start in hypervisor mode */

    /**< Calculate stack offset */
    ldr     r1, =stack_total_size
    mul     r1, r7
#endif /* PMK_SMP */

setup_stack:
    mrs     r4, cpsr /* save original processor status value */

    /**< Set SVC mode */
    mov     r0, #(ARM_PSR_SVC | ARM_PSR_I | ARM_PSR_F)
    msr     cpsr, r0
    ldr     sp, =stack_svc_end
#ifdef PMK_SMP
    add     sp, r1
#endif

    /**< Set FIQ mode */
    mov     r0, #(ARM_PSR_FIQ | ARM_PSR_I | ARM_PSR_F)
    msr     cpsr, r0
    ldr     sp, =stack_fiq_end
#ifdef PMK_SMP
    add     sp, r1
#endif

    /* TODO some boards require fiq registers initialization */

    /**< Set IRQ mode */
    mov     r0, #(ARM_PSR_IRQ | ARM_PSR_I | ARM_PSR_F)
    msr     cpsr, r0
    ldr     sp, =stack_irq_end
#ifdef PMK_SMP
    add     sp, r1
#endif

    /**< Set ABT mode */
    mov     r0, #(ARM_PSR_ABT | ARM_PSR_I | ARM_PSR_F)
    msr     cpsr, r0
    ldr     sp, =stack_abt_end
#ifdef PMK_SMP
    add     sp, r1
#endif

    /**< Set UND mode */
    mov     r0, #(ARM_PSR_UND | ARM_PSR_I | ARM_PSR_F)
    msr     cpsr, r0
    ldr     sp, =stack_und_end
#ifdef PMK_SMP
    add     sp, r1
#endif

    /**< Change back to SVC mode */
    mov     r0, #(ARM_PSR_SVC | ARM_PSR_I | ARM_PSR_F)
    msr     cpsr, r0

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
    mov     r0, =vector_table_begin

    bl      bsp_start_hook

bsp_start_hook_done:

    SWITCH_FROM_ARM_TO_THUMB    r3
    mov     r0, #0
    bl      pmk_init

    /* never returns here */

#endif /* __ARM_ARCH >= 7 */
#endif /* ASM */
