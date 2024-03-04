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

#include <armv7.h>
#include <cpu.h>
#include <bsp.h>
#include <workspace.h>
#include <air.h>
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
 * \brief Interrupt stack size for each core context
 */
#define SIZEOF_ISF (MAX_NESTING_LEVEL + 1) * sizeof(arm_interrupt_stack_frame_t)

/**
 * This function initializes the architecture dependent part of the core
 * context
 */
void core_context_init(core_context_t *context, air_u32_t id)
{

    /* core id */
    context->vcpu.id = id;

    /* initialize the context */
    context->entry_point = NULL;
    context->isr_nesting_level = 0;

    /* allocate context stack */
    air_uptr_t isf = (air_uptr_t)((air_u32_t)pmk_workspace_alloc(SIZEOF_ISF) + SIZEOF_ISF);

    context->idle_isf_pointer = (void *)(isf - sizeof(arm_interrupt_stack_frame_t));
    context->isf_pointer = context->idle_isf_pointer;

#if PMK_FPU_SUPPORT
    arm_enable_fpu();
    /* allocate space to hold an FPU context */
    context->vfp_context = (arm_vfp_context_t *)pmk_workspace_alloc(sizeof(arm_vfp_context_t));
#else
   context->vfp_context = (arm_vfp_context_t *)NULL;
#endif

    /* initialize the IPC event */
    context->ipc_event = PMK_IPC_NO_ACTION;

    /* initialize the System State and HM event */
    context->state = AIR_STATE_MODULE_EXEC;

    pmk_hm_event_t *hm_event = (pmk_hm_event_t *)pmk_workspace_alloc(sizeof(pmk_hm_event_t));
    context->hm_event = hm_event;
    hm_event->nesting = 0;

#ifdef PMK_DEBUG
    printk("\n    :: context %02i at           0x%08x\n", id, context);
    printk("       idle_isf_pointer at    0x%08x to 0x%08x\n", context->idle_isf_pointer,
           context->idle_isf_pointer + sizeof(arm_interrupt_stack_frame_t));
     printk("       fpu_context at         0x%08x to 0x%08x\n",
             context->vfp_context,
             context->vfp_context + sizeof(arm_vfp_context_t));
    printk("       hm_event at            0x%08x to 0x%08x\n", context->hm_event,
           context->hm_event + sizeof(pmk_hm_event_t));
#endif
}

/**
 * This function setups a core context the architecture dependent part of
 * an idle context
 */
void core_context_setup_idle(core_context_t *context)
{

    /* initialize the virtual core */
    context->vcpu.psr = 0;
    context->vcpu.vbar = NULL;

    /* this context doesn't need to be saved */
    context->trash = 1;
    context->isr_nesting_level = 0;

    /* initialize idle stack */
    /* setup the isf used in restoring context equal to the idle context */
    arm_interrupt_stack_frame_t *isf = (arm_interrupt_stack_frame_t *)context->idle_isf_pointer;

    /* setup the context return PSR */
    isf->ret_psr = (ARM_PSR_T | ARM_PSR_SYS);

    /* setup the context entry point */
    isf->ret_addr = (air_u32_t)bsp_idle_loop;
    isf->usr_lr = (air_u32_t)bsp_idle_loop;
}

/**
 * This function setups a core context with the architecture dependent
 * configuration for a partition
 */
