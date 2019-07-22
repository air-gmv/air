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

    .arm
global(bare_start)

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
    SWITCH_FROM_ARM_TO_THUMB    r4
    bl      arm_pos_init
    SWITCH_FROM_THUMB_TO_ARM

2:
    /* setup stack. already done in AIR */
//  ldr     sp, =stack_start

    /* exception vector table ?*/

    /* enable irqs */
    svc     #(AIR_SYSCALL_ARM_ENABLE_TRAPS)

    /* enable fpu */
    svc     #(AIR_SYSCALL_ARM_ENABLE_FPU)

//  cpsie   aif, #ARM_PSR_USR

    SWITCH_FROM_ARM_TO_THUMB    r4
    bl      main
    SWITCH_FROM_THUMB_TO_ARM

    mov     r1, #(AIR_MODE_IDLE)
    mov     r0, #(0xffffffff)
    svc     #(AIR_SYSCALL_SET_PARTITION_MODE)

    b       twiddle


global(pos_exceptions)
    b       bare_start
    b       pos_undef
    nop
    b       pos_pref_abort
    b       pos_data_abort
    nop
    nop
    nop

pos_undef:
    push    {r0-r4, lr}

    SWITCH_FROM_ARM_TO_THUMB    r4
    bl      pos_hm_undef
    SWITCH_FROM_THUMB_TO_ARM

    pop     {r0-r4, lr}
    svc     #(AIR_SYSCALL_ARM_RETT)

pos_pref_abort:
    push    {r0-r4, lr}

    SWITCH_FROM_ARM_TO_THUMB    r4
    bl      pos_hm_pref_abort
    SWITCH_FROM_THUMB_TO_ARM

    pop     {r0-r4, lr}
    svc     #(AIR_SYSCALL_ARM_RETT)

pos_data_abort:
    push    {r0-r4, lr}

    SWITCH_FROM_ARM_TO_THUMB    r4
    bl      pos_hm_data_abort
    SWITCH_FROM_THUMB_TO_ARM

    pop     {r0-r4, lr}
    svc     #(AIR_SYSCALL_ARM_RETT)



    /* idle */
twiddle:
    wfi
    b       twiddle
