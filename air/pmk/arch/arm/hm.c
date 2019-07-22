/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file exception_handler.c
 * @author lumm
 * @brief exception_handlers and default
 */

#include <armv7.h>
#include <isr.h>
#include <pmk.h>
#ifdef PMK_DEBUG
#include <printk.h>
#endif

void arm_healthmonitor_init(void) {}

air_uptr_t arm_partition_hm_handler(air_u32_t id, pmk_core_ctrl_t *core) {

    arm_virtual_cpu_t *vcpu = &core->context->vcpu;
    arm_virtual_gic_t *vgic = &core->context->vgic;

    air_u32_t psr = vcpu->psr;
    air_u32_t psr_a = (psr & ARM_PSR_A);
    air_u32_t **vbar = vcpu->vbar;

    air_u32_t isr = vgic->isr;
    air_u32_t iccpmr = vgic->iccpmr;
    air_uptr_t icdiser = vgic->icdiser;
    air_uptr_t icdispr = vgic->icdispr;
    air_uptr_t icdipr = vgic->icdipr;

    if (!psr_a && (vbar != NULL)) {

        pmk_hm_event_t *hm_event = (pmk_hm_event_t *)core->context->hm_event;
        if (hm_event->nesting > 0 && vbar[1] && vbar[3] && vbar[4]) {

            vcpu->psr |= (ARM_PSR_A | ARM_PSR_I);
            vgic->icciar = ((core->idx << 10) & id);
            switch (hm_event->error_id) {
            case AIR_UNIMPLEMENTED_ERROR:
                return vbar[1];

            case AIR_VIOLATION_ERROR:
                return vbar[3];

            case AIR_SEGMENTATION_ERROR:
                return vbar[4];

            case AIR_IO_ERROR:
                // TODO not sure in this case
                return vbar[6];

            default:
                return NULL;
            }
        }

        if (arm_is_vint_enabled(icdiser, id)) {

            if (arm_get_vint_priority(icdipr, id) < iccpmr) {

                vcpu->psr |= (ARM_PSR_A | ARM_PSR_I);
                return vbar[6];

            } else {
                arm_set_vint_pending(icdispr, id);
            }
        }
    } else {

        arm_set_vint_pending(icdispr, id);
    }

    return NULL;
}

void arm_hm_handler(
        air_u32_t id, air_u32_t instr,
        air_u32_t status, pmk_core_ctrl_t *core) {

    /* Return Value */
    air_uptr_t ret = NULL;
    air_error_e error_id;

    if (id == ARM_EXCEPTION_UNDEF) {

#ifdef PMK_DEBUG
        printk("\n $$$ Undefined instr at 0x%08x : 0x%04x $$$\n\n",
                instr, (air_u16_t)*(air_u16_t *)instr);
#endif
        error_id = AIR_UNIMPLEMENTED_ERROR;

    } else if (id == ARM_EXCEPTION_PREF_ABORT) {

#ifdef PMK_DEBUG
        printk("\n $$$ Prefetch abort at 0x%08x $$$\n\n", instr);
#endif
        error_id = AIR_VIOLATION_ERROR;

    } else if (id == ARM_EXCEPTION_DATA_ABORT) {

#ifdef PMK_DEBUG
        printk("\n $$$ Data abort at 0x%08x $$$\n\n", instr);
#endif
        error_id = AIR_SEGMENTATION_ERROR;

    } else if (id == ARM_EXCEPTION_FIQ){

#ifdef PMK_DEBUG
        printk("\n $$$ FIIIIIIIIIIQ? $$$\n\n");
#endif
        error_id = AIR_IO_ERROR;

    } else {

#ifdef PMK_DEBUG
        printk("\n $$$ HOW? $$$\n\n");
#endif
        error_id = AIR_UNIMPLEMENTED_ERROR;
    }

    pmk_hm_isr_handler(error_id);

    ret = arm_partition_hm_handler(id, core);

    return ret;
}
