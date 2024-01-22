/*
 * Copyright (C) 2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief Contains the PMK abstractions to handle the SPARC processor context
 */

#include <cpu.h>
#include <ipc.h>
#include <pmk.h>
#include <bsp.h>
#include <sparc.h>
#include <partition.h>
#include <workspace.h>
#ifdef PMK_DEBUG
#include <printk.h>
#endif

/**
 * @brief Stack size allocated for each core context
 * @ingroup cpu_sparc
 */
#define CONTEXT_STACK_SIZE (16 * (0x60 + 0x58))

/**
 * @brief SPARC Initial stack
 * @ingroup cpu_sparc
 */
air_u8_t sparc_initial_stack[SPARC_STACK_MINIMUM_SIZE * PMK_MAX_CORES];

/**
 * @brief Initializes the core context
 * @param context the core context to be initialized
 * @param id virtual core id;
 *
 * This function initializes the architecture dependent part of the core context
 */
void core_context_init(core_context_t *context, air_u32_t id)
{

    /* core id */
    context->vcpu.id = id;

    /* initialize the context */
    context->entry_point = NULL;
    context->isr_nesting_level = 1;

    /* allocate context stack */
    air_uptr_t stack_space = (air_uptr_t)pmk_workspace_alloc(CONTEXT_STACK_SIZE);
    context->isf_stack_pointer =
        (void *)((air_u32_t)stack_space + CONTEXT_STACK_SIZE - sizeof(sparc_interrupt_stack_frame_t));

#if PMK_FPU_SUPPORT
    /* allocate space to hold an FPU context */
    context->fpu_context = (sparc_fpu_context_t *)pmk_workspace_alloc(sizeof(sparc_fpu_context_t));

    memset(context->fpu_context, 0, sizeof(sparc_fpu_context_t));
    context->fpu_context->fsr = 0x00400000;
#else
    context->fpu_context = (sparc_fpu_context_t *)NULL;
#endif

    /* initialize the IPC event */
    context->ipc_event = PMK_IPC_NO_ACTION;

    /* initialize the System State and HM event */
    context->state = AIR_STATE_MODULE_EXEC;
    pmk_hm_event_t *hm_event = (pmk_hm_event_t *)pmk_workspace_alloc(sizeof(pmk_hm_event_t));
    context->hm_event = hm_event;
    hm_event->nesting = 0;

#ifdef PMK_DEBUG

    printk("    :: context: %02i (0x%08x)\n", id, context);
    printk("       stack: [0x%08x : 0x%08x]\n", stack_space, context->isf_stack_pointer);
    printk("         fpu: [0x%08x : 0x%08x]\n", context->fpu_context,
           (air_uptr_t)context->fpu_context + sizeof(sparc_fpu_context_t));
    printk("          hm: [0x%08x : 0x%08x]\n", context->hm_event, context->hm_event + sizeof(pmk_hm_event_t));

#endif
}

/**
 * @brief Setups an idle context
 * @param context the core context to be set as idle
 *
 * This function setups a core context the architecture dependent part of
 * an idle context
 */
void core_context_setup_idle(core_context_t *context)
{

    /* initialize the virtual core */
    context->vcpu.psr = 0;
    context->vcpu.tbr = 0;
    context->vcpu.ipend = 0;
    context->vcpu.imask = 0;
    context->vcpu.mmu_ctrl = 1;
    context->vcpu.mmu_fsr = 0;

    /* initial stack frame */
    sparc_interrupt_stack_frame_t *isf = (sparc_interrupt_stack_frame_t *)(context->isf_stack_pointer);

    /* setup the space for the 1st window and the restore point */
    isf->i6_fp = (air_u32_t)isf;
    context->stack_pointer = (void *)isf;

    /* this context doesn't need to be saved */
    context->trash = 1;
    context->isr_nesting_level = 1;

    /* setup the context return PSR */
    isf->psr = SPARC_PSR_S_MASK;
    isf->psr |= SPARC_PSR_PS_MASK; /* the idle loop is in the PMK */

    /* setup the context entry point */
    isf->pc = (air_u32_t)bsp_idle_loop;
    isf->nkpc = (air_u32_t)bsp_idle_loop + 0x00000004;
}

/**
 * @brief Setups a reload partition context
 * @param context the core context responsible for the reload
 * @param partition the partition to be reloaded
 *
 * This function setups a core context the architecture dependent part of
 * an reload context
 */
