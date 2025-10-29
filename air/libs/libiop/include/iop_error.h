/*
 * Copyright (C) 2011  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file IOPerror.h
 *
 * @author cdcs
 * @brief  Error definition for the IOPartition
 *
 * Macros and Definitions for the error handler
 */

#ifndef __ERROR_H__
#define __ERROR_H__

#include <iop.h>

/**
 * @brief IOP Error codes
 */
typedef enum errors
{
    GENERIC_ERROR = 0,       //!< GENERIC_ERROR
    NO_REQUESTS = 1,         //!< NO_REQUESTS
    NO_REPLIES = 2,          //!< NO_REPLIES
    CANT_CREATE_PORT = 3,    //!< CANT_CREATE_PORT
    CANT_CREATE_TASK = 4,    //!< CANT_CREATE_TASK
    HW_PROBLEM = 5,          //!< HW_PROBLEM
    QUEUE_OVERFLOW = 6,      //!< QUEUE_OVERFLOW
    WRITE_ERROR_Q = 7,       //!< WRITE_ERROR_Q
    READ_ERROR = 8,          //!< READ_ERROR
    CONFIGURATION_ERROR = 9, //!< CONFIGURATION_ERROR
    HW_WRITE_ERROR = 10,     //!< HW_WRITE_ERROR
    HW_READ_ERROR = 11,      //!< HW_READ_ERROR
    WRITE_ERROR_S = 12,      //!< WRITE_ERROR_S
    TIME_ERROR = 13,         //!< TIME_ERROR
    OUT_OF_MEMORY = 14,

} IOP_ERROR;

air_status_code_e iop_raise_error(int error);

#endif
