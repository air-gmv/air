/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file
 * @author pfnf
 * @brief Contains the SPARC virtual CPU control functions
 */

#include <xky.h>
#include <pmk.h>
#include <cpu.h>
#include <bsp.h>
#include <ipc.h>
#include <sparc.h>
#include <syscall.h>
#include <multicore.h>
#include <health_monitor.h>
#include <schedule.h>
#include <configurations.h>


#define LOW_BITS_MASK           0x000003FF
#define HIGH_BITS_MASK          0xFFFFFC00
#define HIGH_BITS_SHIFT         10


void sparc_os_syscall();
void sparc_virt_syscall();
void sparc_supervisor_swap_pil();
void sparc_window_flush_handler();
void sparc_isr_handler_entry();


void *sparc_get_physical_addr(mmu_context_t *context, void *v_addr);

/**
 * @brief SPARC OS system call handler
 * @param isf Interrupt stack frame
 * @param core Executing core control structure
 */
static void sparc_syscall_os_handler(
        sparc_interrupt_stack_frame_t *isf, pmk_core_ctrl_t *core) {

    /* call the system call handling function */
    switch (isf->i5) {

        /* get physical address */
        case XKY_SYSCALL_GET_P_ADDR:
            isf->i0 = (xky_u32_t)sparc_get_physical_addr(
                    core->partition->mmu_ctrl,
                    (void *)isf->i0);
            break;

        /* get partition id */
        case XKY_SYSCALL_GET_PARTITION_ID:
            isf->i0 = (xky_u32_t)pmk_syscall_get_partition_id(
                    core,
                    (xky_name_ptr_t)isf->i0,
                    (xky_identifier_t *)isf->i1);
            break;

        /* get partition status */
        case XKY_SYSCALL_GET_PARTITION_STATUS:
            isf->i0 = (xky_u32_t)pmk_syscall_get_partition_status(
                    core,
                    (xky_identifier_t)isf->i0,
                    (xky_partition_status_t *)isf->i1);
            break;

        /* set partition mode */
        case XKY_SYSCALL_SET_PARTITION_MODE:
            isf->i0 = (xky_u32_t)pmk_syscall_set_partition_mode(
                    core,
                    (xky_identifier_t)isf->i0,
                    (xky_operating_mode_e)isf->i1);
            break;

        /* get schedule id */
        case XKY_SYSCALL_GET_SCHEDULE_ID:
            isf->i0 = (xky_u32_t)pmk_syscall_get_schedule_id(
                    core,
                    (xky_name_ptr_t)isf->i0,
                    (xky_identifier_t *)isf->i1);
            break;

        /* get schedule status */
        case XKY_SYSCALL_GET_SCHEDULE_STATUS:
            isf->i0 = (xky_u32_t)pmk_syscall_get_schedule_status(
                    core,
                    (xky_schedule_status_t *)isf->i0);
            break;

        /* set schedule */
        case XKY_SYSCALL_SET_SCHEDULE:
            isf->i0 = (xky_u32_t)pmk_syscall_set_schedule(
                    core,
                    (xky_identifier_t)isf->i0);
            break;

        /* get time of day */
        case XKY_SYSCALL_GET_TIME_OF_DAY:
            isf->i0 = (xky_u32_t)pmk_syscall_get_tod(
                    core,
                    (xky_time_t *)isf->i0);
            break;

        /* set time of day */
        case XKY_SYSCALL_SET_TIME_OF_DAY:
            isf->i0 = (xky_u32_t)pmk_syscall_set_tod(
                    core,
                    (xky_time_t *)isf->i0);
            break;

        /* get port id */
        case XKY_SYSCALL_GET_PORT_ID:
            isf->i0 = (xky_u32_t)pmk_syscall_get_port_id(
                    core,
                    (xky_port_type_e)isf->i0,
                    (xky_name_ptr_t)isf->i1,
                    (xky_identifier_t *)isf->i2);
            break;

        /* create port */
        case XKY_SYSCALL_CREATE_PORT:
            isf->i0 = (xky_u32_t)pmk_syscall_create_port(
                    core,
                    (xky_port_type_e)isf->i0,
                    (xky_name_ptr_t)isf->i1,
                    (void *)isf->i2,
                    (xky_identifier_t *)isf->i3);
            break;

        /* get port status */
        case XKY_SYSCALL_GET_PORT_STATUS:
            isf->i0 = (xky_u32_t)pmk_syscall_get_port_status(
                core,
                (xky_port_type_e)isf->i0,
                (xky_identifier_t)isf->i1,
                (void *)isf->i2);
            break;

        /* read port */
        case XKY_SYSCALL_READ_PORT:
            isf->i0 = (xky_u32_t)pmk_syscall_read_port(
                    core,
                    (xky_port_type_e)isf->i0,
                    (xky_identifier_t)isf->i1,
                    (xky_message_ptr_t)isf->i2,
                    (xky_sz_t *)isf->i3,
                    (void *)isf->i4);
            break;

        /* write port */
        case XKY_SYSCALL_WRITE_PORT:
            isf->i0 = (xky_u32_t)pmk_syscall_write_port(
                    core,
                    (xky_port_type_e)isf->i0,
                    (xky_identifier_t)isf->i1,
                    (xky_message_ptr_t)isf->i2,
                    (xky_sz_t)isf->i3,
                    (void *)isf->i4);
            break;

        /* get shared memory information */
        case XKY_SYSCALL_GET_SHARED_AREA:

            isf->i0 = (xky_u32_t)pmk_syscall_get_sharedmemory(
                    core,
                    (xky_name_ptr_t)isf->i0,
                    (xky_sharedmemory_t *)isf->i1);
            break;

        /* set system state */
        case XKY_SYSCALL_SET_SYSTEM_STATE:
            isf->i0 = (xky_u32_t)pmk_syscall_set_system_state(
                    core,
                    (xky_state_e)isf->i0);
            break;

        /* get health-monitor event */
        case XKY_SYSCALL_GET_HM_EVENT:
            isf->i0 = (xky_u32_t)pmk_syscall_get_hm_event(
                    core,
                    (xky_hm_event_t *)isf->i0);
            break;

        /* raise health-monitor event */
        case XKY_SYSCALL_RAISE_HM_EVENT:
            pmk_hm_isr_handler((xky_error_e)isf->i0);
            break;

        /* print partition character */
        case XKY_SYSCALL_PUTCHAR:
            pmk_syscall_putchar(core, (char)isf->i0);
            break;

        /* print partition buffer */
        case XKY_SYSCALL_PRINT:
            isf->i0 = (xky_u32_t)pmk_syscall_print(
                    core,
                    (char *)isf->i0,
                    (xky_sz_t)isf->i1);
            break;

        /* system call not defined */
        default:
            break;
    }
}

