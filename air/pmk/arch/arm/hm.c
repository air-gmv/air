/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file hm.c
 * \author lumm
 * \brief Health Monitor handler
 *
 * TODO better undef logic
 */

#include <hm.h>
#include <cp15.h>
#include <air.h>
#include <svc.h>

#ifdef PMK_DEBUG
#include <printk.h>
#endif

static air_uptr_t *arm_partition_hm_handler(air_u32_t id, pmk_core_ctrl_t *core);
static air_boolean_t arm_hm_undef_is_fpu(air_u32_t ret_addr, air_boolean_t is_T32);

/**
 * the PMK will call a cpu_hm_init(), so it is here for compatibility and
 * future is necessary
 */
void arm_hm_init(void) {}

air_uptr_t *arm_hm_handler(arm_interrupt_stack_frame_t *frame, pmk_core_ctrl_t *core) {

    arm_exception_e hm_id = frame->exception_name;
    air_uptr_t *ret = NULL;

    air_error_e error_id;
    air_u32_t fsr = 0, far = 0;

    /**
     * \note After taking a Data Abort exception, the state of the exclusive monitors is UNKNOWN.
     * Therefore ARM strongly recommends that the abort handling software performs a CLREX instruction,
     * or a dummy STREX instruction, to clear the monitor state.
     *
     * ARM Architecture Reference Manual ARMv7-A/R edition ARM DDI 0406C.d (ID040418)
     */
    __asm__ volatile ("clrex");

    switch(hm_id) {

    /*
     * spsr: SPSR
     * ret_addr: return addr
     */
    case ARM_EXCEPTION_UNDEF:

        /* FPU Lazy Switching */
        if (arm_hm_undef_is_fpu(*(air_uptr_t *)frame->ret_addr, (frame->ret_psr & ARM_PSR_T))) {
            error_id = AIR_FLOAT_ERROR;
        } else {
            error_id = AIR_UNIMPLEMENTED_ERROR;
        }

#ifdef PMK_DEBUG
        fsr = frame->ret_psr;
        far = frame->ret_addr;
#endif
        break;

    /*
     * fsr: ifsr
     * far: ifar
     */
    case ARM_EXCEPTION_PREF_ABORT:
        error_id = AIR_VIOLATION_ERROR;
        fsr = arm_cp15_get_IFSR();
        far = arm_cp15_get_IFAR();
        break;

    /*
     * fsr: dfsr
     * far: dfar
     */
    case ARM_EXCEPTION_DATA_ABORT:
        error_id = AIR_SEGMENTATION_ERROR;
        fsr = arm_cp15_get_DFSR();
        far = arm_cp15_get_DFAR();
        break;

    case ARM_EXCEPTION_FIQ:
        error_id = AIR_IO_ERROR;
        fsr = frame->ret_psr;
        far = frame->ret_addr;
        break;

    default:
        error_id = AIR_UNIMPLEMENTED_ERROR;
#ifdef PMK_DEBUG
        fsr = frame->ret_psr;
        far = frame->ret_addr;
#endif
    }

#ifdef PMK_DEBUG
    printk("HM event  ARM id %d  FSR = 0x%08x  FAR = 0x%08x  LR = 0x%08x\n",
            hm_id, fsr, far, (int)frame->ret_addr);
#endif

    pmk_hm_isr_handler(error_id);

    if (!core->context->trash)
        ret = arm_partition_hm_handler(hm_id, core);

#ifdef PMK_DEBUG
    if (ret != NULL)
        printk("hm_handler ret = 0x%x\n", (int)ret);
#endif

    return ret;
}

/**
 * virtualizes the HM handling of a partition
 * requires that the partition registers the its exception table base address
 * \param id architecture exception id
 * \param core pmk_core_ctrl_t
 * \return POS HM return address
 */
static air_uptr_t *arm_partition_hm_handler(air_u32_t id, pmk_core_ctrl_t *core) {

    arm_virtual_cpu_t *vcpu = &core->context->vcpu;

    air_uptr_t *vbar = vcpu->vbar;
    if (vbar == NULL) return NULL;

    air_u32_t psr = vcpu->psr;
    air_u32_t psr_a = ((psr & ARM_PSR_A) >> 8);

    air_uptr_t *ret_addr = NULL;

    if (!psr_a) {

        pmk_hm_event_t *hm_event = (pmk_hm_event_t *)core->context->hm_event;
        if (hm_event->nesting > 0) {

            vcpu->psr |= (ARM_PSR_A | ARM_PSR_I);

            switch (hm_event->error_id) {
            case AIR_POWER_ERROR:
                ret_addr = vbar + 0;
                break;

            case AIR_UNIMPLEMENTED_ERROR:
                ret_addr = vbar + 1;
                break;

            case AIR_VIOLATION_ERROR:
                ret_addr = vbar + 3;
                break;

            case AIR_SEGMENTATION_ERROR:
                ret_addr = vbar + 4;
                break;

            case AIR_IO_ERROR:
                // TODO not sure in this case
                ret_addr = vbar + 6;
                break;

            case AIR_FLOAT_ERROR:
                arm_restore_fpu(core->context->vfp_context);
                arm_syscall_rett(core);
                break;

            case AIR_ARITHMETIC_ERROR: //overflow
            case AIR_DIVISION_BY_0_ERROR: // /0
            default:
                break;
            }
        }
    }

    return ret_addr;
}

static air_boolean_t arm_hm_undef_is_fpu(air_u32_t instr, air_boolean_t is_T32) {

    air_boolean_t is_fpu = False;

    if (is_T32) {

        if ( ((instr & 0xef00) == 0xef00) ||
                ((instr & 0x0e10ef00) == 0x0a00ee00) ||
                ((instr & 0x0e00ee00) == 0x0a00ec00) ||
                ((instr & 0xff10) == 0xf900) ||
                ((instr & 0x0e10ef00) == 0x0a10ee00) ||
                ((instr & 0x0e00efe0) == 0x0a00ec40) )

            is_fpu = True;
    } else {
        if ( ((instr & 0xfe000000) == 0xf2000000) ||
                ((instr & 0x0f000e10) == 0x0e000a00) ||
                ((instr & 0x0e000e00) == 0x0c000a00) ||
                ((instr & 0xff100000) == 0xf4000000) ||
                ((instr & 0x0f000e10) == 0x0e000a10) ||
                ((instr & 0x0fe00e00) == 0x0c400a00) )

            is_fpu = True;
    }

    return is_fpu;
}
