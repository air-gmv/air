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

void arm_exception_default_handler(arm_interrupt_stack_frame_t *frame, pmk_core_ctrl_t* core) {
    //TODO terminate
    return;
}
