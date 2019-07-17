/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file start.S
 * @author lumm
 * @brief Basic bare OS start.S
 */

#ifndef ASM
#define ASM
#endif

#include <air.h>
#include <asm.h>
#include <armv7.h>


FUNC(bare_start)

    // todo svc
