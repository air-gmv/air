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
#include <syscall.h>

#ifndef ARM_SVC_H
#define ARM_SVC_H

extern air_uptr_t arm_svc_table[MAX_SVC];

air_u32_t arm_syscall_disable_interrupts(void);
air_u32_t arm_syscall_enable_interrupts(void);
air_u32_t arm_syscall_disable_traps(void);
air_u32_t arm_syscall_enable_traps(void);
air_u32_t arm_syscall_disable_fpu(void);
air_u32_t arm_syscall_enable_fpu(void);
air_u32_t arm_syscall_get_tbr(void);
air_u32_t arm_syscall_set_tbr(void);
air_u32_t arm_syscall_get_psr(void);
air_u32_t arm_syscall_set_psr(void);
// air_u32_t AIR_SYSCALL_ARM_RETT
air_u32_t arm_syscall_get_cache_register(void);
air_u32_t arm_syscall_set_cache_register(void);
// air_u32_t AIR_SYSCALL_ARM_RESTORE_CACHE_REGISTER
air_u32_t arm_syscall_get_irq_mask_register(void);
air_u32_t arm_syscall_set_irq_mask_register(void);
air_u32_t arm_syscall_set_irq_force_register(void);

static inline air_uptr_t arm_svc_handler_install(air_u32_t vector, void *handler) {

    air_uptr_t old_handler = arm_svc_table[vector];
    arm_svc_table[vector] = handler;
    return old_handler;
}

static inline void arm_svc_table_initialize(void) {

    arm_svc_handler_install(AIR_SYSCALL_ARM_DISABLE_INTERRUPTS, arm_syscall_disable_interrupts);
    arm_svc_handler_install(AIR_SYSCALL_ARM_ENABLE_INTERRUPTS, arm_syscall_enable_interrupts);
    arm_svc_handler_install(AIR_SYSCALL_ARM_DISABLE_INTERRUPTS, arm_syscall_disable_traps);
    arm_svc_handler_install(AIR_SYSCALL_ARM_ENABLE_INTERRUPTS, arm_syscall_enable_traps);
    arm_svc_handler_install(AIR_SYSCALL_ARM_DISABLE_FPU, arm_syscall_disable_fpu);
    arm_svc_handler_install(AIR_SYSCALL_ARM_ENABLE_FPU, arm_syscall_enable_fpu);
    arm_svc_handler_install(AIR_SYSCALL_ARM_GET_TBR, arm_syscall_get_tbr);
    arm_svc_handler_install(AIR_SYSCALL_ARM_SET_TBR, arm_syscall_set_tbr);
    arm_svc_handler_install(AIR_SYSCALL_ARM_GET_PSR, arm_syscall_get_psr);
    arm_svc_handler_install(AIR_SYSCALL_ARM_SET_PSR, arm_syscall_set_psr);
//  arm_svc_handler_install(AIR_SYSCALL_ARM_RETT
    arm_svc_handler_install(AIR_SYSCALL_ARM_GET_CACHE_REGISTER, arm_syscall_get_cache_register);
    arm_svc_handler_install(AIR_SYSCALL_ARM_SET_CACHE_REGISTER, arm_syscall_set_cache_register);
//  arm_svc_handler_install(AIR_SYSCALL_ARM_RESTORE_CACHE_REGISTER
    arm_svc_handler_install(AIR_SYSCALL_ARM_GET_IRQ_MASK_REGISTER, arm_syscall_get_irq_mask_register);
    arm_svc_handler_install(AIR_SYSCALL_ARM_SET_IRQ_MASK_REGISTER, arm_syscall_set_irq_mask_register);
    arm_svc_handler_install(AIR_SYSCALL_ARM_SET_IRQ_FORCE_REGISTER, arm_syscall_set_irq_force_register);

    arm_svc_handler_install(AIR_SYSCALL_GET_P_ADDR, arm_get_physical_addr); //TODO
//  arm_svc_handler_install(AIR_SYSCALL_GET_CORE_ID, );
//  arm_svc_handler_install(AIR_SYSCALL_BOOT_CORE, );
//  arm_svc_handler_install(AIR_SYSCALL_GET_US_PER_TICK, );
//  arm_svc_handler_install(AIR_SYSCALL_GET_ELAPSED_TICKS, );
    arm_svc_handler_install(AIR_SYSCALL_GET_PARTITION_ID, pmk_syscall_get_partition_id);
    arm_svc_handler_install(AIR_SYSCALL_GET_PARTITION_STATUS, pmk_syscall_get_partition_status);
    arm_svc_handler_install(AIR_SYSCALL_SET_PARTITION_MODE, pmk_syscall_set_partition_mode);
    arm_svc_handler_install(AIR_SYSCALL_GET_SCHEDULE_ID, pmk_syscall_get_schedule_id);
    arm_svc_handler_install(AIR_SYSCALL_GET_SCHEDULE_STATUS, pmk_syscall_get_schedule_status);
    arm_svc_handler_install(AIR_SYSCALL_SET_SCHEDULE, pmk_syscall_set_schedule);
    arm_svc_handler_install(AIR_SYSCALL_GET_TIME_OF_DAY, pmk_syscall_get_tod);
    arm_svc_handler_install(AIR_SYSCALL_SET_TIME_OF_DAY, pmk_syscall_set_tod);
    arm_svc_handler_install(AIR_SYSCALL_GET_PORT_ID, pmk_syscall_get_port_id);
    arm_svc_handler_install(AIR_SYSCALL_CREATE_PORT, pmk_syscall_create_port);
    arm_svc_handler_install(AIR_SYSCALL_GET_PORT_STATUS, pmk_syscall_get_port_status);
    arm_svc_handler_install(AIR_SYSCALL_READ_PORT, pmk_syscall_read_port);
    arm_svc_handler_install(AIR_SYSCALL_WRITE_PORT, pmk_syscall_write_port);
    arm_svc_handler_install(AIR_SYSCALL_GET_SHARED_AREA, pmk_syscall_get_sharedmemory);
//  arm_svc_handler_install(AIR_SYSCALL_SHUTDOWN_MODULE)
//  arm_svc_handler_install(AIR_SYSCALL_GET_EVENT)
//  arm_svc_handler_install(AIR_SYSCALL_GET_SYSTEM_STATE)
    arm_svc_handler_install(AIR_SYSCALL_SET_SYSTEM_STATE, pmk_syscall_set_system_state);
    arm_svc_handler_install(AIR_SYSCALL_GET_HM_EVENT, pmk_syscall_get_hm_event);
    arm_svc_handler_install(AIR_SYSCALL_RAISE_HM_EVENT, pmk_hm_isr_handler);
    arm_svc_handler_install(AIR_SYSCALL_PRINT, pmk_syscall_print);
    arm_svc_handler_install(AIR_SYSCALL_PUTCHAR, pmk_syscall_putchar);
}

#endif /* ARM_SVC_H */
