/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file lock.h
 * @author lumm
 * @brief lock function definitions
 */

#ifndef ARM_LOCK_H
#define ARM_LOCK_H

#include <armv7.h>

air_u32_t arm_lock(air_uptr_t hash);
air_u32_t arm_unlock(air_uptr_t hash, air_u32_t pil);

air_u32_t arm_save_preemption(void);
void arm_restore_preemption(air_u32_t pil);

#endif /* ARM_LOCK_H */
