/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2018
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file cpu.h
 * @author lumm
 * @brief Just for backward compatibility
 */

#ifndef ARM_CPU_H
#define ARM_CPU_H

#ifndef ASM

#include <armv7.h>
#include <health_monitor.h>

/* Double-word alignment */
#define CPU_CRITICAL_ALIGN  8

/**
 * @brief CPU MMU context
 */
typedef arm_mmu_context_t mmu_context_t;

/**
 * @brief CPU core context
 */
typedef arm_core_context_t core_context_t;

/**
 * @brief CPU specific configuration
 */
typedef arm_mmu_configuration_t arch_configuration_t;

/**
 * @brief CPU Interrupt stack frame
 */
typedef arm_exception_frame_t interrupt_stack_frame_t;

/**
 * @brief CPU Preemption flags
 */
#define cpu_preemption_flags_t air_u32_t

/**
 * @brief Initializes the core context
 * @param context the core context to be initialized
 * @param id virtual core id;
 */
void core_context_init(core_context_t *context, air_u32_t id);
/**
 * @brief Setups an idle context
 * @param context the core context to be set as idle
 */
void core_context_setup_idle(core_context_t *context);
/**
 * @brief Get core context IPC message
 * @param context core context
 * @return core context IPC message
 */
#define core_context_get_ipc_message(context) \
        (context)->ipc_event
/**
 * @brief Set core context IPC message
 * @param context core context
 * @param message IPC message
 */
#define core_context_set_ipc_message(context, message) \
        (context)->ipc_event = (message)
/**
 * @brief Set core context entry point
 * @param context core context
 * @param entry_point core entry point
 */
#define core_context_set_entry_point(context, entrypoint) \
        (context)->entry_point = (entrypoint)
/**
 * @brief Get core context entry point
 * @param context core context
 * @return entry_point core entry point
 */
#define core_context_get_entry_point(context) \
        (void *)(context)->entry_point
/**
 * @brief Sets the core context system state
 * @param context core context
 * @return core system state
 */
#define core_context_get_system_state(context) \
        (context)->state
/**
 * @brief Sets the core context system state
 * @param context core context
 * @param state system state to apply
 */
#define core_context_set_system_state(context, state) \
        (context)->state = (air_u32_t)(state)

#endif /* ASM */
#endif /* ARM_CPU_H */
