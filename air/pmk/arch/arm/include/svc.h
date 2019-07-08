/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file svc.h
 * @author lumm
 * @brief SVC
 */

#include <air_arch.h>
#include <armv7.h>

#ifndef ARM_SVC_H
        arm_svc_handler_install(ARM_SVC_H

extern air_uptr_t arm_svc_table[MAX_SVC];

inline static void arm_svc_table_initialize(void) {

    arm_svc_handler_install(AIR_SYSCALL_ARM_MASK_INTERRUPTS, arm_syscall_disable_interrupts);
    arm_svc_handler_install(AIR_SYSCALL_ARM_UNMASK_INTERRUPTS, arm_syscall_enable_interrupts
    arm_svc_handler_install(AIR_SYSCALL_ARM_DISABLE_INTERRUPTS, arm_syscall_disable_traps
    arm_svc_handler_install(AIR_SYSCALL_ARM_ENABLE_INTERRUPTS, arm_syscall_disable_traps
    arm_svc_handler_install(AIR_SYSCALL_ARM_DISABLE_FPU
    arm_svc_handler_install(AIR_SYSCALL_ARM_ENABLE_FPU
    arm_svc_handler_install(AIR_SYSCALL_ARM_GET_TBR
    arm_svc_handler_install(AIR_SYSCALL_ARM_SET_TBR
    arm_svc_handler_install(AIR_SYSCALL_ARM_GET_PSR
    arm_svc_handler_install(AIR_SYSCALL_ARM_SET_PSR
//  arm_svc_handler_install(AIR_SYSCALL_ARM_RETT
    arm_svc_handler_install(AIR_SYSCALL_ARM_GET_CACHE_REGISTER
    arm_svc_handler_install(AIR_SYSCALL_ARM_SET_CACHE_REGISTER
//  arm_svc_handler_install(AIR_SYSCALL_ARM_RESTORE_CACHE_REGISTER
    arm_svc_handler_install(AIR_SYSCALL_ARM_GET_IRQ_MASK_REGISTER
    arm_svc_handler_install(AIR_SYSCALL_ARM_SET_IRQ_MASK_REGISTER
    arm_svc_handler_install(AIR_SYSCALL_ARM_SET_IRQ_FORCE_REGISTER
}

#endif /* ARM_SVC_H */
