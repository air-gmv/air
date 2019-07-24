/**
 * @file
 * @author pfnf
 * @brief CPU - SPARC abstractions
 */

#ifndef __CPU_H__
#define __CPU_H__

#ifndef ASM
#include <sparc.h>
#include <health_monitor.h>

/**
 * @defgroup cpu_sparc CPU - SPARC Abstractions
 * @brief CPU - SPARC Abstractions for AIR common code
 * @{
 */

/**
 * @brief CPU data alignment
 */
#define CPU_CRITICAL_ALIGN                              (8)

/**
 * @brief CPU MMU context
 */
typedef sparc_mmu_context_t mmu_context_t;

/**
 * @brief CPU core context
 */
typedef sparc_core_context_t core_context_t;

/**
 * @brief CPU specific configuration
 */
typedef sparc_configuration_t arch_configuration_t;

/**
 * @brief CPU Interrupt stack frame
 */
typedef sparc_interrupt_stack_frame_t interrupt_stack_frame_t;

/**
 * @brief CPU Preemption flags
 */
#define cpu_preemption_flags_t air_u32_t

/**
 * @brief CPU enable preemption
 * @param flags Preemption flags
 */
#define cpu_enable_preemption(flags) \
    flags = sparc_enable_preemption()

/**
 * @brief CPU disable preemption
 * @param flags Preemption flags
 */
#define cpu_disable_preemption(flags) \
    sparc_disable_preemption(flags)

/**
 * @brief CPU health-monitor initialization
 */
#define cpu_hm_init()   \
        sparc_healthmonitor_init()

/**
 * @brief CPU segregation initialization
 */
#define cpu_segregation_init() \
    sparc_segregation_init()

/**
 * @brief CPU segregation map memory
 */
#define cpu_segregation_map_memory(ctrl, paddr, vaddr, size, unit, perms) \
    sparc_map_memory((ctrl), (paddr), (vaddr), (size), (unit), (air_u32_t)(perms))

/**
 * @brief CPU segregation copy to user-land
 */
#define cpu_copy_to_user(ctx, dst, src, size) \
    sparc_copy_to_user((ctx), (dst), (src), (size))

/**
 * @brief CPU segregation copy from user-land
 */
#define cpu_copy_from_user(ctx, dst, src, size) \
    sparc_copy_from_user((ctx), (dst), (src), (size))

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
/**
 * @brief Flag an HM event on a core context
 * @param context core context
 * @param state_id current system state
 * @param error_id current error Id
 */
void core_context_add_hm_event(core_context_t *context,
                               air_state_e state_id, air_error_e error_id);

/**
 * @brief Removes an HM event from the core context
 * @param context Context of the core
 */
void core_context_remove_hm_event(core_context_t *context);

/**
 * @brief Saves the core context
 * @param core_ctrl control control pointer
 */
#define core_context_save(core) \
        sparc_core_context_save((void *)(core))
/**
 * @brief Restores the core context
 * @param core_ctrl control control pointer
 */
#define core_context_restore(core) \
        sparc_core_context_restore((void *)(core))
/**
 * @brief Get the core context virtual core Id
 * @param context core context
 * @return Virtual core Id of the context
 */
#define core_context_id(context) \
        (context)->vcpu.id
/**
 * @brief Get the core context Table Base register
 * @param context core context
 * @return Table Base register of the context
 */
#define core_context_tbr(context) \
        (context)->vcpu.tbr
/**
 * @brief Check if the context is flagged as trashed
 * @param context Context to check
 * @return true if trashed, false otherwise
 */
#define core_context_trashed(context) \
        (context)->trash

/** @} */

#endif /* ASM */
#endif /* __CPU_H__ */
