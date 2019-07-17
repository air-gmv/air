/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file context_switch.S
 * @author lumm
 * @brief Contains the routines to peform a partition core context save
 *        and partition core context restore on a ARMv7
 */

#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <asm_offsets.h>

/**
 * @brief Partition context save
 * @params r0   pmk_core_ctrl_t
 *
 *  r1  pmk_core_ctrl_t->arm_core_context_t
 *  r2  pmk_partition_t
 *  r3  pmk_partition_t->arm_core_context_t
 */

FUNC(arm_core_context_save)
    push    {r1-r12}

    ldr     r1, [r0, #(offsetof_pmk_core_ctrl_t_context)]
    ldr     r2, [r0, #(offsetof_pmk_core_ctrl_t_partition)]
    ldr     r3, [r2, #(offsetof_pmk_partition_t_context)]

    // TODO SOMETHING ABOUT MMUs

#ifdef PMK_FPU_SUPPORT
    /* Ensure that the FPU is enabled */
    vmrs    r4, FPEXC
    tst     r4, #(1 << 30)
    beq     1f

    /* Save VFP context */
    ldr     r0, [r3, #(offsetof_arm_core_context_t_fpu_context)]

    vmrs    r5, FPSCR
    stmia   r0!, {r4-r5}
    vstmia  r0!, {d0-d15}
#ifdef VFP_D32
    vstmia  r0!, {d16-d31}
#else
    mov     r4, #0
    mov     r5, #0
    mov     r6, #0
    mov     r7, #0
    mov     r8, #0
    mov     r9, #0
    mov     r10, #0
    mov     r11, #0
    add     r12, r0, #128
2:
    stmia   r0!, {r4-r11}
    cmp     r0, r12
    bne     2b
#endif /* VFP_D32 */
1:
#endif /* PMK_FPU_SUPPORT */

    /* Change what is in the core context to the partition context */
    ldr     r0, [r1, #(offsetof_arm_core_context_t_isf_pointer)]
    ldr     r1, [r3, #(offsetof_arm_core_context_t_isf_pointer)]

    ldm     r0!, {r2-r12}
    stm     r1!, {r2-r12}
    ldm     r0!, {r2-r8}
    stm     r1!, {r2-r8}

    pop     {r1-r12}
    b       lr



FUNC(arm_core_context_restore)
    push    {r1-r12}

    ldr     r1, [r0, #(offsetof_pmk_core_ctrl_t_context)]
    ldr     r2, [r0, #(offsetof_pmk_core_ctrl_t_partition)]
    ldr     r3, [r2, #(offsetof_pmk_partition_t_context)]

    // TODO SOMETHING ABOUT MMUs

#ifdef PMK_FPU_SUPPORT
    /* Load VFP context */
    ldr     r0, [r3, #(offsetof_arm_core_context_t_fpu_context)]

    ldm     r0!, {r4-r5}

    /* Ensure that the FPU is enabled */
    vmsr    FPEXC, r4
    vmsr    FPSCR, r5
    tst     r4, #(1 << 30)
    beq     1f

    vldmia  r0!, {d0-d15}
#ifdef VFP_D32
    vldmia  r0!, {d16-d31}
#endif /* VFP_D32 */
1:
#endif /* PMK_FPU_SUPPORT */

    /* Change what is in the core context to the partition context */
    ldr     r0, [r1, #(offsetof_arm_core_context_t_isf_pointer)]
    ldr     r1, [r3, #(offsetof_arm_core_context_t_isf_pointer)]

    ldm     r1!, {r2-r12}
    stm     r0!, {r2-r12}
    ldm     r1!, {r2-r8}
    stm     r0!, {r2-r8}

    // TODO SOMETHING ABOUT IRQs

    pop     {r1-r12}
    b       lr
