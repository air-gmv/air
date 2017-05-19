/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2008-2015
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file
 * @author pfnf
 * @brief IRQMP device initialization and handling functions
 */

#ifndef __IRQMP_H__
#define __IRQMP_H__

#include <pmk.h>

/**
 * @defgroup bsp_leon_irqmp BSP - LEON 3 & 4 IRQMP
 * @brief Board Support package IRQMP support functions
 * @{
 */

/**
 * @brief IRQMP driver initialization
 * @return 0 if the device was initialize successfully, 1 otherwise
 */
int irqmp_init(void);
/**
 * @brief Get the maximum available cores
 * @return Number of cores available
 */
xky_u32_t irqmp_get_number_of_cores();
/**
 * @brief Enable interrupt on core
 * @param core_id Id of the core to enable the interrupt
 * @param tn Interrupt trap number
 */
void irqmp_enable_interrupt(xky_u32_t core_id, xky_u32_t tn);
/**
 * @brief Disable interrupt on core
 * @param core_id Id of the core to enable the interrupt
 * @param tn Interrupt trap number
 */
void irqmp_disable_interrupt(xky_u32_t core_id, xky_u32_t tn);
/**
 * @brief Set core interrupt mask
 * @param core_id Id of the core to apply the IRQ mask
 * @param irq_mask IRQ mask to apply
 */
void irqmp_set_interrupt_mask(xky_u32_t core_id, xky_u32_t irq_mask);
/**
 * @brief Set an interrupt priority
 * @param tn interrupt number
 * @param p priority for the interrupt
 */
void irqmp_interrupt_set_priority(xky_u32_t tn, xky_u32_t p);
/**
 * @brief Interrupt core
 * @param core_id Id of the core to interrupt
 * @param tn Interrupt tap number
 */
void irqmp_interrupt_core(xky_u32_t core_id, xky_u32_t tn);
/**
 * @brief Broadcast interrupt
 * @param tn Interrupt tap number
 */
void irqmp_interrupt_broadcast(xky_u32_t tn);
/**
 * @brief Boot core
 * @param core_id Index of the core to boot
 * @param entry_point Core execution entry point
 */
void irqmp_boot_core(xky_u32_t core_id, void *entry_point);

/** @} */

#endif /* __IRQMP_H__ */
