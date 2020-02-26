/*
 * Copyright (C) 2008-2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 *  @file
 *  @author pfnf
 *  @brief Structures used to define and control each partition
 */

#ifndef __PARTITION_H__
#define __PARTITION_H__

#ifndef ASM
#include <pmk.h>
#include <loader.h>
#include <barrier.h>
#include <health_monitor.h>
#include <segregation.h>

/**
 * @defgroup pmk_partition PMK - Partitions
 * @brief Definitions, structures and functions to control the partitions state
 * @{
 */

/** 
 * @brief Partition internal structure
 */
typedef struct {

    /** Partition sequential index                                          */
    air_u32_t idx;
    /** Partition number of allocated cores                                 */
    air_u32_t cores;
    /** Partition core context (one per allocated core)                     */
    core_context_t *context;
    /** Partition initial cache state                                       */
    air_cache_e init_cache;
    /** Partition internal state                                            */
    pmk_partition_state_e state;
    /** Partition last clock tick                                           */
    air_clocktick_t last_clock_tick;
    /** Partition elapsed clock ticks                                       */
    air_clocktick_t elapsed_ticks;
    /** Partition operating mode (controlled by the partition)              */
    air_operating_mode_e mode;
    /** Partition virtual to real core mapping                              */
    volatile air_u32_t *core_mapping;
    /** Partition system permissions flags                                  */
    air_permissions_e permissions;
    /** Partition events                                                    */
    air_u32_t events;
    /** Partition health-monitor table                                      */
    pmk_hm_action_list_t **hm_table;
    /** Partition execution state condition                                 */
    air_start_condition_e start_condition;
    /** Partition number of restarts                                        */
    air_u32_t restart_count;
    /** Partition ports                                                     */
    pmk_list_t ports;
    /** Partition barrier for SMP configurations                            */
    pmk_barrier_t smp_barrier;
    /** Partition memory map                                                */
    pmk_mmap_t *mmap;
    /** Partition MMU context                                               */
    mmu_context_t *mmu_ctrl;
    /** Partition compressed ELF pointer                                    */
    pmk_elf_t *elf;
    /** Partition identifier                                                */
    air_identifier_t id;
    /** Partition name                                                      */
    air_name_t name;
    /** Current partition schedule period                                   */
    air_u64_t period;
    /** Current partition schedule duration                                 */
    air_u64_t duration;
    /** Current partition schedule window identifier                        */
    air_identifier_t window_id;

} pmk_partition_t;

/**
 * @brief Initializes the partition internal control structures
 */
void pmk_partitions_init(void);

/**
 * @brief Partition start code
 * @param partition partition control structure
 * @param context current core context structure
 */
void pmk_partition_start(pmk_partition_t *partition, core_context_t *context);
/**
 * @brief Halts a partition
 * @param partition partition control structure
 */
void pmk_partition_halt(pmk_partition_t *partition);
/**
 * @brief Restarts a partition
 * @param partition partition control structure
 */
void pmk_partition_restart(pmk_partition_t *partition);

/**
 * @brief Reload ELF into partition's memory
 * @param partition partition control structure
 */
void pmk_partition_reload(pmk_partition_t *partition);

/**
 * @brief Get partition configuration by Id
 * @param pid partition Id
 * @return partition configuration pointer if Id is valid, NULL otherwise
 */
pmk_partition_t *pmk_get_partition_by_id(air_identifier_t pid);
/**
 * @brief Get partition configuration by name
 * @param name partition name
 * @return partition configuration pointer if name is valid, NULL otherwise
 */
pmk_partition_t *pmk_get_partition_by_name(air_name_ptr_t name);
/**
 * @brief Setups a reload partition context
 * @param partition the partition to be reloaded
 * @param context the core context responsible for the reload
 */
void core_context_setup_reload_partition(
        core_context_t *context, pmk_partition_t *partition);
/**
 * @brief Setups a core partition context
 * @param partition partition information
 * @param context core context
 */
void core_context_setup_partition(
        core_context_t *context, pmk_partition_t *partition);

/** @} */

#endif /* ASM */
#endif /* __PARTITION_H__ */
