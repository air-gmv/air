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
#ifdef PMK_DEBUG
#include <printk.h>
#endif

void arm_svc_handler(
        air_u32_t svc_id,
        pmk_core_ctrl_t *core,
        arm_supervisor_stack_frame_t *frame) {

#ifdef PMK_DEBUG_SVC
    printk("\n ** svc #%i **\n", svc_id);
    printk("       with ret_addr: 0x%08x\n", frame->lr);
#endif
    air_u32_t elapsed = 0;

    switch (svc_id) {
    case AIR_SYSCALL_ARM_DISABLE_INTERRUPTS:
        arm_syscall_disable_interrupts(core);
        break;
    case AIR_SYSCALL_ARM_ENABLE_INTERRUPTS:
        arm_syscall_enable_interrupts(core);
        break;
    case AIR_SYSCALL_ARM_DISABLE_TRAPS:
        arm_syscall_disable_traps(core);
        break;
    case AIR_SYSCALL_ARM_ENABLE_TRAPS:
        arm_syscall_enable_traps(core);
        break;
    case AIR_SYSCALL_ARM_DISABLE_FPU:
        arm_syscall_disable_fpu(core);
        break;
    case AIR_SYSCALL_ARM_ENABLE_FPU:
        arm_syscall_enable_fpu(core);
        break;
    case AIR_SYSCALL_ARM_GET_TBR:
        frame->r0 = arm_syscall_get_tbr(core);
        break;
    case AIR_SYSCALL_ARM_GET_PSR:
        frame->r0 = arm_syscall_get_psr(core);
        break;
    case AIR_SYSCALL_ARM_GET_IRQ_MASK_REGISTER:
        frame->r0 = arm_syscall_get_irq_mask_register();
        break;

    case AIR_SYSCALL_ARM_SET_TBR:
        arm_syscall_set_tbr(core, (void *)frame->r0);
        break;
    case AIR_SYSCALL_ARM_SET_PSR:
        arm_syscall_set_psr(core, frame->r0);
        break;
    case AIR_SYSCALL_ARM_SET_IRQ_MASK_REGISTER:
        arm_syscall_set_irq_mask_register(frame->r0);
        break;

    case AIR_SYSCALL_GET_P_ADDR:
        frame->r0 = (air_u32_t)arm_get_physical_addr(core->partition->mmu_ctrl,
                (void *)frame->r0);
        break;
    case AIR_SYSCALL_GET_CORE_ID:
        frame->r0 = arm_syscall_get_core_id(core);
        break;
//  arm_svc_install_handler(AIR_SYSCALL_BOOT_CORE, );
    case AIR_SYSCALL_GET_US_PER_TICK:
        frame->r0 = pmk_get_usr_us_per_tick();
        break;
    case AIR_SYSCALL_GET_ELAPSED_TICKS:
        elapsed = arm_syscall_get_elapsed_ticks(core);
        frame->r0 = (air_u32_t)(elapsed & 0xffffffff);
        frame->r1 = (air_u32_t)((elapsed & 0xffffffff00000000) >> 32);
        break;
    case AIR_SYSCALL_GET_PARTITION_ID:
        pmk_syscall_get_partition_id(core,
                (air_name_ptr_t)frame->r0,
                (air_identifier_t *)frame->r1);
        break;
    case AIR_SYSCALL_GET_PARTITION_STATUS:
        frame->r0 = (air_u32_t)pmk_syscall_get_partition_status(
                core,
                (air_identifier_t)frame->r0,
                (air_partition_status_t *)frame->r1);
        break;
    case AIR_SYSCALL_SET_PARTITION_MODE:
        frame->r0 = (air_u32_t)pmk_syscall_set_partition_mode(
                core,
                (air_identifier_t)frame->r0,
                (air_operating_mode_e)frame->r1);
        break;
    case AIR_SYSCALL_GET_SCHEDULE_ID:
        frame->r0 = (air_u32_t)pmk_syscall_get_schedule_id(
                core,
                (air_name_ptr_t)frame->r0,
                (air_identifier_t *)frame->r1);
        break;
    case AIR_SYSCALL_GET_SCHEDULE_STATUS:
        frame->r0 = (air_u32_t)pmk_syscall_get_schedule_status(
                core,
                (air_schedule_status_t *)frame->r0);
        break;
    case AIR_SYSCALL_SET_SCHEDULE:
        frame->r0 = (air_u32_t)pmk_syscall_set_schedule(
                core,
                (air_identifier_t)frame->r0);
        break;
    case AIR_SYSCALL_GET_TIME_OF_DAY:
        frame->r0 = (air_u32_t)pmk_syscall_get_tod(
                core,
                (air_time_t *)frame->r0);
        break;
    case AIR_SYSCALL_SET_TIME_OF_DAY:
        frame->r0 = (air_u32_t)pmk_syscall_set_tod(
                core,
                (air_time_t *)frame->r0);
        break;
    case AIR_SYSCALL_GET_PORT_ID:
        frame->r0 = (air_u32_t)pmk_syscall_get_port_id(
                core,
                (air_port_type_e)frame->r0,
                (air_name_ptr_t)frame->r1,
                (air_identifier_t *)frame->r2);
        break;
    case AIR_SYSCALL_CREATE_PORT:
        frame->r0 = (air_u32_t)pmk_syscall_create_port(
                core,
                (air_port_type_e)frame->r0,
                (air_name_ptr_t)frame->r1,
                (void *)frame->r2,
                (air_identifier_t *)frame->r3);
        break;
    case AIR_SYSCALL_GET_PORT_STATUS:
        frame->r0 = (air_u32_t)pmk_syscall_get_port_status(
                core,
                (air_port_type_e)frame->r0,
                (air_identifier_t)frame->r1,
                (void *)frame->r2);
        break;
    case AIR_SYSCALL_READ_PORT:
        frame->r0 = (air_u32_t)pmk_syscall_read_port(
                core,
                (air_port_type_e)frame->r0,
                (air_identifier_t)frame->r1,
                (air_message_ptr_t)frame->r2,
                (air_sz_t *)frame->r3,
                (void *)frame->r4);
        break;
    case AIR_SYSCALL_WRITE_PORT:
        frame->r0 = (air_u32_t)pmk_syscall_write_port(
                core,
                (air_port_type_e)frame->r0,
                (air_identifier_t)frame->r1,
                (air_message_ptr_t)frame->r2,
                (air_sz_t)frame->r3,
                (void *)frame->r4);
        break;
    case AIR_SYSCALL_GET_SHARED_AREA:
        frame->r0 = (air_u32_t)pmk_syscall_get_sharedmemory(
                core,
                (air_name_ptr_t)frame->r0,
                (air_sharedmemory_t *)frame->r1);
        break;
//  arm_svc_install_handler(AIR_SYSCALL_SHUTDOWN_MODULE)
//  arm_svc_install_handler(AIR_SYSCALL_GET_EVENT)
//  arm_svc_install_handler(AIR_SYSCALL_GET_SYSTEM_STATE)
    case AIR_SYSCALL_SET_SYSTEM_STATE:
         frame->r0 = (air_u32_t)pmk_syscall_set_system_state(
                core,
                (air_state_e)frame->r0);
        break;
    case AIR_SYSCALL_GET_HM_EVENT:
        frame->r0 = (air_u32_t)pmk_syscall_get_hm_event(
                core,
                (air_hm_event_t *)frame->r0);
        break;
    case AIR_SYSCALL_RAISE_HM_EVENT:
        pmk_hm_isr_handler((air_error_e)frame->r0);
        break;
    case AIR_SYSCALL_PRINT:
        frame->r0 = (air_u32_t)pmk_syscall_print(
                core,
                (char *)frame->r0,
                (air_sz_t)frame->r1);
        break;
    case AIR_SYSCALL_PUTCHAR:
        pmk_syscall_putchar(core, (char)frame->r0);
        break;
    default:
        break;
    }

    return;
}