/**
 * @brief SPARC trap table initialization
 */
void sparc_trap_table_int() {

    xky_i32_t tn;

    /* populate the PMK trap table */
    for (tn = 1; tn < 256; ++tn) {

        switch (tn) {

            /* skip reset, overflow and underflow */
            case 0x00:
            case 0x05:
            case 0x06:
                break;

            /* system calls for the guest OS */
            case XKY_SYSCALL_OS_TRAP:
                sparc_install_raw_trap_handler(tn, sparc_os_syscall);
                break;

            /* SPARC virtualization system calls for the guest OS */
            case XKY_SYSCALL_SPARC_TRAP:
                sparc_install_raw_trap_handler(tn, sparc_virt_syscall);
                break;

            /* SPARC windows flush system calls */
            case XKY_SYSCALL_FLUSH_TRAP:
                sparc_install_raw_trap_handler(tn, sparc_window_flush_handler);
                break;

            /* SPARC supervisor PIL swap */
            case 0xFF:
                sparc_install_raw_trap_handler(tn, sparc_supervisor_swap_pil);
                break;

            default:
                sparc_install_raw_trap_handler(tn, sparc_isr_handler_entry);
                break;
        }
    }

    /* install PMK OS system call C support */
    sparc_register_vector_trap_handler(
            XKY_SYSCALL_OS_TRAP, sparc_syscall_os_handler);
}
