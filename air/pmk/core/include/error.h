/*
 * Copyright (C) 2013-2015  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief Definitions and functions to handle internal errors
 */

#ifndef __ERROR_H__
#define __ERROR_H__

/**
 * @defgroup pmk_error PMK - Errors
 * @brief Definitions and functions to handle internal errors
 * @{
 */

/**
 * @brief PMK internal error enumeration
 */
typedef enum {

    PMK_INTERNAL_ERROR_BSP       = 0x00, /**< Error in BSP                    */
    PMK_INTERNAL_ERROR_CONFIG    = 0x01, /**< Error in Configuration          */
    PMK_INTERNAL_ERROR_MEMORY    = 0x02, /**< Error in Memory (out of memory) */

} pmk_error_e;

/**
 * @brief Processes a fatal error and halts the execution
 * @param code PMK internal error code
 * @param func Function that generated the error
 * @param file File containing the faulty function
 * @param line Line of the error
 */
void pmk_fatal_error(
        pmk_error_e code, const char *func, const char *file, int line);

/** @} */

#endif /* __ERROR_H__ */
