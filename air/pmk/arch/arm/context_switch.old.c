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
#include <printk.h>

void arm_core_context_save(void *core) {
#ifdef PMK_DEBUG
    printk("    :: Saving Context\n");
#endif
    core_context_t *context = ((pmk_core_ctrl_t *)core)->context;
    pmk_partition_t *partition = ((pmk_core_ctrl_t *)core)->partition;
    core_context_t *partition_context = partition->context;
    // FPU_CONTEXT
#ifdef PMK_DEBUG
    printk("core context:      0x%08x\n"
            "partition context: 0x%08x\n",
            (air_u32_t)context, (air_u32_t)partition_context);
#endif

#ifdef PMK_DEBUG
    printk("before context save\n"
            "context->isf_pointer->lr                  = 0x%08x\n",
            ((arm_interrupt_stack_frame_t *)context->isf_pointer)->lr);
    printk("partition_context->isf_pointer->lr        = 0x%08x\n\n",
            ((arm_interrupt_stack_frame_t *)partition_context->isf_pointer)->lr);
#endif

    /* memcpy(dest, src, size) */
    memcpy(partition_context, context, sizeof(core_context_t));

#ifdef PMK_DEBUG
    printk("after context save\n"
            "context->isf_pointer->lr                  = 0x%08x\n",
            ((arm_interrupt_stack_frame_t *)context->isf_pointer)->lr);
    printk("partition_context->isf_pointer->lr        = 0x%08x\n\n",
            ((arm_interrupt_stack_frame_t *)partition_context->isf_pointer)->lr);
#endif
#ifdef PMK_DEBUG
    printk("    :: Done Saving\n");
#endif
}

void arm_core_context_restore(void *core) {
#ifdef PMK_DEBUG
    printk("    :: Restoring Context\n");
#endif
//  pmk_core_ctrl_t *_core = (pmk_core_ctrl_t *)core;
    core_context_t *context = ((pmk_core_ctrl_t *)core)->context;
    if (context->trash) {
#ifdef PMK_DEBUG
        printk("       Context Trashed\n");
#endif
        return;
    }
    pmk_partition_t *partition = ((pmk_core_ctrl_t *)core)->partition;
    core_context_t *partition_context = partition->context;

#ifdef PMK_DEBUG
    printk("core context:      0x%08x\n"
            "partition context: 0x%08x\n",
            (air_u32_t)context, (air_u32_t)partition_context);
#endif
    // FPU_CONTEXT ?
#ifdef PMK_DEBUG
    printk("before context restore\n"
            "context->isf_pointer->lr                  = 0x%08x\n",
            ((arm_interrupt_stack_frame_t *)context->isf_pointer)->lr);
    printk("partition_context->isf_pointer->lr        = 0x%08x\n\n",
            ((arm_interrupt_stack_frame_t *)partition_context->isf_pointer)->lr);
#endif

    /* memcpy(dest, src, size) */
    memcpy(context, partition_context, sizeof(core_context_t));

#ifdef PMK_DEBUG
    printk("after context restore\n"
            "context->isf_pointer->lr                  = 0x%08x\n",
            ((arm_interrupt_stack_frame_t *)context->isf_pointer)->lr);
    printk("partition_context->isf_pointer->lr        = 0x%08x\n\n",
            ((arm_interrupt_stack_frame_t *)partition_context->isf_pointer)->lr);
#endif
#ifdef PMK_DEBUG
    printk("    : Done Restoring\n");
#endif
}
