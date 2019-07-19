/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file context_switch.c
 * @author lumm
 * @brief Contains the routines to peform a partition core context save
 *        and partition core context restore on a ARMv7
 */

#include <context_switch.h>
#include <workspace.h>

void arm_core_context_save(void *core) {

    pmk_core_ctrl_t *_core = (pmk_core_ctrl_t *)core;
    core_context_t *context = _core->context;
    pmk_partition_t *partition = _core->partition;
    core_context_t *partition_context = partition->context;

    // FPU_CONTEXT

    /* memcpy(dest, src, size) */
    memcpy(partition_context, context, sizeof(core_context_t));

    return;
}

void arm_core_context_restore(void *core) {

    pmk_core_ctrl_t *_core = (pmk_core_ctrl_t *)core;
    core_context_t *context = _core->context;
    pmk_partition_t *partition = _core->partition;
    core_context_t *partition_context = partition->context;

    // FPU_CONTEXT ?

    /* memcpy(dest, src, size) */
    memcpy(context, partition_context, sizeof(core_context_t));

    return;
}
