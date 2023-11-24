/*
 * Copyright (C) 2008-2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file entry.c
 * @author cdcs
 * @brief  Partition entry point and initialization functions
 */

#include <imaspex.h>

int imaspex_tsal_init = 0;
air_clocktick_t imaspex_ns_per_tick;

int imaspex_init(void)
{

    int ret_code = NO_ACTION;

    if (0 == imaspex_tsal_init)
    {

        /* init once only :) */
        imaspex_tsal_init = 1;
        imaspex_ns_per_tick = air_syscall_get_us_per_tick() * 1000;

        ret_code = NO_ERROR;
    }

    return ret_code;
}

int TSAL_INIT(void)
{

    return imaspex_init();
}
