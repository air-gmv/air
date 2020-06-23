/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file a9mpcore.c
 * \author lumm
 * \brief A9MPCore routines
 */

#include <a9mpcore.h>

void arm_a9mpcore_start_hook(air_u32_t cpu_id) {

    if (cpu_id == 0) {
        /* Enable SCU */
        SCU->ctrl |= CTRL_SCU_EN;
    }

#if PMK_SMP
    /* Enable cache coherency and cache/MMU maintenance broadcasts for this processor. */
    air_u32_t actlr = arm_cp15_get_auxiliary_control();
    actlr |= CP15_ACTLR_SMP | CP15_ACTLR_FW;
    arm_cp15_set_auxiliary_control(actlr);
#endif

    arm_scu_invalidate(cpu_id);
}
