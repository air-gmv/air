/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013 - 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file 
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE. 
 * ==========================================================================*/
/**
 *  @file
 *  @author cdcs
 *  @brief Multi-core function definitions
 */

#ifndef __MULTICORE_H__
#define __MULTICORE_H__

#include <pmk.h>
#include <workspace.h>


/**
 * @defgroup pmk_multicore PMK - Multicore
 * @brief Definitions, structures and functions to provide multicore awareness
 * @{
 */

/**
 * @brief Initializes the Multicore awareness
 */
void pmk_multicore_init(void);

/**
 * @brief Get current core control structure
 * @return Executing Core control structure
 */
pmk_core_ctrl_t *pmk_get_current_core_ctrl(void);

/**
 * @brief Core idle context's
 */
extern core_context_t *pmk_core_idle_context;

/** @} */

/**
 * @brief Restart module
 * @param core Executing core control structure
 */
void pmk_module_restart(pmk_core_ctrl_t *core);

/**
 * @brief Shutdown module
 * @param core Executing core control structure
 */
void pmk_module_shutdown(pmk_core_ctrl_t *core);

#endif /* __MULTICORE_H__ */
