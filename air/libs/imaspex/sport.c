/*
 * Copyright (C) 2008-2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file sport.c
 * @author sfbs
 * @brief  ARINC653 sampling port interface implementation
 */
#include <imaspex.h>

extern int imaspex_tsal_init;
extern air_clocktick_t imaspex_ns_per_tick;

/* ARINC 653 (PART 1) SAMPLING PORT SERVICES */
/* -------------------------------------------------------------------------- */
void CREATE_SAMPLING_PORT(
    /*in */ SAMPLING_PORT_NAME_TYPE SAMPLING_PORT_NAME,
    /*in */ MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
    /*in */ PORT_DIRECTION_TYPE PORT_DIRECTION,
    /*in */ SYSTEM_TIME_TYPE REFRESH_PERIOD,
    /*out*/ SAMPLING_PORT_ID_TYPE *SAMPLING_PORT_ID,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* convert configuration to AIR format */
    air_sampling_port_configuration_t config;

    config.max_message_size = MAX_MESSAGE_SIZE;
    config.refresh_period = REFRESH_PERIOD / (air_u32_t)imaspex_ns_per_tick;
    config.port_direction = PORT_DIRECTION;

    /* create port via system call */
    *RETURN_CODE = air_syscall_create_port(AIR_SAMPLING_PORT, (air_name_ptr_t)SAMPLING_PORT_NAME, (void *)&config,
                                           (air_identifier_t *)SAMPLING_PORT_ID);
}

void WRITE_SAMPLING_MESSAGE(
    /*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
    /*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR, /* by reference */
    /*in */ MESSAGE_SIZE_TYPE LENGTH,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* write message view system call */
    *RETURN_CODE = air_syscall_write_port(AIR_SAMPLING_PORT, (air_identifier_t)SAMPLING_PORT_ID,
                                          (air_message_ptr_t)MESSAGE_ADDR, (air_sz_t)LENGTH, (void *)NULL);
}

void READ_SAMPLING_MESSAGE(
    /*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
    /*out*/ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
    /*out*/ MESSAGE_SIZE_TYPE *LENGTH,
    /*out*/ VALIDITY_TYPE *VALIDITY,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* read regular message */
    air_sampling_port_status_t status;
    status.operation = AIR_SAMPLING_MODE_REGULAR;

    /* read message view system call */
    *RETURN_CODE = air_syscall_read_port(AIR_SAMPLING_PORT, (air_identifier_t)SAMPLING_PORT_ID,
                                         (air_message_ptr_t)MESSAGE_ADDR, (air_sz_t *)LENGTH, (void *)&status);

    /* convert back to ARINC format */
    *VALIDITY = status.last_msg_validity;
}

void GET_SAMPLING_PORT_ID(
    /*in */ SAMPLING_PORT_NAME_TYPE SAMPLING_PORT_NAME,
    /*out*/ SAMPLING_PORT_ID_TYPE *SAMPLING_PORT_ID,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* get port id via system call */
    *RETURN_CODE = air_syscall_get_port_id(AIR_SAMPLING_PORT, (air_name_ptr_t)SAMPLING_PORT_NAME,
                                           (air_identifier_t *)SAMPLING_PORT_ID);
}

void GET_SAMPLING_PORT_STATUS(
    /*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
    /*out*/ SAMPLING_PORT_STATUS_TYPE *SAMPLING_PORT_STATUS,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* get port status via system call */
    air_sampling_port_status_t status;
    *RETURN_CODE = air_syscall_get_port_status(AIR_SAMPLING_PORT, (air_identifier_t)SAMPLING_PORT_ID, (void *)&status);

    /* convert status back to the ARINC format */
    if (*RETURN_CODE == NO_ERROR)
    {

        SAMPLING_PORT_STATUS->MAX_MESSAGE_SIZE = status.max_message_size;
        SAMPLING_PORT_STATUS->REFRESH_PERIOD = status.refresh_period * imaspex_ns_per_tick;
        SAMPLING_PORT_STATUS->PORT_DIRECTION = status.port_direction;
        SAMPLING_PORT_STATUS->LAST_MSG_VALIDITY = status.last_msg_validity;
    }
}

