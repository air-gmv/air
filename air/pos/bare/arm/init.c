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
#include <pprintf.h>

extern int main();
extern void bare_start();

void arm_pos_init(void) {

    pprintf("POS init :: pos_exceptions: 0x%08x\n", pos_exceptions);
    air_set_tbr((int)pos_exceptions);

    return;
}
