/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file lock.h
 * @author lumm
 * @brief lock function definitions
 */

#ifndef ARM_LOCK_H
#define ARM_LOCK_H

#include <armv7.h>
//#include <gic.h>

void arm_lock(air_uptr_t hash);
air_u32_t arm_unlock(air_uptr_t hash);

void arm_disable_preemption(air_u32_t irq_mask);
air_u32_t arm_enable_preemption(void);

#endif /* ARM_LOCK_H */
