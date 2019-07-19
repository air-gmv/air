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
#ifdef PMK_DEBUG
#include <printk.h>
#endif

#define N 1 // nesting level

/* stacks for the various modes */
air_u8_t air_stack[TOTAL_STACK_SIZE * PMK_MAX_CORES];

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

    context->isf_pointer = context->idle_isf_pointer;

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

    printk("    context %i at         "
            "0x%08x\n",
            id, context);
    printk("    idle_isf_pointer at  "
            "0x%08x to 0x%08x\n",
            context->idle_isf_pointer,
            (context->idle_isf_pointer + N*sizeof(arm_interrupt_stack_frame_t)));
    printk("    fpu_context at       "
            "0x%08x to 0x%08x\n",
            context->fpu_context,
            context->fpu_context + sizeof(arm_vfp_context_t));
    printk("    stack at             "
            "0x%08x to 0x%08x\n",
            &air_stack,
            &air_stack + sizeof(air_stack));

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

    /* initial stack frame */
    arm_interrupt_stack_frame_t *isf =
            (arm_interrupt_stack_frame_t *)(context->idle_isf_pointer);

    /* setup the space for the 1st window and the restore point */
    context->isf_pointer = context->idle_isf_pointer;

    /* this context doesn't need to be saved */
    context->trash = 1;
    context->isr_nesting_level = 1;

    /* setup the context return PSR */
    isf->orig_cpsr = (ARM_PSR_T | ARM_PSR_USR);

    /* setup the context entry point */
    isf->lr = (air_u32_t)bsp_idle_loop;
    isf->orig_lr = (air_u32_t)bsp_idle_loop;
}

/**
 * @brief Setups a core partition context
 * @param partition partition information
 * @param context core context
 */
void core_context_setup_partition(
        core_context_t *context, pmk_partition_t *partition){

    /* initialize the virtual core */
    context->vcpu.psr = (ARM_PSR_T | ARM_PSR_USR);
    context->vcpu.tbr = 0;
    context->vcpu.ipend = 0;
    context->vcpu.imask = 0;

    /* initialize the System State and HM event */
    context->state = AIR_STATE_PARTITION_INIT;
    pmk_hm_event_t *hm_event = (pmk_hm_event_t *)context->hm_event;
    hm_event->nesting = 0;

    if (context->entry_point != NULL) {

        /* initial stack frame */
        arm_interrupt_stack_frame_t *isf =
                (arm_interrupt_stack_frame_t *)(context->isf_pointer);

        context->trash = 0;

        /* at start ISR level is one */
        context->isr_nesting_level = 1;

        /* setup partition real PSR */
        /* check if the partition have supervisor permissions */
        if ((partition->permissions & AIR_PERMISSION_SUPERVISOR) != 0) {
            isf->orig_cpsr = (ARM_PSR_T | ARM_PSR_SYS);
        } else {
            isf->orig_cpsr = (ARM_PSR_T | ARM_PSR_USR);
        }

        /* setup the partition entry point */
        isf->lr = (air_u32_t)context->entry_point;

        /* setup the stack pointer of the partition */
        air_u32_t stack =
                (air_u32_t)partition->mmap->v_addr + partition->mmap->size;
        stack = (stack & ~(32 - 1));
        isf->orig_sp = stack;

        // TODO something about cache
    } else {

        core_context_setup_idle(context);
    }
}

/**
 * @brief Flag an HM event on a core context
 * @param context core context
 * @param state_id current system state
 * @param error_id current error Id
 */
void core_context_add_hm_event(
        core_context_t *context,
        air_state_e state_id,
        air_error_e error_id) {

    /* get the current core HM event */
    pmk_hm_event_t *hm_event = (pmk_hm_event_t *)context->hm_event;

    /* save the current state if the HM event isn't nested */
    if (0 == hm_event->nesting) {

        hm_event->previous_state_id = context->state;
        context->state = AIR_STATE_PARTITION_HM;
    }

    /* increment the HM event nesting and save the state */
    ++hm_event->nesting;
    hm_event->error_id = error_id;
    hm_event->state_id = state_id;
}

void core_context_remove_hm_event(core_context_t *context) {

    /* get the current core HM event */
    pmk_hm_event_t *hm_event = (pmk_hm_event_t *)context->hm_event;

    /* consume current HM event */
    --hm_event->nesting;
    if (hm_event->nesting == 0) {
        hm_event->state_id = hm_event->previous_state_id;
        context->state = hm_event->state_id;
    }
}
