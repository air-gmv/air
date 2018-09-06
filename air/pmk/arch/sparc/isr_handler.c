/**
 * @file
 * @author pfnf
 * @brief
 */

#include <air.h>
#include <pmk.h>
#include <cpu.h>
#include <bsp.h>
#include <ipc.h>
#include <sparc.h>
#include <syscall.h>
#include <multicore.h>
#include <health_monitor.h>
#include <configurations.h>

/**
 * @brief SPARC ISR vector
 */
air_u32_t sparc_isr_vector[SPARC_TRAP_COUNT];

typedef void (*pmk_handling_function)(void *, pmk_core_ctrl_t*);

/**
 * @brief Get the partition trap handler address
 * @param ctx partition current CPU core context
 * @param tn trap/interrupt number
 */
static void *sparc_partition_isr_virtualization(
        sparc_interrupt_stack_frame_t *isf,
        pmk_core_ctrl_t *core_ctrl) {

    /* get core context */
    core_context_t *core_ctx = core_ctrl->context;
    /* get the trap number from the ISF */
    air_u32_t tn = SPARC_REAL_TRAP_NUMBER(isf->tpc);

    if (tn == BSP_IPC_PCS) {
        isf->tpc = timer_ctrl.irq;
        tn = timer_ctrl.irq;
    }
    else
        if(tn == BSP_IPC_IRQ)
            return NULL;

    /* get an easy pointer to the virtual core structure */
    sparc_virtual_cpu_t *vcpu = &core_ctx->vcpu;

    /* get virtual ET, PIL  */
    air_u32_t *tbr    = (air_u32_t *)vcpu->tbr;
    air_u32_t psr_et  = (vcpu->psr & SPARC_PSR_ET_MASK) >> SPARC_PSR_ET_BIT_POSITION;
    air_u32_t psr_pil = (vcpu->psr & SPARC_PSR_PIL_MASK) >> SPARC_PSR_PIL_BIT_POSITION;

    /* check if the partition can handle traps */
    if (psr_et != 0 && tbr != NULL) {

        /* flag the partition context switch event
        if (core_ctrl->partition_switch == 1) {
            if (tbr[0x80 + AIR_SYSCALL_PS_TRAP] != 0) {
                vcpu->psr &= ~SPARC_PSR_ET_MASK;
                isf->tpc = 0x80 + AIR_SYSCALL_PS_TRAP;
                return (void *)tbr[0x80 + AIR_SYSCALL_PS_TRAP];
            }
        }*/

        /* check if any Health-Monitor event is pending */
        pmk_hm_event_t *hm_event = (pmk_hm_event_t *)core_ctx->hm_event;
        if (hm_event->nesting > 0 && tbr[AIR_SYSCALL_HM_TRAP] != 0) {

            vcpu->psr &= ~SPARC_PSR_ET_MASK;
            isf->tpc = AIR_SYSCALL_HM_TRAP;
            return (void *)tbr[AIR_SYSCALL_HM_TRAP];
        }

        /* pass trap to the guest OS */
        if (tbr[tn] != 0) {

            /* check if we are handling an interrupt */
            if (tn > 0x10 && tn < 0x1F && (tn - 0x10) <= psr_pil) {

                vcpu->ipend |= (1 << (tn - 0x10));
                return NULL;

            } else {

                /* disable virtual traps */
                vcpu->psr &= ~SPARC_PSR_ET_MASK;
                return (void *)tbr[tn];
            }
        }

    } else {

        if (tn > 0x10 && tn < 0x1F) {

            vcpu->ipend |= (1 << (tn - 0x10));
        }
    }

    /* no handler is set */
    return NULL;
}

/**
 * @brief
 */
void *sparc_isr_handler(
        sparc_interrupt_stack_frame_t *isf,
        pmk_core_ctrl_t *core,
        air_u32_t tn) {

    void *ret = NULL;

    /* call the handling function */
    tn = SPARC_REAL_TRAP_NUMBER(tn);
    if (sparc_isr_vector[tn] != (air_uptr_t)NULL) {

        ((pmk_handling_function)sparc_isr_vector[tn])((void *)isf, core);
    }

    /* core point may change if the execution was preempted */
    core = pmk_get_current_core_ctrl();

    /* check if context switch was performed */
    if (core->partition_switch == 1) {

        /* load the new ISF from the new context */
        isf = core->context->stack_pointer;
    }

    /* route trap to the current partition */
    if (core->partition != NULL &&
        core->context->isr_nesting_level == 1) {

        ret = sparc_partition_isr_virtualization(isf, core);
    }

    /* clear context switch flag */
    core->partition_switch = 0;

    /* return ISF */
    return ret;
}
