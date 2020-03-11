/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file exception.c
 * \author lumm
 * \brief Interrupt Service Routines (ISR)
 */

#include <isr.h>
#include <hm.h>
#include <svc.h>

#ifdef PMK_DEBUG_EXCEPTION
#include <printk.h>
#endif

air_uptr_t * arm_exception_handler(arm_interrupt_stack_frame_t *frame, pmk_core_ctrl_t *core) {

#ifdef PMK_DEBUG_EXCEPTION
    printk(" :: AIR arm_exception_handler with id %d\n", frame->exception_name);
#endif
    /* return value */
    air_uptr_t * ret = NULL;

    switch(frame->exception_name) {

        case ARM_EXCEPTION_UNDEF:
        case ARM_EXCEPTION_PREF_ABORT:
        case ARM_EXCEPTION_DATA_ABORT:
        case ARM_EXCEPTION_FIQ:
            ret = arm_hm_handler(frame, core);

        case ARM_EXCEPTION_SWI:
            arm_svc_handler(frame, core);

        case ARM_EXCEPTION_IRQ:
            ret = arm_isr_handler(frame, core);
    }

    return ret;
}
