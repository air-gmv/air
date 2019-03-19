/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file asm.h
 * @author lumm
 * @brief Assembly macros to improve readability.
 */

#ifndef ARM_ASM_H
#define ARM_ASM_H

#ifdef ASM

#define global(label) \
    .globl label; \
    label:

#endif /* ASM */

#endif /* ARM_ASM_H */
