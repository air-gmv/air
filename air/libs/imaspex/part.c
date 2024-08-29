/*
 * Copyright (C) 2008-2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author sfbs
 * @brief  Partition Management interface implementation
 */

#include <imaspex.h>

extern int imaspex_tsal_init;
extern air_clocktick_t imaspex_ns_per_tick;

void GET_PARTITION_STATUS(PARTITION_STATUS_TYPE *PARTITION_STATUS, RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* get partition status via system call */
    air_partition_status_t status;
    *RETURN_CODE = air_syscall_get_partition_status(-1, &status);

    /* convert status to ARINC status */
    if (*RETURN_CODE == NO_ERROR)
    {
        PARTITION_STATUS->START_CONDITION = status.start_condition;
        PARTITION_STATUS->DURATION = status.duration * imaspex_ns_per_tick;
        PARTITION_STATUS->IDENTIFIER = status.identifier;
        PARTITION_STATUS->PERIOD = status.period * imaspex_ns_per_tick;
        PARTITION_STATUS->OPERATING_MODE = status.operating_mode;
    }
}

void SET_PARTITION_MODE(OPERATING_MODE_TYPE OPERATING_MODE, RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    *RETURN_CODE = air_syscall_set_partition_mode(-1, OPERATING_MODE);
}

void GET_TIME(SYSTEM_TIME_TYPE *SYSTEM_TIME, RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    air_clocktick_t ticks = air_syscall_get_elapsed_ticks();
    *SYSTEM_TIME = ticks * imaspex_ns_per_tick;
    *RETURN_CODE = NO_ERROR;
}

void GET_MTF_TIME(SYSTEM_TIME_TYPE *SYSTEM_TIME, RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    air_clocktick_t ticks = air_syscall_get_elapsed_mtf_ticks();
    *SYSTEM_TIME = ticks * imaspex_ns_per_tick;
    *RETURN_CODE = NO_ERROR;
}

void SET_A_PARTITION_MODE(
    /*in */ PARTITION_ID_TYPE PARTITION_IDENTIFIER,
    /*in */ OPERATING_MODE_TYPE OPERATING_MODE,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* set partition mode via system call */
    *RETURN_CODE = air_syscall_set_partition_mode(PARTITION_IDENTIFIER, OPERATING_MODE);
}

void GET_A_PARTITION_STATUS(
    /*in */ PARTITION_ID_TYPE PARTITION_IDENTIFIER,
    /*out*/ PARTITION_STATUS_TYPE *PARTITION_STATUS,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* get partition status via system call */
    air_partition_status_t status;
    *RETURN_CODE = air_syscall_get_partition_status(PARTITION_IDENTIFIER, &status);

    /* convert status to ARINC status */
    if (*RETURN_CODE == NO_ERROR)
    {
        PARTITION_STATUS->START_CONDITION = status.start_condition;
        PARTITION_STATUS->DURATION = status.duration;
        PARTITION_STATUS->IDENTIFIER = status.identifier;
        PARTITION_STATUS->PERIOD = status.period;
        PARTITION_STATUS->OPERATING_MODE = status.operating_mode;
    }
}

void GET_A_PARTITION_ID(
    /*in */ NAME_TYPE PARTITION_NAME,
    /*out*/ PARTITION_ID_TYPE *PARTITION_ID,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* get partition id via system call */
    *RETURN_CODE = air_syscall_get_partition_id(PARTITION_NAME, PARTITION_ID);
}

void GET_PARTITION_ID(
    /*out*/ PARTITION_ID_TYPE *PARTITION_ID,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* get partition id via system call */
    *RETURN_CODE = air_syscall_get_partition_id(NULL, PARTITION_ID);
}

void SIGNAL_ACTIVITY_COMPLETION(
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    *RETURN_CODE = NO_ACTION;
}
