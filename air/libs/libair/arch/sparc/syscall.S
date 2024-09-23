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
 * @brief Contais the PMK generic system calls (user side)
 */

/* enforce ASM definition */
#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <air.h>

    GLOBAL(air_syscall_get_physical_addr)
SYM(air_syscall_get_physical_addr):

    set     AIR_SYSCALL_GET_P_ADDR, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop

    GLOBAL(air_syscall_get_nb_cores)
SYM(air_syscall_get_nb_cores):

    set     AIR_SYSCALL_GET_NB_CORES, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop

    GLOBAL(air_syscall_get_core_id)
SYM(air_syscall_get_core_id):

    set     AIR_SYSCALL_GET_CORE_ID, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop

    GLOBAL(air_syscall_copy_tbr)
SYM(air_syscall_set_tbr):

    set     AIR_SYSCALL_COPY_TBR, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop

    GLOBAL(air_syscall_boot_core)
SYM(air_syscall_boot_core):

    set     AIR_SYSCALL_BOOT_CORE, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop


    GLOBAL(air_syscall_get_us_per_tick)
SYM(air_syscall_get_us_per_tick):

    set     AIR_SYSCALL_GET_US_PER_TICK, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop

    GLOBAL(air_syscall_get_elapsed_ticks)
SYM(air_syscall_get_elapsed_ticks):

    set     AIR_SYSCALL_GET_ELAPSED_TICKS, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop;

    GLOBAL(air_syscall_get_partition_id)
SYM(air_syscall_get_partition_id):

    set     AIR_SYSCALL_GET_PARTITION_ID, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop


    GLOBAL(air_syscall_get_partition_status)
SYM(air_syscall_get_partition_status):

    set     AIR_SYSCALL_GET_PARTITION_STATUS, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop


    GLOBAL(air_syscall_set_partition_mode)
SYM(air_syscall_set_partition_mode):

    set     AIR_SYSCALL_SET_PARTITION_MODE, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop


    GLOBAL(air_syscall_get_schedule_id)
SYM(air_syscall_get_schedule_id):

    set     AIR_SYSCALL_GET_SCHEDULE_ID, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop


    GLOBAL(air_syscall_get_schedule_status)
SYM(air_syscall_get_schedule_status):

    set     AIR_SYSCALL_GET_SCHEDULE_STATUS, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop


    GLOBAL(air_syscall_set_schedule)
SYM(air_syscall_set_schedule):

    set     AIR_SYSCALL_SET_SCHEDULE, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop


    GLOBAL(air_syscall_get_tod)
SYM(air_syscall_get_tod):

    set     AIR_SYSCALL_GET_TIME_OF_DAY, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop


    GLOBAL(air_syscall_set_tod)
SYM(air_syscall_set_tod):

    set     AIR_SYSCALL_SET_TIME_OF_DAY, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop


    GLOBAL(air_syscall_get_port_id)
SYM(air_syscall_get_port_id):

    set     AIR_SYSCALL_GET_PORT_ID, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop


    GLOBAL(air_syscall_create_port)
SYM(air_syscall_create_port):

    set     AIR_SYSCALL_CREATE_PORT, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop


    GLOBAL(air_syscall_get_port_status)
SYM(air_syscall_get_port_status):

    set     AIR_SYSCALL_GET_PORT_STATUS, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop

    GLOBAL(air_syscall_get_port_name)
SYM(air_syscall_get_port_name):

    set     AIR_SYSCALL_GET_PORT_NAME, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop
    
    GLOBAL(air_syscall_read_port)
SYM(air_syscall_read_port):

    set     AIR_SYSCALL_READ_PORT, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop


    GLOBAL(air_syscall_write_port)
SYM(air_syscall_write_port):

    set     AIR_SYSCALL_WRITE_PORT, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop


    GLOBAL(air_syscall_get_sharedmemory)
SYM(air_syscall_get_sharedmemory):

    set     AIR_SYSCALL_GET_SHARED_AREA, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop


    GLOBAL(air_syscall_shutdown_module)
SYM(air_syscall_shutdown_module):

    set     AIR_SYSCALL_SHUTDOWN_MODULE, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop

    GLOBAL(air_syscall_get_event)
SYM(air_syscall_get_event):
    set     AIR_SYSCALL_GET_EVENT, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop

    GLOBAL(air_syscall_get_system_state)
SYM(air_syscall_get_system_state):

    set     AIR_SYSCALL_GET_SYSTEM_STATE, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop

    GLOBAL(air_syscall_set_system_state)
SYM(air_syscall_set_system_state):

    set     AIR_SYSCALL_SET_SYSTEM_STATE, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop


    GLOBAL(air_syscall_get_hm_event)
SYM(air_syscall_get_hm_event):

    set     AIR_SYSCALL_GET_HM_EVENT, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop

    GLOBAL(air_syscall_raise_hm_event)
SYM(air_syscall_raise_hm_event):

    set     AIR_SYSCALL_RAISE_HM_EVENT, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop

#if DEBUG_MONITOR != 2
    GLOBAL(air_syscall_print)
SYM(air_syscall_print):

    set     AIR_SYSCALL_PRINT, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop
#endif /* DEBUG_MONITOR != 2 */

    GLOBAL(air_syscall_putchar)
SYM(air_syscall_putchar):
    set     AIR_SYSCALL_PUTCHAR, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop

    GLOBAL(air_syscall_get_ambaconf)
SYM(air_syscall_get_ambaconf):

    set     AIR_SYSCALL_GET_AMBACONF, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop

    GLOBAL(air_syscall_end_window)
SYM(air_syscall_end_window):

    set     AIR_SYSCALL_END_WINDOW, %o5
    ta      AIR_SYSCALL_OS_TRAP
    retl
    nop; nop
