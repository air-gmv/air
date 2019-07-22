/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file smp.c
 * @author lumm
 * @brief Basic SMP support. not really :p, just for symbols xD
 */

#include <air.h>
#include <armv7.h>
#include <cpu.h>

extern int main();
extern void bare_start();

void arm_smp_init(void) {
    return;
}
