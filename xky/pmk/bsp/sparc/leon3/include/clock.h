/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2008-2013
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file
 * @author
 * @brief
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

    volatile void *regs;    /**< Base register                  */
    air_u32_t irq;           /**< IRQ number                     */
    air_u32_t frequency;     /**< Frequency                      */

} timer_ctrl_t;

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
