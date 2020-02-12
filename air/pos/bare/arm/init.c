/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file init.c
 * @author lumm
 * @brief Initialization functions, SMP support
 */

#include <air.h>
#include <bare.h>
#ifdef POS_DEBUG
#include <pprintf.h>
#endif

/**
 * @brief POS exception table
 */
extern volatile air_u32_t arm_pos_exceptions;
extern volatile air_uptr_t *isr_table[PMK_MAX_CORES];

void arm_pos_init(void) {
#ifdef POS_DEBUG
    pprintf("POS init :: pos_exceptions: 0x%08x\n", &arm_pos_exceptions);
    pprintf("POS init :: isr_table: 0x%08x\n", &isr_table);
#endif
    air_syscall_set_tbr((void *)&arm_pos_exceptions);

    arm_pos_smp_init();

    return;
}
