/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author lumm
 * @brief exception_handlers and default
 */

#include <armv7.h>
#include <isr.h>
#include <pmk.h>
#ifdef PMK_DEBUG
#include <printk.h>
#endif

void arm_hm_init(void) {}

air_uptr_t arm_partition_hm_handler(air_u32_t id, pmk_core_ctrl_t *core) {

    arm_virtual_cpu_t *vcpu = &core->context->vcpu;

    air_u32_t psr = vcpu->psr;
    air_u32_t psr_a = ((psr & ARM_PSR_A) >> 8);
    air_uptr_t vbar = vcpu->vbar;

#ifdef PMK_DEBUG_HM
    printk("entered partition_hm_handler. vbar = 0x%x, vbar+4 = 0x%x, vpsr = 0x%x\n", (int)vbar, (int)(vbar+4), psr);
#endif

    if (vbar == NULL)
        return NULL;

    if (!psr_a) {

        pmk_hm_event_t *hm_event = (pmk_hm_event_t *)core->context->hm_event;
        if (hm_event->nesting > 0) {

            vcpu->psr |= (ARM_PSR_A | ARM_PSR_I);

#ifdef PMK_DEBUG_HM
            printk("psr(A|I) = 0x%x, hm_event->error_id = %d\n", vcpu->psr, hm_event->error_id);
#endif

            switch (hm_event->error_id) {
            case AIR_POWER_ERROR:
                return vbar + 0;

            case AIR_UNIMPLEMENTED_ERROR:
                return vbar + 1;

            case AIR_VIOLATION_ERROR:
                return vbar + 3;

            case AIR_SEGMENTATION_ERROR:
                return vbar + 4;

            case AIR_IO_ERROR:
                // TODO not sure in this case
                return vbar + 6;

            case AIR_FLOAT_ERROR:
                arm_restore_fpu(((arm_interrupt_stack_frame_t *)(core->context->isf_pointer))->vfp_context);
                arm_syscall_rett(core);
                return NULL;

            case AIR_ARITHMETIC_ERROR: //overflow
            case AIR_DIVISION_BY_0_ERROR: // /0
            default:
                return NULL;
            }
        }
    }

    return NULL;
}

air_uptr_t arm_hm_handler(arm_exception_e id, air_u32_t fsr, air_u32_t far, air_u32_t *instr) {

    pmk_core_ctrl_t *core = (pmk_core_ctrl_t *)arm_cp15_get_Per_CPU();

    air_error_e error_id;
    air_uptr_t ret = NULL;

    if (id == ARM_EXCEPTION_UNDEF) {

#ifdef PMK_DEBUG
        printk("\n !!! Undefined Instruction at 0x%08x, *instr = 0x%08x, SPSR: 0x%08x !!!\n\n",
                instr, (air_u32_t)*(air_u32_t *)instr, fsr);
#endif
        error_id = AIR_UNIMPLEMENTED_ERROR;

        /* Lazy Switching. fsr is the spsr */
        if ((fsr && ARM_PSR_T)) {

            if ( ((*instr & 0xef00) == 0xef00) ||
                    ((*instr & 0x0e10ef00) == 0x0a00ee00) ||
                    ((*instr & 0x0e00ee00) == 0x0a00ec00) ||
                    ((*instr & 0xff10) == 0xf900) ||
                    ((*instr & 0x0e10ef00) == 0x0a10ee00) ||
                    ((*instr & 0x0e00efe0) == 0x0a00ec40) )
                error_id = AIR_FLOAT_ERROR;
        } else {
            if ( ((*instr & 0xfe000000) == 0xf2000000) ||
                    ((*instr & 0x0f000e10) == 0x0e000a00) ||
                    ((*instr & 0x0e000e00) == 0x0c000a00) ||
                    ((*instr & 0xff100000) == 0xf4000000) ||
                    ((*instr & 0x0f000e10) == 0x0e000a10) ||
                    ((*instr & 0x0fe00e00) == 0x0c400a00) )
                error_id = AIR_FLOAT_ERROR;
        }

    } else if (id == ARM_EXCEPTION_PREF_ABORT) {

#ifdef PMK_DEBUG
        printk("\n !!! Prefetch abort at 0x%08x, IFAR: 0x%08x, IFSR: 0x%08x !!!\n\n",
                instr, far, fsr);
#endif
        error_id = AIR_VIOLATION_ERROR;

    } else if (id == ARM_EXCEPTION_DATA_ABORT) {

#ifdef PMK_DEBUG
        printk("\n !!! Data abort at 0x%08x (<10000), DFAR: 0x%08x, DFSR: 0x%08x !!!\n\n",
                instr, far, fsr);
#endif
        error_id = AIR_SEGMENTATION_ERROR;

    } else if (id == ARM_EXCEPTION_FIQ){

#ifdef PMK_DEBUG
        printk("\n !!! FIIIIIIIIIIQ? !!!\n\n");
#endif
        error_id = AIR_IO_ERROR;

    } else {

#ifdef PMK_DEBUG
        printk("\n !!! HOW? !!!\n\n");
#endif
        error_id = AIR_UNIMPLEMENTED_ERROR;
    }

    pmk_hm_isr_handler(error_id);

    if (!core->context->trash)
        ret = arm_partition_hm_handler(id, core);

#ifdef PMK_DEBUG
    printk("hm_handler ret = 0x%x\n", (int)ret);
#endif

    return ret;
}