void GET_SAMPLING_PORT_NAME(SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID, SAMPLING_PORT_NAME_TYPE SAMPLING_PORT_NAME, RETURN_CODE_TYPE *RETURN_CODE)
{
    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* get port name via system call */
    *RETURN_CODE = air_syscall_get_port_name(AIR_SAMPLING_PORT, -1, (air_identifier_t) SAMPLING_PORT_ID, (air_name_ptr_t)SAMPLING_PORT_NAME);
}

void GET_A_SAMPLING_PORT_NAME(SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID, PARTITION_ID_TYPE PARTITION_ID,
                                SAMPLING_PORT_NAME_TYPE SAMPLING_PORT_NAME, RETURN_CODE_TYPE *RETURN_CODE)
{
    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* get port name via system call */
    *RETURN_CODE = air_syscall_get_port_name(AIR_SAMPLING_PORT, PARTITION_ID, (air_identifier_t) SAMPLING_PORT_ID, (air_name_ptr_t)SAMPLING_PORT_NAME);
}

/* ARINC 653 (PART 2) SAMPLING PORT EXTENSION SERVICES */
/* -------------------------------------------------------------------------- */
void READ_UPDATED_SAMPLING_MESSAGE(
    /*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
    /*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
    /*out*/ MESSAGE_SIZE_TYPE *LENGTH,
    /*out*/ UPDATED_TYPE *UPDATED,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* read updated message */
    air_sampling_port_status_t status;
    status.operation = AIR_SAMPLING_MODE_UPDATED;

    /* read message view system call */
    *RETURN_CODE = air_syscall_read_port(AIR_SAMPLING_PORT, (air_identifier_t)SAMPLING_PORT_ID,
                                         (air_message_ptr_t)MESSAGE_ADDR, (air_sz_t *)LENGTH, (void *)&status);

    /* convert back to ARINC format */
    *UPDATED = status.updated;
}

void GET_SAMPLING_PORT_CURRENT_STATUS(
    /*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
    /*out*/ SAMPLING_PORT_CURRENT_STATUS_TYPE *SAMPLING_PORT_CURRENT_STATUS,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* get port status via system call */
    air_sampling_port_status_t status;
    *RETURN_CODE = air_syscall_get_port_status(AIR_SAMPLING_PORT, (air_identifier_t)SAMPLING_PORT_ID, (void *)&status);

    /* convert status back to the ARINC format */
    if (*RETURN_CODE == NO_ERROR)
    {

        SAMPLING_PORT_CURRENT_STATUS->MAX_MESSAGE_SIZE = status.max_message_size;
        SAMPLING_PORT_CURRENT_STATUS->REFRESH_PERIOD = status.refresh_period * imaspex_ns_per_tick;
        SAMPLING_PORT_CURRENT_STATUS->TIME_STAMP = status.time_stamp * imaspex_ns_per_tick;
        SAMPLING_PORT_CURRENT_STATUS->PORT_DIRECTION = status.port_direction;
        SAMPLING_PORT_CURRENT_STATUS->MESSAGE_AGE = status.message_age;
        SAMPLING_PORT_CURRENT_STATUS->UPDATED = status.updated;
    }
}

void READ_SAMPLING_MESSAGE_CONDITIONAL(
    /*in */ SAMPLING_PORT_ID_TYPE SAMPLING_PORT_ID,
    /*in */ SYSTEM_TIME_TYPE REF_TIME_STAMP,
    /*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
    /*out*/ MESSAGE_SIZE_TYPE *LENGTH,
    /*out*/ SYSTEM_TIME_TYPE *TIME_STAMP,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* read updated message */
    air_sampling_port_status_t status;
    status.time_stamp = REF_TIME_STAMP / imaspex_ns_per_tick;
    status.operation = AIR_SAMPLING_MODE_CONDITIONAL;

    /* read message view system call */
    *RETURN_CODE = air_syscall_read_port(AIR_SAMPLING_PORT, (air_identifier_t)SAMPLING_PORT_ID,
                                         (air_message_ptr_t)MESSAGE_ADDR, (air_sz_t *)LENGTH, (void *)&status);

    /* convert back to ARINC format */
    *TIME_STAMP = status.time_stamp * imaspex_ns_per_tick;
}
