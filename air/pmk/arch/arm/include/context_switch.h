/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013-2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file context_switch.h
 * @author lumm
 * @brief Context Switch headers
 */

#include <armv7.h>

void arm_core_context_save(void *core);
void arm_core_context_restore(void *core);
