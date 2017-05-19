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
 * @brief Contais the PMK generic system calls (user side)
 */

/* enforce ASM definition */
#ifndef ASM
#define ASM
#endif

#include <asm.h>
#include <xky.h>

    GLOBAL(xky_syscall_get_physical_addr)
SYM(xky_syscall_get_physical_addr):

    set     XKY_SYSCALL_GET_P_ADDR, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop


    GLOBAL(xky_syscall_get_core_id)
SYM(xky_syscall_get_core_id):

    set     XKY_SYSCALL_GET_CORE_ID, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop


    GLOBAL(xky_syscall_boot_core)
SYM(xky_syscall_boot_core):

    set     XKY_SYSCALL_BOOT_CORE, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop


    GLOBAL(xky_syscall_get_us_per_tick)
SYM(xky_syscall_get_us_per_tick):

    set     XKY_SYSCALL_GET_US_PER_TICK, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop

    GLOBAL(xky_syscall_get_elapsed_ticks)
SYM(xky_syscall_get_elapsed_ticks):

    set     XKY_SYSCALL_GET_ELAPSED_TICKS, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop


    GLOBAL(xky_syscall_get_partition_id)
SYM(xky_syscall_get_partition_id):

    set     XKY_SYSCALL_GET_PARTITION_ID, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop


    GLOBAL(xky_syscall_get_partition_status)
SYM(xky_syscall_get_partition_status):

    set     XKY_SYSCALL_GET_PARTITION_STATUS, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop


    GLOBAL(xky_syscall_set_partition_mode)
SYM(xky_syscall_set_partition_mode):

    set     XKY_SYSCALL_SET_PARTITION_MODE, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop


    GLOBAL(xky_syscall_get_schedule_id)
SYM(xky_syscall_get_schedule_id):

    set     XKY_SYSCALL_GET_SCHEDULE_ID, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop


    GLOBAL(xky_syscall_get_schedule_status)
SYM(xky_syscall_get_schedule_status):

    set     XKY_SYSCALL_GET_SCHEDULE_STATUS, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop


    GLOBAL(xky_syscall_set_schedule)
SYM(xky_syscall_set_schedule):

    set     XKY_SYSCALL_SET_SCHEDULE, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop


    GLOBAL(xky_syscall_get_tod)
SYM(xky_syscall_get_tod):

    set     XKY_SYSCALL_GET_TIME_OF_DAY, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop


    GLOBAL(xky_syscall_set_tod)
SYM(xky_syscall_set_tod):

    set     XKY_SYSCALL_SET_TIME_OF_DAY, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop


    GLOBAL(xky_syscall_get_port_id)
SYM(xky_syscall_get_port_id):

    set     XKY_SYSCALL_GET_PORT_ID, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop


    GLOBAL(xky_syscall_create_port)
SYM(xky_syscall_create_port):

    set     XKY_SYSCALL_CREATE_PORT, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop


    GLOBAL(xky_syscall_get_port_status)
SYM(xky_syscall_get_port_status):

    set     XKY_SYSCALL_GET_PORT_STATUS, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop


    GLOBAL(xky_syscall_read_port)
SYM(xky_syscall_read_port):

    set     XKY_SYSCALL_READ_PORT, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop


    GLOBAL(xky_syscall_write_port)
SYM(xky_syscall_write_port):

    set     XKY_SYSCALL_WRITE_PORT, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop


    GLOBAL(xky_syscall_get_sharedmemory)
SYM(xky_syscall_get_sharedmemory):

    set     XKY_SYSCALL_GET_SHARED_AREA, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop


    GLOBAL(xky_syscall_shutdown_module)
SYM(xky_syscall_shutdown_module):

    set     XKY_SYSCALL_SHUTDOWN_MODULE, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop

    GLOBAL(xky_syscall_get_event)
SYM(xky_syscall_get_event):
    set     XKY_SYSCALL_GET_EVENT, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop

    GLOBAL(xky_syscall_get_system_state)
SYM(xky_syscall_get_system_state):

    set     XKY_SYSCALL_GET_SYSTEM_STATE, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop

    GLOBAL(xky_syscall_set_system_state)
SYM(xky_syscall_set_system_state):

    set     XKY_SYSCALL_SET_SYSTEM_STATE, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop


    GLOBAL(xky_syscall_get_hm_event)
SYM(xky_syscall_get_hm_event):

    set     XKY_SYSCALL_GET_HM_EVENT, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop

    GLOBAL(xky_syscall_raise_hm_event)
SYM(xky_syscall_raise_hm_event):

    set     XKY_SYSCALL_RAISE_HM_EVENT, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop

    GLOBAL(xky_syscall_print)
SYM(xky_syscall_print):

    set     XKY_SYSCALL_PRINT, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop

    GLOBAL(xky_syscall_putchar)
SYM(xky_syscall_putchar):
    set     XKY_SYSCALL_PUTCHAR, %o5
    ta      XKY_SYSCALL_OS_TRAP - 0x80
    retl
    nop
