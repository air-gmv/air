/*
 * Copyright (C) 2014-2023  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * https://github.com/air-gmv/air/tree/master/LICENSE
 */

/**
 * @file configurations.h
 * @author pfnf
 * @brief Contains Kernel and user configuration access function definitions
 */

#ifndef __CONFIGURATIONS_H__
#define __CONFIGURATIONS_H__

#include <health_monitor.h>
#include <pmk.h>

/**
 * @defgroup pmk_configuration PMK - Application Configuration
 * @brief Application Configuration definitions and access functions
 * @{
 */

/**
 *  @brief Configuration Magic number
 */
#define PMK_CONFIG_MAGIC_NUMBER (0xDEADC0DE)

/**
 * @brief Number of bytes to clear in each shared memory block
 */
#define PMK_SHARED_MEMORY_CLEAR (64)

/**
 * @brief Application Configuration Table
 */
typedef struct
{
    air_u32_t magic;                          /**< Configuration magic */
    air_u32_t cores;                          /**< Number of configured cores */
    air_clocktick_t us_per_tick;              /**< Micro-seconds per tick */
    pmk_list_t partitions;                    /**< List of partitions */
    pmk_list_t schedules;                     /**< List of schedules */
    pmk_list_t sharedareas;                   /**< List of shared memory blocks */
    pmk_list_t channels;                      /**< List of channels */
    pmk_hm_level_list_t *hm_system_table;     /**< System state HM table */
    pmk_hm_action_list_t **hm_module_table;   /**< Module HM table */
    arch_configuration_t *arch_configuration; /**< Architecture dependent configuration */
} pmk_configuration_t;

/**
 * @brief Pointer to the application configuration
 */
extern pmk_configuration_t *pmk_configuration;

/**
 * @brief Initializes and validates the module configuration
 */
void pmk_configurations_int(void);

/**
 * @brief Get Module Partitions
 * @return List of partitions
 */
static inline pmk_list_t *pmk_get_usr_partitions(void)
{
    return &pmk_configuration->partitions;
}

/**
 * @brief Get Module Schedules
 * @return List of schedules
 */
static inline pmk_list_t *pmk_get_schedules(void)
{
    return &pmk_configuration->schedules;
}

/**
 * @brief Get the Module Channels
 * @return List of channels
 */
static inline pmk_list_t *pmk_get_usr_channels(void)
{
    return &pmk_configuration->channels;
}

/**
 * @brief Get the number of configured cores
 * @return Number of configured cores
 */
static inline air_u32_t pmk_get_usr_configured_cores(void)
{
    return pmk_configuration->cores;
}

/**
 * @brief Get the number of configured micro-seconds per tick
 * @return Number of micro-seconds per tick
 */
static inline air_clocktick_t pmk_get_usr_us_per_tick(void)
{
    return pmk_configuration->us_per_tick;
}

/**
 * @brief Get the number of configured nano-seconds per tick
 * @return Number of nano-seconds per tick
 */
static inline air_clocktick_t pmk_get_usr_ns_per_tick(void)
{
    return pmk_configuration->us_per_tick * 1000;
}

/**
 * @brief Gets the Health-Monitor System State table
 * @returns Health-Monitor System State table pointer
 */
static inline pmk_hm_level_list_t *pmk_get_usr_hm_system_table(void)
{
    return pmk_configuration->hm_system_table;
}

/**
 * @brief Gets the module Health-Monitor Table
 * @returns Module Health-Monitor System State table pointer
 */
static inline pmk_hm_action_list_t **pmk_get_usr_hm_module_table(void)
{
    return pmk_configuration->hm_module_table;
}

/**
 * @brief Get architecture dependent configuration
 * @return Configuration pointer
 */
static inline arch_configuration_t *pmk_get_arch_configuration(void)
{
    return pmk_configuration->arch_configuration;
}

/**
 * @brief Get the shared memory areas
 * @return Shared memory areas list pointer
 */
static inline pmk_list_t *pmk_get_shared_memory_areas(void)
{
    return &pmk_configuration->sharedareas;
}

/** @} */

#endif /* __CONFIGURATIONS_H__ */
