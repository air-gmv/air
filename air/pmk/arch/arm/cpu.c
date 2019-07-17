/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file cpu.c
 * @author lumm
 * @brief Contains the PMK abstractions to handle the ARM processor context
 */

#include <cpu.h>
#include <bsp.h>
#include <pmk.h>
#include <ipc.h>
#define N 1 // nesting level

/**
 * @brief Initializes the core context
 * @param context the core context to be initialized
 * @param id virtual core id;
 *
 * This function initializes the architecture dependent part of the core context
 */
void core_context_init(core_context_t *context, air_u32_t id) {

    /* core id */
    context->vcpu.id = id;

    /* initialize the context */
    context->entry_point = NULL;
    context->isr_nesting_level = 1;

    /* allocate context stack */
    context->idle_isf_pointer =
            (void *)pmk_workspace_alloc(N*sizeof(arm_interrupt_stack_frame_t));

#if PMK_FPU_SUPPORT
    /* allocate space to hold an FPU context */
    context->fpu_context = (arm_vfp_context_t *) \
            pmk_workspace_alloc(sizeof(arm_vfp_context_t));
#else
    context->fpu_context = (arm_vfp_context_t *)NULL;
#endif

    /* initialize the IPC event */
    context->ipc_event = PMK_IPC_NO_ACTION;

    /* initialize the System State and HM event */
    context->state = AIR_STATE_MODULE_EXEC;
    pmk_hm_event_t *hm_event = (pmk_hm_event_t *) \
            pmk_workspace_alloc(sizeof(pmk_hm_event_t));
    context->hm_event = hm_event;
    hm_event->nesting = 0;

#ifdef PMK_DEBUG

    printk("    :: context: %02i (0x%08x)\n",
            id, context);
    printk("       stack: [0x%08x : 0x%08x]\n",
            stack_space, context->isf_stack_pointer);
    printk("         fpu: [0x%08x : 0x%08x]\n",
            context->fpu_context, context->fpu_context +
            sizeof(arm_vfp_context_t));

#endif
}

/**
 * @brief Setups an idle context
 * @param context the core context to be set as idle
 *
 * This function setups a core context the architecture dependent part of
 * an idle context
 */
void core_context_setup_idle(core_context_t *context) {

    /* initialize the virtual core */
    context->vcpu.psr = 0;
    context->vcpu.tbr = 0;
    context->vcpu.ipend = 0;
    context->vcpu.imask = 0;
    context->vcpu.mmu_ctrl = 1;
    context->vcpu.mmu_fsr = 0;

    /* initial stack frame */
    arm_interrupt_stack_frame_t *isf =
            (arm_interrupt_stack_frame_t *)(context->idle_isf_pointer);

    /* setup the space for the 1st window and the restore point */
    context->isf_pointer = (void *)isf;

    /* this context doesn't need to be saved */
    context->trash = 1;
    context->isr_nesting_level = 1;

    /* setup the context return PSR */
    isf->orig_cpsr = ARM_PSR_USR;

    /* setup the context entry point */
    isf->lr = (air_uptr_t)bsp_idle_loop;
}
