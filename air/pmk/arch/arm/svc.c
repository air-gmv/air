/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file svc_handler.c
 * @author lumm
 * @brief exception_handlers and default
 */

#include <svc.h>

air_uptr_t arm_svc_table[MAX_SVC];

air_uptr_t arm_svc_handler(arm_interrupt_stack_frame_t *frame, pmk_core_ctrl_t *core) {

    air_uptr_t ret;
    air_u32_t syscall_number = frame->reserved;

    if (syscall_number < MAX_SVC) {
        ret = ((svc) arm_svc_table[syscall_number])(frame, core);
    }
    return ret;
}

air_uptr_t arm_svc_install_handler(air_u32_t vector, void *handler) {

    air_uptr_t old_handler = (air_uptr_t)arm_svc_table[vector];
    arm_svc_table[vector] = (air_uptr_t)handler;
    return old_handler;
}

void arm_svc_table_initialize(void) {

    arm_svc_install_handler(AIR_SYSCALL_ARM_DISABLE_INTERRUPTS, arm_syscall_disable_interrupts);
    arm_svc_install_handler(AIR_SYSCALL_ARM_ENABLE_INTERRUPTS, arm_syscall_enable_interrupts);
    arm_svc_install_handler(AIR_SYSCALL_ARM_DISABLE_INTERRUPTS, arm_syscall_disable_traps);
    arm_svc_install_handler(AIR_SYSCALL_ARM_ENABLE_INTERRUPTS, arm_syscall_enable_traps);
    arm_svc_install_handler(AIR_SYSCALL_ARM_DISABLE_FPU, arm_syscall_disable_fpu);
    arm_svc_install_handler(AIR_SYSCALL_ARM_ENABLE_FPU, arm_syscall_enable_fpu);
    arm_svc_install_handler(AIR_SYSCALL_ARM_GET_TBR, arm_syscall_get_tbr);
    arm_svc_install_handler(AIR_SYSCALL_ARM_SET_TBR, arm_syscall_set_tbr);
    arm_svc_install_handler(AIR_SYSCALL_ARM_GET_PSR, arm_syscall_get_psr);
    arm_svc_install_handler(AIR_SYSCALL_ARM_SET_PSR, arm_syscall_set_psr);
//  arm_svc_install_handler(AIR_SYSCALL_ARM_RETT
//  arm_svc_install_handler(AIR_SYSCALL_ARM_GET_CACHE_REGISTER, arm_syscall_get_cache_register);
//  arm_svc_install_handler(AIR_SYSCALL_ARM_SET_CACHE_REGISTER, arm_syscall_set_cache_register);
//  arm_svc_install_handler(AIR_SYSCALL_ARM_RESTORE_CACHE_REGISTER
    arm_svc_install_handler(AIR_SYSCALL_ARM_GET_IRQ_MASK_REGISTER, arm_syscall_get_irq_mask_register);
    arm_svc_install_handler(AIR_SYSCALL_ARM_SET_IRQ_MASK_REGISTER, arm_syscall_set_irq_mask_register);
//  arm_svc_install_handler(AIR_SYSCALL_ARM_SET_IRQ_FORCE_REGISTER, arm_syscall_set_irq_force_register);

    arm_svc_install_handler(AIR_SYSCALL_GET_P_ADDR, arm_get_physical_addr);
//  arm_svc_install_handler(AIR_SYSCALL_GET_CORE_ID, );
//  arm_svc_install_handler(AIR_SYSCALL_BOOT_CORE, );
//  arm_svc_install_handler(AIR_SYSCALL_GET_US_PER_TICK, );
//  arm_svc_install_handler(AIR_SYSCALL_GET_ELAPSED_TICKS, );
    arm_svc_install_handler(AIR_SYSCALL_GET_PARTITION_ID, pmk_syscall_get_partition_id);
    arm_svc_install_handler(AIR_SYSCALL_GET_PARTITION_STATUS, pmk_syscall_get_partition_status);
    arm_svc_install_handler(AIR_SYSCALL_SET_PARTITION_MODE, pmk_syscall_set_partition_mode);
    arm_svc_install_handler(AIR_SYSCALL_GET_SCHEDULE_ID, pmk_syscall_get_schedule_id);
    arm_svc_install_handler(AIR_SYSCALL_GET_SCHEDULE_STATUS, pmk_syscall_get_schedule_status);
    arm_svc_install_handler(AIR_SYSCALL_SET_SCHEDULE, pmk_syscall_set_schedule);
    arm_svc_install_handler(AIR_SYSCALL_GET_TIME_OF_DAY, pmk_syscall_get_tod);
    arm_svc_install_handler(AIR_SYSCALL_SET_TIME_OF_DAY, pmk_syscall_set_tod);
    arm_svc_install_handler(AIR_SYSCALL_GET_PORT_ID, pmk_syscall_get_port_id);
    arm_svc_install_handler(AIR_SYSCALL_CREATE_PORT, pmk_syscall_create_port);
    arm_svc_install_handler(AIR_SYSCALL_GET_PORT_STATUS, pmk_syscall_get_port_status);
    arm_svc_install_handler(AIR_SYSCALL_READ_PORT, pmk_syscall_read_port);
    arm_svc_install_handler(AIR_SYSCALL_WRITE_PORT, pmk_syscall_write_port);
    arm_svc_install_handler(AIR_SYSCALL_GET_SHARED_AREA, pmk_syscall_get_sharedmemory);
//  arm_svc_install_handler(AIR_SYSCALL_SHUTDOWN_MODULE)
//  arm_svc_install_handler(AIR_SYSCALL_GET_EVENT)
//  arm_svc_install_handler(AIR_SYSCALL_GET_SYSTEM_STATE)
    arm_svc_install_handler(AIR_SYSCALL_SET_SYSTEM_STATE, pmk_syscall_set_system_state);
    arm_svc_install_handler(AIR_SYSCALL_GET_HM_EVENT, pmk_syscall_get_hm_event);
    arm_svc_install_handler(AIR_SYSCALL_RAISE_HM_EVENT, pmk_hm_isr_handler);
    arm_svc_install_handler(AIR_SYSCALL_PRINT, pmk_syscall_print);
    arm_svc_install_handler(AIR_SYSCALL_PUTCHAR, pmk_syscall_putchar);

}
