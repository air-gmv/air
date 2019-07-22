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
 * @brief Contains the routines to perform a partition core context save
 *        and partition core context restore on a ARMv7
 *        The context is handled in schedule.c, so in here the mmu and irq
 *        switch happens
 */

#include <context_switch.h>
#include <workspace.h>
#ifdef PMK_DEBUG
#include <printk.h>
#endif

void arm_core_context_save(void *core) {
#ifdef PMK_DEBUG_SCHED
    printk("\n       CS :: Saving Context\n");
#endif

    if (((pmk_core_ctrl_t *)core)->context->trash) {
#ifdef PMK_DEBUG_SCHED
        printk("             CS :: TRASHED save context\n");
#endif
        return;
    }

    pmk_partition_t *partition = ((pmk_core_ctrl_t *)core)->partition;

#ifdef PMK_DEBUG_SCHED
    printk("       CS :: Done Saving\n");
#endif
}

void arm_core_context_restore(void *core) {
#ifdef PMK_DEBUG_SCHED
    printk("\n       CS :: Restoring Context\n");
#endif
    if (((pmk_core_ctrl_t *)core)->context->trash) {
#ifdef PMK_DEBUG_SCHED
        printk("       CS :: TRASHED restore context\n");
#endif
        return;
    }
    pmk_partition_t *partition = ((pmk_core_ctrl_t *)core)->partition;

    arm_mmu_disable();
    arm_mmu_enable(partition->mmu_ctrl);

#ifdef PMK_DEBUG_SCHED
    printk("       CS :: Done Restoring\n");
#endif
}
