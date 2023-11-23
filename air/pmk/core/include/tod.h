/*
 * Copyright (C) 2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author jaac
 * @brief Time Of Day definitions, structures and functions
 */

#ifndef __TOD_H__
#define __TOD_H__

#include <pmk.h>

/**
 * @defgroup pmk_tod PMK - Time Of Day
 * @brief Time Of Day definitions, structures and functions
 * @{
 */

/* Flag to check whether the system time of day was defined */
extern int pmk_tod_defined;

/* System Time of Day */
extern air_time_t pmk_tod_now;
   
/**
 *  @brief Time of Day initialization support
 */
void pmk_tod_initialization(void);

/**
 * @brief Time Of day update
 * @note This function is called only by the primary core during the scheduler
 *       update
 */
void pmk_tod_update(void);

/** @} */

#endif /* __TOD_H__ */
