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

void arm_svc_handler(arm_exception_frame_t *frame) {



    return;
}

