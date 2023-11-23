/*
 * Copyright (C) 2008-2013  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author
 * @brief SPARC clock init function and register structs
 */

#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <air_arch.h>

/**
 * @defgroup bsp_leon_clock BSP - LEON 3 & 4 Clock/Timer
 * @brief Board Support package Clock/Timer support functions
 * @{
 */

/**
 * @brief Generic Timer control structure
 */
typedef struct {

    volatile void *regs;    /**< Base register*/
    air_u32_t irq;           /**< IRQ number  */
    air_u32_t frequency;     /**< Frequency   */

} timer_ctrl_t;

/**
 * @brief LEON registers per timer
 */
typedef struct {

   volatile air_u32_t value;
   volatile air_u32_t reload;
   volatile air_u32_t conf;
   volatile air_u32_t dummy;

} timer_subtype_t;

/**
 * @brief LEON timer register map
 */
typedef struct {

   volatile air_u32_t scaler_value;
   volatile air_u32_t scaler_reload;
   volatile air_u32_t status;
   volatile air_u32_t dummy;
   timer_subtype_t timer[8];

} timer_regmap_t;

/**
 * @brief Timer control structure
 */
extern timer_ctrl_t timer_ctrl;

/**
 * @brief Initializes the timer driver
 * @return 0 if the device was initialize successfully, 1 otherwise
 */
int clock_init(void);

/** @} */

#endif /* __CLOCK_H__ */
