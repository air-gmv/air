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
 * @brief Basic bare OS start.S
 */

#ifndef ASM
#define ASM
#endif

#include <air.h>
#include <asm.h>
#include <armv7.h>


FUNC(bare_start)

    /* get core id */
    svc     #(AIR_SYSCALL_GET_CORE_ID)

    /* clear BSS if id 0 */
    tst     r0, #0
    bne     2f

    /* clear BSS */
    ldr     r2, =bss_start
    ldr     r3, =bss_end
    mov     r0, #0
    mov     r1, #0
1:
    stm     r2!, {r0,r1}
    cmp     r2, r3
    bls     1b

    /* todo SMP */


2:
    /* setup stack */
    ldr     sp, =stack_start

    /* exception vector table ?*/

    /* enable irqs */
    svc     #(AIR_SYSCALL_ARM_ENABLE_TRAPS)

    /* enable fpu */
    svc     #(AIR_SYSCALL_ARM_ENABLE_FPU)

    bl      entry_point

    mov     r1, #(AIR_MODE_IDLE)
    mov     r0, #(0xffffffff)
    svc     #(AIR_SYSCALL_SET_PARTITION_MODE)

    /* idle */
    wfe
