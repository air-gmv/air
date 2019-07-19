/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file exceptions.S
 * @author lumm
 * @brief Exception handlers
 */

/* Are we in assembly file */
#ifdef ASM

#include <asm.h>
#include <armv7.h>

    .extern arm_exception_default_handler

/* Save global registers and branch to save context */
FUNC(exception_undef)
FUNC(exception_pref_abort)
FUNC(exception_data_abort)

    push    {r0-r2, lr}

    /* Call high level handler */
    SWITCH_FROM_ARM_TO_THUMB    r1
    bl      arm_exception_default_handler
    SWITCH_FROM_THUMB_TO_ARM

    pop     {r0-r2, lr}
    subs    pc, lr, #4

#endif /* ASM */
