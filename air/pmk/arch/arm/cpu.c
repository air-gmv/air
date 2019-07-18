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

#define DEFAULT_STACK_SIZE  0x1000
#define TOTAL_STACK_SIZE    7*DEFAULT_STACK_SIZE

/* stacks for the various modes */
air_u32_t svc_stack_size = DEFAULT_STACK_SIZE;
air_u32_t fiq_stack_size = DEFAULT_STACK_SIZE;
air_u32_t irq_stack_size = DEFAULT_STACK_SIZE;
air_u32_t abt_stack_size = DEFAULT_STACK_SIZE;
air_u32_t und_stack_size = DEFAULT_STACK_SIZE;
air_u32_t sys_stack_size = DEFAULT_STACK_SIZE;
air_u32_t mon_stack_size = DEFAULT_STACK_SIZE;
air_u32_t total_stack_size = TOTAL_STACK_SIZE;
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

/**
 * @brief Setups a core partition context
 * @param partition partition information
 * @param context core context
 */
void core_context_setup_partition(
        core_context_t *context, pmk_partition_t *partition){

    /* initialize the virtual core */
    context->vcpu.psr = 0;
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

        /* mark the partition context as the ISF */
        context->isf_pointer = isf;

        /* at start ISR level is one */
        context->isr_nesting_level = 1;

        /* setup partition real PSR */
        /* check if the partition have supervisor permissions */
        if ((partition->permissions & AIR_PERMISSION_SUPERVISOR) != 0) {
            isf->orig_cpsr = ARM_PSR_SYS;
        } else {
            isf->orig_cpsr = ARM_PSR_USR;
        }

        /* setup the partition entry point */
        isf->lr   = (air_uptr_t)context->entry_point;

        /* setup the stack pointer of the partition */
        air_uptr_t stack =
                (air_uptr_t)partition->mmap->v_addr + partition->mmap->size;
        stack = (air_uptr_t)((air_u32_t)stack & ~(32 - 1));
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