void core_context_setup_partition(core_context_t *context, pmk_partition_t *partition)
{

    /* initialize the virtual core */
    context->vcpu.psr = (ARM_PSR_SVC);
    context->vcpu.vbar = NULL;

    /* initialize the System State and HM event */
    context->state = AIR_STATE_PARTITION_INIT;

    pmk_hm_event_t *hm_event = (pmk_hm_event_t *)context->hm_event;
    hm_event->nesting = 0;

    if (context->entry_point != NULL)
    {

        arm_interrupt_stack_frame_t *isf =
            (arm_interrupt_stack_frame_t *)((air_u32_t)context->idle_isf_pointer - sizeof(arm_interrupt_stack_frame_t));

        /* initial stack frame */
        context->isf_pointer = (void *)isf;

        context->trash = 0;

        /* at start ISR level is one */
        context->isr_nesting_level = 1;

        /* setup partition real PSR */
        /* check if the partition have supervisor permissions */
        if (partition->permissions == AIR_PERMISSION_SUPERVISOR) {

            isf->ret_psr = (ARM_PSR_SYS);
            context->vcpu.psr = (ARM_PSR_SYS);
        }
        else
        {
            isf->ret_psr = (ARM_PSR_USR);
        }
#if PMK_FPU_SUPPORT
        if ((partition->permissions & AIR_PERMISSION_FPU_CONTROL) != 0) {
            context->vfp_context->fpexc = (ARM_VFP_FPEXC_ENABLE);
            context->vfp_context->fpscr &=0xFFF8FFFF;
            //isf->vfp_context.fpexc = (ARM_VFP_FPEXC_ENABLE);
        }
        else
        {
            context->vfp_context->fpexc = 0;
            //isf->vfp_context.fpexc = 0;
        }
#endif
        /* setup the partition entry point */
        isf->ret_addr = (air_u32_t)context->entry_point;

#ifdef PMK_DEBUG
        printk("       cpu::setup::context->entry_point   = 0x%x\n"
               "       cpu::setup::isf->ret_addr          = 0x%x\n"
               "       cpu::setup::isf->ret_psr           = 0x%x\n",
               context->entry_point, isf->ret_addr, isf->ret_psr);
#endif

        /* setup the stack pointer of the partition */
        air_u32_t stack = (air_u32_t)partition->mmap->v_addr + (partition->mmap->size);
        stack = (stack & ~(32 - 1));
        isf->usr_sp = stack;
        /*initialize virtual stack pointers*/
        context->virt.sp_svc = stack;

        context->virt.sp_irq = stack - ((partition->mmap->size) / 2);

        //get original SP (from before exception handler)
        int sp_original = arm_get_sp() + 0xA8;  // 0xA8 seems to be the stack that is pushed by the C handler and is popped until we get back to exception.S
        //store SVC SP, adding space for each partition, in core context
        context->virt.svc_sp = sp_original - partition->id * (0x600); //TODO: magic number, to review

        /*Enable virtual interrupts*/
        context->vgic.vm_ctrl = 1;
        /*Set the priority mask so that all the interrupts are enabled*/
        context->vgic.pmr = 255;

#ifdef PMK_DEBUG
        printk("       cpu::setup::stack                  = 0x%x\n"
               "       cpu::setup::isf->usr_sp            = 0x%x\n",
               stack, isf->usr_sp);
#endif

        // TODO something about cache permissions
    }
    else
    {

        core_context_setup_idle(context);
    }
}

/**
 * TODO This function setups a core context the architecture dependent part of
 * an reload context
 */
void core_context_setup_reload_partition(core_context_t *context, pmk_partition_t *partition)
{

    /* initialize the virtual core */
    context->vcpu.psr = 0;
    context->vcpu.vbar = NULL;

    /* setup the isf used in restoring context equal to the idle context */
    arm_interrupt_stack_frame_t *isf =
        (arm_interrupt_stack_frame_t *)((air_u32_t)context->idle_isf_pointer - sizeof(arm_interrupt_stack_frame_t));

    context->isf_pointer = (void *)isf;

    /* this context doesn't need to be saved (but needs to be restore duh!X)*/
    context->trash = 1;
    context->isr_nesting_level = 1;

    /* setup the reload entry point */
    isf->ret_psr = (ARM_PSR_T | ARM_PSR_SYS);
    isf->ret_addr = (air_u32_t)pmk_partition_reload;
    isf->r0 = (air_u32_t)partition;
}

void core_context_add_hm_event(core_context_t *context, air_state_e state_id, air_error_e error_id)
{

    /* get the current core HM event */
    pmk_hm_event_t *hm_event = (pmk_hm_event_t *)context->hm_event;

    /* save the current state if the HM event isn't nested */
    if (0 == hm_event->nesting)
    {

        hm_event->previous_state_id = context->state;
        context->state = AIR_STATE_PARTITION_HM;
    }

    /* increment the HM event nesting and save the state */
    ++hm_event->nesting;
    hm_event->error_id = error_id;
    hm_event->state_id = state_id;
}

void core_context_remove_hm_event(core_context_t *context)
{

    /* get the current core HM event */
    pmk_hm_event_t *hm_event = (pmk_hm_event_t *)context->hm_event;

    /* consume current HM event */
    if (--(hm_event->nesting) == 0)
    {
        hm_event->state_id = hm_event->previous_state_id;
        context->state = hm_event->state_id;
    }
}
