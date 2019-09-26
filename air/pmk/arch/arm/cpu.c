/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file cpu.c
 * \author lumm
 * \brief Contains the PMK abstractions to handle the ARM processor context
 */

#include <cpu.h>
#include <bsp.h>
#include <pmk.h>
#include <ipc.h>
#ifdef PMK_DEBUG
#include <printk.h>
#endif

/**
 * \brief This is the stack used by the multiple ARM operating modes
 *
 * \note
 * Separate sizes can be changed in cpu.h
 */
air_u8_t air_stack[STACK_SIZE * PMK_MAX_CORES];

/**
 * This function initializes the architecture dependent part of the core
 * context
 */
void core_context_init(core_context_t *context, air_u32_t id) {

    /* core id */
    context->vcpu.id = id;

    /* initialize the context */
    context->entry_point = NULL;
    context->isr_nesting_level = 1;

    /* allocate context stack */
    context->idle_isf_pointer =
            (void *)pmk_workspace_alloc(sizeof(arm_interrupt_stack_frame_t));

    context->isf_pointer = context->idle_isf_pointer;

#if PMK_FPU_SUPPORT
    /* allocate space to hold an FPU context */
    ((arm_interrupt_stack_frame_t *)context->isf_pointer)->vfp_context =
            (arm_vfp_context_t *) pmk_workspace_alloc(sizeof(arm_vfp_context_t));
#else
    ((arm_interrupt_stack_frame_t *)context->isf_pointer)->vfp_context =
            (arm_vfp_context_t *)NULL;
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
            (context->idle_isf_pointer + sizeof(arm_interrupt_stack_frame_t)));
    printk("    fpu_context at       "
            "0x%08x to 0x%08x\n",
            ((arm_interrupt_stack_frame_t *)context->isf_pointer)->vfp_context,
            ((arm_interrupt_stack_frame_t *)context->isf_pointer)->vfp_context
            + sizeof(arm_vfp_context_t));

#endif
}

/**
 * This function setups a core context the architecture dependent part of
 * an idle context
 */
void core_context_setup_idle(core_context_t *context) {

    /* initialize the virtual core */
    context->vcpu.psr = (ARM_PSR_T | ARM_PSR_SYS);
    context->vcpu.vbar = NULL;

    /* initial stack frame */
    arm_interrupt_stack_frame_t *isf =
            (arm_interrupt_stack_frame_t *)(context->idle_isf_pointer);

    /* setup the space for the 1st window and the restore point */
    context->isf_pointer = context->idle_isf_pointer;

    /* this context doesn't need to be saved */
    context->trash = 1;
    context->isr_nesting_level = 1;

    /* setup the context return PSR */
    isf->ret_psr = (ARM_PSR_T | ARM_PSR_SYS);

    /* setup the context entry point */
#ifdef PMK_DEBUG
    printk("\n    isf at               "
            "0x%08x\n",
            isf);
    printk("    bsp_idle_loop at     "
            "0x%08x\n",
            bsp_idle_loop);
#endif
    isf->ret_addr = (air_u32_t)bsp_idle_loop + 4;
    isf->usr_lr = (air_u32_t)bsp_idle_loop + 4;
}

/**
 * This function setups a core context with the architecture dependent
 * configuration for a partition
 */
void core_context_setup_partition(
        core_context_t *context, pmk_partition_t *partition){

    /* initialize the virtual core */
    context->vcpu.psr = (ARM_PSR_USR);
    context->vcpu.vbar = NULL;

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
            isf->ret_psr = (ARM_PSR_SYS);
        } else {
            isf->ret_psr = (ARM_PSR_USR);
        }

        if ((partition->permissions & AIR_PERMISSION_FPU_CONTROL) != 0) {
            isf->vfp_context->fpexc = (ARM_VFP_FPEXC_ENABLE);
        } else {
            isf->vfp_context->fpexc = 0;
        }

        /* setup the partition entry point */
        isf->ret_addr = (air_u32_t)context->entry_point + 4;

#ifdef PMK_DEBUG
        printk("       cpu::setup::context->entry_point   = 0x%x\n"
                "       cpu::setup::isf->ret_addr          = 0x%x\n",
                context->entry_point,
                isf->ret_addr);
#endif

        /* setup the stack pointer of the partition */
        air_u32_t stack =
                (air_u32_t)partition->mmap->v_addr + partition->mmap->size;
        stack = (stack & ~(32 - 1));
        isf->usr_sp = stack - 4;
#ifdef PMK_DEBUG
        printk("       cpu::setup::stack                  = 0x%x\n"
                "       cpu::setup::isf->usr_sp            = 0x%x\n",
                stack, isf->usr_sp);
#endif

        // TODO something about cache
    } else {

        core_context_setup_idle(context);
    }
}

/**
 * TODO This function setups a core context the architecture dependent part of
 * an reload context
 */
void core_context_setup_reload_partition(core_context_t *context, pmk_partition_t *partition) {


}

void core_context_add_hm_event(core_context_t *context, air_state_e state_id, air_error_e error_id) {

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
