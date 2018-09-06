/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file ipc.c
 * @author pfnf
 * @brief Framework for inter-processor communication
 */

#include <pmk.h>
#include <cpu.h>
#include <bsp.h>
#include <ipc.h>
#include <multicore.h>

/**
 * @brief Core boot
 * @param core Executing core control structure
 * @ingroup pmk_multicore
 */
static void pmk_ipc_boot_partition_core(pmk_core_ctrl_t *core) {

    /* setup the core context */
    core_context_setup_partition(core->context, core->partition);

    /* restore the new core context */
    core_context_restore(core);
}

/**
 * @brief Core trash
 * @param core Executing core control structure
 * @ingroup pmk_multicore
 */
static void pmk_ipc_trash_partition_core(pmk_core_ctrl_t *core) {

    /* setup the core context as idle */
    core_context_setup_idle(core->context);

    /* restore the new core context */
    core_context_restore(core);
}

/**
 * @brief Copy main Core virtual tbr
 * @param core Executing core control structure
 * @ingroup pmk_multicore
 */
static void pmk_ipc_set_tbr(pmk_core_ctrl_t *core) {

    /*Get actual core vcpu*/
    core_context_t *core_ctx = core->context;
    sparc_virtual_cpu_t *core_vcpu = &core_ctx->vcpu;

    /*Extract partition's main vcpu*/
    pmk_partition_t *core_partition = core->partition;
    core_context_t *partition_ctx = &core_partition->context[0];
    sparc_virtual_cpu_t *partition_vcpu = &partition_ctx->vcpu;

    /*Set actual core context with partition's main core tbr*/
    core_vcpu->tbr = partition_vcpu->tbr;
}


void pmk_ipc_handler(void *isf, pmk_core_ctrl_t *core) {

    /* process the current IPC event */
    pmk_ipc_message ipc_msg_id =
            core_context_get_ipc_message(core->context);

    switch (ipc_msg_id) {

        /* boot a partition secondary core */
        case PMK_IPC_BOOT_PARTITION_CORE:

            pmk_ipc_boot_partition_core(core);
            break;

        /* trash a partition core */
        case PMK_IPC_TRASH_PARTITION_CORE:

            pmk_ipc_trash_partition_core(core);
            break;

        /* module shutdown */
        case PMK_IPC_MODULE_SHUTDOWN:
            bsp_shutdown_core(core);
            break;

        /* module restart */
        case PMK_IPC_MODULE_RESTART:
            bsp_restart_core(core);
            break;

        case PMK_IPC_SET_TBR:
            pmk_ipc_set_tbr(core);
            break;

        /* no action */
        default:

            break;
    }

    /* clear IPC event */
    core_context_set_ipc_message(core->context, PMK_IPC_NO_ACTION);
}
