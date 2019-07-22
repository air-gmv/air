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
 * @brief POS exception handlers
 */

#include <air.h>
#include <bare.h>
#include <pprintf.h>

void pos_hm_undef(pos_hm_stack *hm_stack){

    pprintf("\n** POS UNDEFINED ERROR**\n");

    /* get HM event */
    air_hm_event_t hm_event;
    air_syscall_get_hm_event(&hm_event);
}

void pos_hm_pref_abort(pos_hm_stack *hm_stack){

    pprintf("\n** POS PREFETCH ERROR**\n");

    /* get HM event */
    air_hm_event_t hm_event;
    air_syscall_get_hm_event(&hm_event);
}

void pos_hm_data_abort(pos_hm_stack *hm_stack){

    pprintf("\n** POS DATA ERROR**\n");

    /* get HM event */
    air_hm_event_t hm_event;
    air_syscall_get_hm_event(&hm_event);
}
