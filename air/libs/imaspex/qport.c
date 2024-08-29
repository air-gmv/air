/*
 * Copyright (C) 2008-2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file qport.c
 * @author sfbs
 * @brief ARINC653 queuing port interface implementation
 */

#include <imaspex.h>

extern int imaspex_tsal_init;

void CREATE_QUEUING_PORT(
    /*in */ QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME,
    /*in */ MESSAGE_SIZE_TYPE MAX_MESSAGE_SIZE,
    /*in */ MESSAGE_RANGE_TYPE MAX_NB_MESSAGE,
    /*in */ PORT_DIRECTION_TYPE PORT_DIRECTION,
    /*in */ QUEUING_DISCIPLINE_TYPE QUEUING_DISCIPLINE,
    /*out*/ QUEUING_PORT_ID_TYPE *QUEUING_PORT_ID,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* convert configuration to AIR format */
    air_queuing_port_configuration_t config;

    config.max_message_size = MAX_MESSAGE_SIZE;
    config.max_nb_message = MAX_NB_MESSAGE;
    config.port_direction = PORT_DIRECTION;
    config.port_discipline = QUEUING_DISCIPLINE;

    /* create port via system call */
    *RETURN_CODE = air_syscall_create_port(AIR_QUEUING_PORT, (air_name_ptr_t)QUEUING_PORT_NAME, (void *)&config,
                                           (air_identifier_t *)QUEUING_PORT_ID);
}

void SEND_QUEUING_MESSAGE(
    /*in */ QUEUING_PORT_ID_TYPE QUEUING_PORT_ID,
    /*in */ MESSAGE_ADDR_TYPE MESSAGE_ADDR, /* by reference */
    /*in */ MESSAGE_SIZE_TYPE LENGTH,
    /*in */ SYSTEM_TIME_TYPE TIME_OUT,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* send message via system call */
    *RETURN_CODE = air_syscall_write_port(AIR_QUEUING_PORT, (air_identifier_t)QUEUING_PORT_ID,
                                          (air_message_ptr_t)MESSAGE_ADDR, (air_sz_t)LENGTH, NULL);

    /** @todo TIME_OUT not implemented */
}

void RECEIVE_QUEUING_MESSAGE(
    /*in */ QUEUING_PORT_ID_TYPE QUEUING_PORT_ID,
    /*in */ SYSTEM_TIME_TYPE TIME_OUT,
    /*out*/ MESSAGE_ADDR_TYPE MESSAGE_ADDR,
    /*out*/ MESSAGE_SIZE_TYPE *LENGTH,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* receive message via system call */
    *RETURN_CODE = air_syscall_read_port(AIR_QUEUING_PORT, (air_identifier_t)QUEUING_PORT_ID,
                                         (air_message_ptr_t)MESSAGE_ADDR, (air_sz_t *)LENGTH, NULL);

    /** @todo TIME_OUT not implemented */
}

void GET_QUEUING_PORT_ID(
    /*in */ QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME,
    /*out*/ QUEUING_PORT_ID_TYPE *QUEUING_PORT_ID,
    /*out*/ RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* get port id via system call */
    *RETURN_CODE = air_syscall_get_port_id(AIR_QUEUING_PORT, (air_name_ptr_t)QUEUING_PORT_NAME,
                                           (air_identifier_t *)QUEUING_PORT_ID);
}

void GET_QUEUING_PORT_STATUS(QUEUING_PORT_ID_TYPE QUEUING_PORT_ID, QUEUING_PORT_STATUS_TYPE *QUEUING_PORT_STATUS,
                             RETURN_CODE_TYPE *RETURN_CODE)
{

    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* get port status via system call */
    air_queuing_port_status_t status;
    *RETURN_CODE = air_syscall_get_port_status(AIR_QUEUING_PORT, (air_identifier_t)QUEUING_PORT_ID, (void *)&status);

    /* convert status back to the ARINC format */
    if (*RETURN_CODE == NO_ERROR)
    {

        QUEUING_PORT_STATUS->MAX_MESSAGE_SIZE = status.max_message_size;
        QUEUING_PORT_STATUS->MAX_NB_MESSAGE = status.max_nb_message;
        QUEUING_PORT_STATUS->PORT_DIRECTION = status.port_direction;
        QUEUING_PORT_STATUS->NB_MESSAGE = status.nb_message;
    }
}

void GET_QUEUING_PORT_NAME(QUEUING_PORT_ID_TYPE QUEUING_PORT_ID, QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME, RETURN_CODE_TYPE *RETURN_CODE)
{
    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* get port name via system call */
    *RETURN_CODE = air_syscall_get_port_name(AIR_QUEUING_PORT, -1, (air_identifier_t) QUEUING_PORT_ID, (air_name_ptr_t)QUEUING_PORT_NAME);
}

void GET_A_QUEUING_PORT_NAME(QUEUING_PORT_ID_TYPE QUEUING_PORT_ID, PARTITION_ID_TYPE PARTITION_ID,
                                 QUEUING_PORT_NAME_TYPE QUEUING_PORT_NAME, RETURN_CODE_TYPE *RETURN_CODE)
{
    /* check TSAL init */
    if (imaspex_tsal_init == 0)
    {
        *RETURN_CODE = INVALID_MODE;
        return;
    }

    /* get port name via system call */
    *RETURN_CODE = air_syscall_get_port_name(AIR_QUEUING_PORT, PARTITION_ID, (air_identifier_t) QUEUING_PORT_ID, (air_name_ptr_t)QUEUING_PORT_NAME);
}