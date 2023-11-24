/*
 * Copyright (C) 2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
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
static void pmk_ipc_boot_partition_core(pmk_core_ctrl_t *core)
{

    /* setup the core context */
    core_context_setup_partition(core->context, core->partition);

    /* restore the new core context */
    core_context_restore(core);

    /*Heir VCPU trap table*/
    core_context_tbr(core->context) = core_context_tbr(core->partition->context);
}

/**
 * @brief Core trash
 * @param core Executing core control structure
 * @ingroup pmk_multicore
 */
static void pmk_ipc_trash_partition_core(pmk_core_ctrl_t *core)
{

    /* setup the core context as idle */
    core_context_setup_idle(core->context);

    /* restore the new core context */
    core_context_restore(core);
}

/**
 * @brief Core reload
 * @param core Executing core control structure
 * @ingroup pmk_multicore
 */
static void pmk_ipc_reload_core(pmk_core_ctrl_t *core)
{

    core_context_setup_reload_partition(core->context, core->partition);
}

void pmk_ipc_handler(void *isf, pmk_core_ctrl_t *core)
{

    /* process the current IPC event */
    pmk_ipc_message ipc_msg_id = core_context_get_ipc_message(core->context);

    switch (ipc_msg_id)
    {

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
        core_context_tbr(core->context) = core_context_tbr(core->partition->context);
        break;

    case PMK_IPC_PARTITION_RESTART:
        pmk_ipc_reload_core(core);
        break;

    /* no action */
    default:

        break;
    }

    /* clear IPC event */
    core_context_set_ipc_message(core->context, PMK_IPC_NO_ACTION);
}
