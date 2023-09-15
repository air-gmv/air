/*
 * Copyright (C) 2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief Contains the SPARC virtual CPU control functions
 */

#include <air.h>
#include <bsp.h>
#include <configurations.h>
#include <cpu.h>
#include <health_monitor.h>
#include <ipc.h>
#include <multicore.h>
#include <pmk.h>
#include <schedule.h>
#include <sparc.h>
#include <syscall.h>

#define LOW_BITS_MASK 0x000003FF
#define HIGH_BITS_MASK 0xFFFFFC00
#define HIGH_BITS_SHIFT 10

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
static void sparc_syscall_os_handler(sparc_interrupt_stack_frame_t *isf, pmk_core_ctrl_t *core)
{

    /* call the system call handling function */
    switch (isf->i5)
    {

    /* get physical address */
    case AIR_SYSCALL_GET_P_ADDR:
        isf->i0 = (air_u32_t)sparc_get_physical_addr(core->partition->mmu_ctrl, (void *)isf->i0);
        break;

    /* get partition id */
    case AIR_SYSCALL_GET_PARTITION_ID:
        isf->i0 = (air_u32_t)pmk_syscall_get_partition_id(core, (air_name_ptr_t)isf->i0, (air_identifier_t *)isf->i1);
        break;

    /* get partition status */
    case AIR_SYSCALL_GET_PARTITION_STATUS:
        isf->i0 = (air_u32_t)pmk_syscall_get_partition_status(core, (air_identifier_t)isf->i0,
                                                              (air_partition_status_t *)isf->i1);
        break;

    /* set partition mode */
    case AIR_SYSCALL_SET_PARTITION_MODE:
        isf->i0 =
            (air_u32_t)pmk_syscall_set_partition_mode(core, (air_identifier_t)isf->i0, (air_operating_mode_e)isf->i1);
        break;

    /* get schedule id */
    case AIR_SYSCALL_GET_SCHEDULE_ID:
        isf->i0 = (air_u32_t)pmk_syscall_get_schedule_id(core, (air_name_ptr_t)isf->i0, (air_identifier_t *)isf->i1);
        break;

    /* get schedule status */
    case AIR_SYSCALL_GET_SCHEDULE_STATUS:
        isf->i0 = (air_u32_t)pmk_syscall_get_schedule_status(core, (air_schedule_status_t *)isf->i0);
        break;

    /* set schedule */
    case AIR_SYSCALL_SET_SCHEDULE:
        isf->i0 = (air_u32_t)pmk_syscall_set_schedule(core, (air_identifier_t)isf->i0);
        break;

    /* get time of day */
    case AIR_SYSCALL_GET_TIME_OF_DAY:
        isf->i0 = (air_u32_t)pmk_syscall_get_tod(core, (air_time_t *)isf->i0);
        break;

    /* set time of day */
    case AIR_SYSCALL_SET_TIME_OF_DAY:
        isf->i0 = (air_u32_t)pmk_syscall_set_tod(core, (air_time_t *)isf->i0);
        break;

    /* get port id */
    case AIR_SYSCALL_GET_PORT_ID:
        isf->i0 = (air_u32_t)pmk_syscall_get_port_id(core, (air_port_type_e)isf->i0, (air_name_ptr_t)isf->i1,
                                                     (air_identifier_t *)isf->i2);
        break;

    /* create port */
    case AIR_SYSCALL_CREATE_PORT:
        isf->i0 = (air_u32_t)pmk_syscall_create_port(core, (air_port_type_e)isf->i0, (air_name_ptr_t)isf->i1,
                                                     (void *)isf->i2, (air_identifier_t *)isf->i3);
        break;

    /* get port status */
    case AIR_SYSCALL_GET_PORT_STATUS:
        isf->i0 = (air_u32_t)pmk_syscall_get_port_status(core, (air_port_type_e)isf->i0, (air_identifier_t)isf->i1,
                                                         (void *)isf->i2);
        break;

    /* read port */
    case AIR_SYSCALL_READ_PORT:
        isf->i0 = (air_u32_t)pmk_syscall_read_port(core, (air_port_type_e)isf->i0, (air_identifier_t)isf->i1,
                                                   (air_message_ptr_t)isf->i2, (air_sz_t *)isf->i3, (void *)isf->i4);
        break;

    /* write port */
    case AIR_SYSCALL_WRITE_PORT:
        isf->i0 = (air_u32_t)pmk_syscall_write_port(core, (air_port_type_e)isf->i0, (air_identifier_t)isf->i1,
                                                    (air_message_ptr_t)isf->i2, (air_sz_t)isf->i3, (void *)isf->i4);
        break;

    /* get shared memory information */
    case AIR_SYSCALL_GET_SHARED_AREA:

        isf->i0 = (air_u32_t)pmk_syscall_get_sharedmemory(core, (air_name_ptr_t)isf->i0, (air_sharedmemory_t *)isf->i1);
        break;

    /* set system state */
    case AIR_SYSCALL_SET_SYSTEM_STATE:
        isf->i0 = (air_u32_t)pmk_syscall_set_system_state(core, (air_state_e)isf->i0);
        break;

    /* get health-monitor event */
    case AIR_SYSCALL_GET_HM_EVENT:
        isf->i0 = (air_u32_t)pmk_syscall_get_hm_event(core, (air_hm_event_t *)isf->i0);
        break;

    /* raise health-monitor event */
    case AIR_SYSCALL_RAISE_HM_EVENT:
        pmk_hm_isr_handler((air_error_e)isf->i0);
        break;

    /* print partition character */
    case AIR_SYSCALL_PUTCHAR:
        pmk_syscall_putchar(core, (char)isf->i0);
        break;

#if DEBUG_MONITOR != 2
    /* print partition buffer */
    case AIR_SYSCALL_PRINT:
        isf->i0 = (air_u32_t)pmk_syscall_print(core, (char *)isf->i0, (air_sz_t)isf->i1);
        break;
#endif /* DEBUG_MONITOR != 2 */

    /* get AMBA Plug & Play configuration area struct address */
    case AIR_SYSCALL_GET_AMBACONF:
        isf->i0 = (air_u32_t)&amba_confarea;
        break;

    /* finish partition execution on current window */
    case AIR_SYSCALL_END_WINDOW:
        pmk_syscall_end_window(core);
        break;

    /* system call not defined */
    default:
        break;
    }
}

/**
 * @brief SPARC trap table initialization
 */
void sparc_trap_table_int()
{

    air_i32_t tn;

    /* populate the PMK trap table */
    for (tn = 1; tn < 256; ++tn)
    {

        switch (tn)
        {

        /* skip reset, overflow and underflow */
        case 0x00:
        case 0x05:
        case 0x06:
            break;

        /* system calls for the guest OS */
        case AIR_SYSCALL_OS_TRAP:
            sparc_install_raw_trap_handler(tn, sparc_os_syscall);
            break;

        /* SPARC virtualization system calls for the guest OS */
        case AIR_SYSCALL_SPARC_TRAP:
            sparc_install_raw_trap_handler(tn, sparc_virt_syscall);
            break;

        /* SPARC windows flush system calls */
        case AIR_SYSCALL_FLUSH_TRAP:
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
    sparc_register_vector_trap_handler(AIR_SYSCALL_OS_TRAP, sparc_syscall_os_handler);
}
