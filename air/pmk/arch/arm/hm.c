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

void arm_hm_handler(air_u32_t id, air_u32_t instr, air_u32_t status) {

    air_error_e error_id;
    if (id == ARM_EXCEPTION_UNDEF) {

#ifdef PMK_DEBUG
        printk("\n $$$ Undefined instr at 0x%x $$$\n\n", instr - 4);
#endif
        error_id = AIR_UNIMPLEMENTED_ERROR;
    } else if (id == ARM_EXCEPTION_PREF_ABORT) {

#ifdef PMK_DEBUG
        printk("\n $$$ Prefetch abort at 0x%x $$$\n\n", instr);
#endif
        error_id = AIR_VIOLATION_ERROR;
    } else if (id == ARM_EXCEPTION_DATA_ABORT) {

#ifdef PMK_DEBUG
        printk("\n $$$ Data abort at 0x%x $$$\n\n", instr);
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
}
