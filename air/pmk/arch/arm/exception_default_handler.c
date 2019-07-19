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

#include <isr.h>
#ifdef PMK_DEBUG
#include <printk.h>
#endif

void arm_exception_default_handler(air_u32_t instr, air_u32_t id) {

#ifdef PMK_DEBUG
    if (id == 3) {
        printk("\n $$$ Prefetch abort at 0x%x $$$\n\n", instr);
    } else if (id == 4) {
        printk("\n $$$ Data abort at 0x%x $$$\n\n", instr);
    } else {
        printk("\n $$$ HOW? $$$\n\n");
    }
#endif

    return;
}
