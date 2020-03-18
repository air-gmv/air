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
global(air_syscall_get_physical_addr)
    svc     #(AIR_SYSCALL_GET_P_ADDR)
    bx      lr

global(air_syscall_get_core_id)
    svc     #(AIR_SYSCALL_GET_CORE_ID)
    bx      lr

global(air_syscall_boot_core)
    svc     #(AIR_SYSCALL_BOOT_CORE)
    bx      lr

global(air_syscall_get_us_per_tick)
    svc     #(AIR_SYSCALL_GET_US_PER_TICK)
    bx      lr

global(air_syscall_get_elapsed_ticks)
    svc     #(AIR_SYSCALL_GET_ELAPSED_TICKS)
    bx      lr

global(air_syscall_get_partition_id)
    svc     #(AIR_SYSCALL_GET_PARTITION_ID)
    bx      lr

global(air_syscall_get_partition_status)
    svc     #(AIR_SYSCALL_GET_PARTITION_STATUS)
    bx      lr

global(air_syscall_set_partition_mode)
    svc     #(AIR_SYSCALL_SET_PARTITION_MODE)
    bx      lr

global(air_syscall_get_schedule_id)
    svc     #(AIR_SYSCALL_GET_SCHEDULE_ID)
    bx      lr

global(air_syscall_get_schedule_status)
    svc     #(AIR_SYSCALL_GET_SCHEDULE_STATUS)
    bx      lr

global(air_syscall_set_schedule)
    svc     #(AIR_SYSCALL_SET_SCHEDULE)
    bx      lr

global(air_syscall_get_tod)
    svc     #(AIR_SYSCALL_GET_TIME_OF_DAY)
    bx      lr

global(air_syscall_set_tod)
    svc     #(AIR_SYSCALL_SET_TIME_OF_DAY)
    bx      lr

global(air_syscall_get_port_id)
    svc     #(AIR_SYSCALL_GET_PORT_ID)
    bx      lr

global(air_syscall_create_port)
    svc     #(AIR_SYSCALL_CREATE_PORT)
    bx      lr

global(air_syscall_get_port_status)
    svc     #(AIR_SYSCALL_GET_PORT_STATUS)
    bx      lr

global(air_syscall_read_port)
    svc     #(AIR_SYSCALL_READ_PORT)
    bx      lr

global(air_syscall_write_port)
    svc     #(AIR_SYSCALL_WRITE_PORT)
    bx      lr

global(air_syscall_get_sharedmemory)
    svc     #(AIR_SYSCALL_GET_SHARED_AREA)
    bx      lr

global(air_syscall_shutdown_module)
    svc     #(AIR_SYSCALL_SHUTDOWN_MODULE)
    bx      lr

global(air_syscall_get_event)
    svc     #(AIR_SYSCALL_GET_EVENT)
    bx      lr

global(air_syscall_get_system_state)
    svc     #(AIR_SYSCALL_GET_SYSTEM_STATE)
    bx      lr

global(air_syscall_set_system_state)
    svc     #(AIR_SYSCALL_SET_SYSTEM_STATE)
    bx      lr

global(air_syscall_get_hm_event)
    svc     #(AIR_SYSCALL_GET_HM_EVENT)
    bx      lr

global(air_syscall_raise_hm_event)
    svc     #(AIR_SYSCALL_RAISE_HM_EVENT)
    bx      lr

global(air_syscall_putchar)
    svc     #(AIR_SYSCALL_PUTCHAR)
    bx      lr

global(air_syscall_print)
    svc     #(AIR_SYSCALL_PRINT)
    bx      lr

/*trcpse---------------------------*/

global(air_syscall_get_nb_cores)
    svc     #(AIR_SYSCALL_GET_NB_CORES)
    bx      lr

/*trcpse---------------------------*/
