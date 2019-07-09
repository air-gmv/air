/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file svc_handler.c
 * @author lumm
 * @brief exception_handlers and default
 */

#include <svc.h>

air_uptr_t arm_svc_table[MAX_SVC];

typedef air_u32_t (*syscall_function)(void);

void arm_svc_handler(arm_exception_frame_t *frame) {

    air_u32_t ret;
    air_u32_t syscall_number = frame->svc_imm;

    if (syscall_number < MAX_SVC) {
        ret = ((syscall_function) arm_svc_table[syscall_number])();
    }
    return;
}

