/*
 * Copyright (C) 2018-2023  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * https://github.com/air-gmv/air/tree/master/LICENSE
 */
/**
 * \file svc.c
 * \author lumm, bmmd
 * \brief Supervisor Call handlers
 */

#include <svc.h>
#include <syscall.h>
#include <configurations.h>
#include <bsp.h>

#ifdef PMK_DEBUG
#include <printk.h>
#endif

#include <gic.h>

/**
 * @brief Supervisor Call handler for ARM
 * @param frame Pointer to the ARM interrupt stack frame
 * @param core Pointer to the PMK core control structure
 */
void arm_svc_handler(arm_interrupt_stack_frame_t *frame, pmk_core_ctrl_t *core) {

    // Declare and initialize local variables
    air_u32_t svc_id;  // SVC ID extracted from the instruction
    air_u64_t elapsed = 0;  // Elapsed time
    int psri = core->context->vcpu.psr; // Processor Status Register(PSR) value
    air_clocktick_t us_per_tick = 0; // Get the number of microseconds per tick

    // Determine the SVC ID based on the instruction
    if (frame->ret_psr & ARM_PSR_T) {
        svc_id = ( *((air_uptr_t *)(frame->ret_addr - 2)) & 0xff );
    } else {
        svc_id = ( *((air_uptr_t *)(frame->ret_addr - 4)) & 0xffffff );
    }

    // Handle different SVC IDs
    switch (svc_id) {
        
    case AIR_SYSCALL_ARM_DISABLE_INTERRUPTS:
        // Disable interrupts
        frame->r0 = arm_syscall_disable_interrupts(core);
        break;
    case AIR_SYSCALL_ARM_ENABLE_INTERRUPTS:
        // Enable interrupts
        arm_syscall_enable_interrupts(core);
        break;
    case AIR_SYSCALL_ARM_DISABLE_TRAPS:
        // Disable traps
        arm_syscall_disable_traps(core);
        break;
    case AIR_SYSCALL_ARM_ENABLE_TRAPS:
        // Enable traps
        arm_syscall_enable_traps(core);
        break;
    case AIR_SYSCALL_ARM_DISABLE_FPU:
        // Disable FPU
        frame->r0  = arm_syscall_disable_fpu(core);
        break;
    case AIR_SYSCALL_ARM_ENABLE_FPU:
        // Enable FPU
        arm_syscall_enable_fpu(core);
        break;
    case AIR_SYSCALL_ARM_GET_TBR:
        // Get the value of the Time Base Register (TBR)
        frame->r0 = arm_syscall_get_tbr(core);
        break;
    case AIR_SYSCALL_ARM_GET_PSR:
        // Get the value of the Program Status Register (PSR)
        frame->r0 = arm_syscall_get_psr(core);
        break;
    case AIR_SYSCALL_ARM_GET_IRQ_MASK_REGISTER:
        // Get the value of the IRQ mask register
        frame->r0 = arm_syscall_get_irq_mask_register(core);
        break;
    case AIR_SYSCALL_ARM_SET_TBR:
        // Set the value of the Time Base Register (TBR)
        arm_syscall_set_tbr(core, frame->r0);
        break;
    case AIR_SYSCALL_ARM_SET_PSR:
        // Set the value of the Program Status Register (PSR)
        arm_syscall_set_psr(core, frame->r0);
        break;
    case AIR_SYSCALL_ARM_GET_SPSR:
        // Get the value of the Saved Program Status Register (SPSR)
        frame->r5 = arm_syscall_get_spsr(core);
        break;
    case AIR_SYSCALL_ARM_SET_SPSR:
        // Set the value of the Saved Program Status Register (SPSR)
        arm_syscall_set_spsr(core, frame->r5);
        break;
    case AIR_SYSCALL_ARM_SET_IRQ_MASK_REGISTER:
        // Set the value of the IRQ mask register
        arm_syscall_set_irq_mask_register(core, frame->r0);
        break;
    case AIR_SYSCALL_GET_P_ADDR:
        // Get the physical address corresponding to a virtual address
        frame->r0 = (air_u32_t)arm_get_physical_addr(core->partition->mmu_ctrl, (void *)frame->r0);
        break;
    case AIR_SYSCALL_GET_CORE_ID:
        // Get the ID of the current core
        frame->r0 = arm_syscall_get_core_id(core);
        break;                
      case AIR_SYSCALL_GET_US_PER_TICK:
        // Get the number of microseconds per tick
        us_per_tick = pmk_get_usr_us_per_tick();
        frame->r0 = (air_u32_t)(us_per_tick & 0xffffffff);
        frame->r1 = (air_u32_t)((us_per_tick & 0xffffffff00000000) >> 32);
        break;

    case AIR_SYSCALL_GET_ELAPSED_TICKS:
        // Get the elapsed ticks since system start
        elapsed = arm_syscall_get_elapsed_ticks(core);
        frame->r0 = (air_u32_t)(elapsed & 0xffffffff);
        frame->r1 = (air_u32_t)((elapsed & 0xffffffff00000000) >> 32);
        break;
    case AIR_SYSCALL_GET_PARTITION_ID:
        // Get the partition ID based on the name and identifier
        frame->r0 = pmk_syscall_get_partition_id(core, 
            (air_name_ptr_t)frame->r0, 
            (air_identifier_t *)frame->r1);
        break;
    case AIR_SYSCALL_GET_PARTITION_STATUS:
        // Get the status of a partition based on its identifier
        frame->r0 = (air_u32_t)pmk_syscall_get_partition_status(core, 
            (air_identifier_t)frame->r0, 
            (air_partition_status_t *)frame->r1);
        break;
    case AIR_SYSCALL_SET_PARTITION_MODE:
        // Set the operating mode of a partition based on its identifier
        frame->r0 = (air_u32_t)pmk_syscall_set_partition_mode(core, 
            (air_identifier_t)frame->r0, 
            (air_operating_mode_e)frame->r1);
        break;
    case AIR_SYSCALL_GET_SCHEDULE_ID:
        // Get the schedule ID based on the name and identifier
        frame->r0 = (air_u32_t)pmk_syscall_get_schedule_id(core, 
            (air_name_ptr_t)frame->r0, 
            (air_identifier_t *)frame->r1);
        break;

    case AIR_SYSCALL_GET_SCHEDULE_STATUS:
        // Get the status of the current schedule
        frame->r0 = (air_u32_t)pmk_syscall_get_schedule_status(core, 
            (air_schedule_status_t *)frame->r0);
        break;
    case AIR_SYSCALL_SET_SCHEDULE:
        // Set the current schedule based on its identifier
        frame->r0 = (air_u32_t)pmk_syscall_set_schedule(core, 
            (air_identifier_t)frame->r0);
        break;
    case AIR_SYSCALL_GET_TIME_OF_DAY:
        // Get the current time of day
        frame->r0 = (air_u32_t)pmk_syscall_get_tod(core, 
            (air_time_t *)frame->r0);
        break;
    case AIR_SYSCALL_SET_TIME_OF_DAY:
        // Set the current time of day
        frame->r0 = (air_u32_t)pmk_syscall_set_tod(core, 
            (air_time_t *)frame->r0);
        break;
    case AIR_SYSCALL_GET_PORT_ID:
        // Get the port ID based on the port type, name, and identifier
        frame->r0 = (air_u32_t)pmk_syscall_get_port_id(core, 
            (air_port_type_e)frame->r0, 
            (air_name_ptr_t)frame->r1, 
            (air_identifier_t *)frame->r2);
        break;
    case AIR_SYSCALL_CREATE_PORT:
        // Create a new port based on the port type, name, data pointer, and identifier
        frame->r0 = (air_u32_t)pmk_syscall_create_port(core, 
            (air_port_type_e)frame->r0, 
            (air_name_ptr_t)frame->r1, 
            (void *)frame->r2, 
            (air_identifier_t *)frame->r3);
        break;
    case AIR_SYSCALL_GET_PORT_STATUS:
        // Get the status of a port based on its identifier
        frame->r0 = (air_u32_t)pmk_syscall_get_port_status(
                core,
                (air_port_type_e)frame->r0,
                (air_identifier_t)frame->r1,
                (void *)frame->r2);
        break;
    case AIR_SYSCALL_READ_PORT:
        // Read from a port based on the port type, identifier, buffer, buffer size, and user stack pointer
        frame->r0 = (air_u32_t)pmk_syscall_read_port(
            core,
            (air_port_type_e)frame->r0,
            (air_identifier_t)frame->r1,
            (air_message_ptr_t)frame->r2,
            (air_sz_t *)frame->r3,
            (void *)((air_uptr_t *)frame->usr_sp)[0]);
        break;
    case AIR_SYSCALL_WRITE_PORT:
        // Write to a port based on the port type, identifier, message, message size, and user stack pointer
        frame->r0 = (air_u32_t)pmk_syscall_write_port(
            core,
            (air_port_type_e)frame->r0,
            (air_identifier_t)frame->r1,
            (air_message_ptr_t)frame->r2,
            (air_sz_t)frame->r3,
            (void *)((air_uptr_t *)frame->usr_sp)[0]);
        break;
    case AIR_SYSCALL_GET_SHARED_AREA:
        // Get a shared memory area based on the name and shared memory pointer
        frame->r0 = (air_u32_t)pmk_syscall_get_sharedmemory(
            core,
            (air_name_ptr_t)frame->r0,
            (air_sharedmemory_t *)frame->r1);
        break;
    case AIR_SYSCALL_SHUTDOWN_MODULE:
        // Shutdown the module for the current core
        bsp_shutdown_core(core);
        break;
    case AIR_SYSCALL_SET_SYSTEM_STATE:
        // Set the system state based on the state
        frame->r0 = (air_u32_t)pmk_syscall_set_system_state(
            core,
            (air_state_e)frame->r0);
        break;
    case AIR_SYSCALL_GET_HM_EVENT:
        // Get the health monitor event for the current core
        frame->r0 = (air_u32_t)pmk_syscall_get_hm_event(
            core,
            (air_hm_event_t *)frame->r0);
        break;
    case AIR_SYSCALL_RAISE_HM_EVENT:
        // Raise a health monitor event with the given error code
        pmk_hm_isr_handler((air_error_e)frame->r0);
        break;

#if DEBUG_MONITOR != 2
    case AIR_SYSCALL_PRINT:
        // Print a string based on the core, string pointer, and string size
        frame->r0 = (air_u32_t)pmk_syscall_print(
            core,
            (char *)frame->r0,
            (air_sz_t)frame->r1);
        break;
#endif /* DEBUG_MONITOR != 2 */
    case AIR_SYSCALL_PUTCHAR:
        // Write a character to the standard output
        pmk_syscall_putchar(core, (char)frame->r0);
        break;
    case AIR_SYSCALL_GET_NB_CORES:
        // Get the number of cores in the system
        frame->r0 = arm_ic_processor_count();
        break;
    case AIR_SYSCALL_ARM_ACK_INT:
        // Acknowledge an interrupt for the current core
        frame->r0 = arm_syscall_acknowledge_int(core);
        break;
    case AIR_SYSCALL_ARM_RETT:
        arm_syscall_return(core);
        //don't break!
    case AIR_SYSCALL_ARM_END_INT:
        // End the interrupt for the current core and return from the handler
        arm_syscall_rett(core);
        break;
    default:
        break;    
    } // switch
    return;
}
