/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013 - 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file
 * @author lumm
 * @brief ARM system calls
 */

/* enforce ASM definition */
#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <air.h>

    .thumb
global(air_disable_interrupts)
    svc     #(AIR_SYSCALL_ARM_DISABLE_INTERRUPTS)
    bx      lr

global(air_enable_interrupts)
    svc     #(AIR_SYSCALL_ARM_ENABLE_INTERRUPTS)
    bx      lr

global(air_disable_traps)
    svc     #(AIR_SYSCALL_ARM_DISABLE_TRAPS)
    bx      lr

global(air_enable_traps)
    svc     #(AIR_SYSCALL_ARM_ENABLE_TRAPS)
    bx      lr

global(air_disable_fpu)
    svc     #(AIR_SYSCALL_ARM_DISABLE_FPU)
    bx      lr

global(air_enable_fpu)
    svc     #(AIR_SYSCALL_ARM_ENABLE_FPU)
    bx      lr

global(air_get_tbr)
    svc     #(AIR_SYSCALL_ARM_GET_TBR)
    bx      lr

global(air_set_tbr)
    svc     #(AIR_SYSCALL_ARM_SET_TBR)
    bx      lr

global(air_get_psr)
    svc     #(AIR_SYSCALL_ARM_GET_PSR)
    bx      lr

global(air_set_psr)
    svc     #(AIR_SYSCALL_ARM_SET_PSR)
    bx      lr

/* global(????)
    svc     #(AIR_SYSCALL_ARM_RETT)
    bx      lr */

//global(air_get_cache_register)
//  svc     #(AIR_SYSCALL_ARM_GET_CACHE_REGISTER)

//global(air_set_cache_register)
//  svc     #(AIR_SYSCALL_ARM_SET_CACHE_REGISTER)

/*global(????)
    svc     #(AIR_SYSCALL_ARM_RESTORE_CACHE_REGISTER)
    bx      lr */

global(air_get_irq_mask_register)
    svc     #(AIR_SYSCALL_ARM_GET_IRQ_MASK_REGISTER)
    bx      lr

global(air_set_irq_mask_register)
    svc     #(AIR_SYSCALL_ARM_SET_IRQ_MASK_REGISTER)

//FUNC(air_set_irq_force_register)
//  svc     #(AIR_SYSCALL_ARM_SET_IRQ_FORCE_REGISTER)