void core_context_setup_reload_partition(core_context_t *context, pmk_partition_t *partition)
{

    /* initialize the virtual core */
    context->vcpu.psr = 0;
    context->vcpu.tbr = 0;
    context->vcpu.ipend = 0;
    context->vcpu.imask = 0;
    context->vcpu.mmu_ctrl = 1;
    context->vcpu.mmu_fsr = 0;

    /* initial stack frame */
    sparc_interrupt_stack_frame_t *isf = (sparc_interrupt_stack_frame_t *)(context->isf_stack_pointer);

    /* setup the space for the 1st window and the restore point */
    isf->i6_fp = (air_u32_t)isf;
    context->stack_pointer = (void *)isf;

    context->trash = 0;

    /* The next ISR is handled as if it's interrupting an ISR */
    context->isr_nesting_level = 2;

    /* setup the context return PSR */
    isf->psr = SPARC_PSR_S_MASK;
    isf->psr |= SPARC_PSR_PS_MASK; /* the reload funtion is in the PMK */

    /* setup the context entry point */
    isf->pc = (air_u32_t)pmk_partition_reload;
    isf->nkpc = (air_u32_t)pmk_partition_reload + 0x00000004;
    isf->i0 = (air_u32_t)partition;
}

/**
 * @brief Setups a core partition context
 * @param context core context
 * @param partition partition information
 */
void core_context_setup_partition(core_context_t *context, pmk_partition_t *partition)
{

    /* initialize the virtual core */
    context->vcpu.psr = 0;
    context->vcpu.tbr = 0;
    context->vcpu.ipend = 0;
    context->vcpu.imask = 0;
    context->vcpu.mmu_ctrl = 1;
    context->vcpu.mmu_fsr = 0;

    /* initialize the System State and HM event */
    context->state = AIR_STATE_PARTITION_INIT;
    pmk_hm_event_t *hm_event = (pmk_hm_event_t *)context->hm_event;
    hm_event->nesting = 0;

    if (context->entry_point != NULL)
    {

        /* initial stack frame */
        sparc_interrupt_stack_frame_t *isf = (sparc_interrupt_stack_frame_t *)(context->isf_stack_pointer);

        context->trash = 0;

        /* mark the partition context as the ISF */
        context->stack_pointer = isf;

        /* at start ISR level is one */
        context->isr_nesting_level = 1;

        /* setup partition real PSR */
        isf->psr = SPARC_PSR_S_MASK;

        /* check if the partition have supervisor permissions*/
        /* This check should be whether it's a System_Partition or not (TODO)*/
        if (partition->permissions == AIR_PERMISSION_SUPERVISOR)
        {

            isf->psr |= SPARC_PSR_PS_MASK;
        }

#if PMK_FPU_SUPPORT
        /* check if the partition have floating point permissions */
        if ((partition->permissions & AIR_PERMISSION_FPU_CONTROL) != 0)
        {

            /* enable FPU and enable virtual FPU */
            isf->psr |= SPARC_PSR_EF_MASK;
            context->vcpu.psr |= SPARC_PSR_EF_MASK;
        }
#endif

        /* setup the partition entry point */
        isf->pc = (air_u32_t)context->entry_point;
        isf->nkpc = (air_u32_t)context->entry_point + 0x04;

        /* setup the stack pointer of the partition */
        air_u32_t stack_high = (air_uptr_t)partition->mmap->v_addr + partition->mmap->size;
        stack_high &= ~(SPARC_STACK_ALIGNMENT - 1);
        isf->i6_fp = stack_high;

        /* setup the initial cache state */
        switch (partition->init_cache)
        {
        case AIR_CACHE_ALL:
            context->vcpu.cctrl = 0xF | 0x800000;
            break;
        case AIR_CACHE_DATA:
            context->vcpu.cctrl = 0xC | 0x800000;
            break;
        case AIR_CACHE_INSTRUCTION:
            context->vcpu.cctrl = 0x3 | 0x800000;
            break;
        default:
            context->vcpu.cctrl = 0 | 0x800000;
            break;
        }
    }
    else
    {

        core_context_setup_idle(context);
    }
}

/**
 * @brief Flag an HM event on a core context
 * @param context core context
 * @param state_id current system state
 * @param error_id current error Id
 */
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

/**
 * @brief Removes an HM event from the core context
 * @param context Context of the core
 */
void core_context_remove_hm_event(core_context_t *context)
{

    /* get the current core HM event */
    pmk_hm_event_t *hm_event = (pmk_hm_event_t *)context->hm_event;

    /* consume current HM event */
    if ((hm_event->nesting > 0) != 0)
    {
        --hm_event->nesting;
        if (hm_event->nesting == 0)
        {
            hm_event->state_id = hm_event->previous_state_id;
            context->state = hm_event->state_id;
        }
    }
}
