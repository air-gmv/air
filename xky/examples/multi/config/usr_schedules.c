/*
 * ============================================================================
 * This file was generated by the GMV's Configurator Tool and is part of an
 * AIR - ARINC 653 Interface in RTEMS - application configuration.
 * File Purpose  : Module Schedules configuration
 * ============================================================================
 */

#include <schedule.h>
#include <partition.h>
#include <usr_config.h>

/**
 * @brief Partition table (defined in usr_partitions.c)
 */
extern pmk_partition_t usr_partition_table[USR_PARTITIONS];

/**
 * @brief Schedule 0: scheduled partitions
 */
static pmk_partition_schedule_t usr_schedule0_partitions[2] = \
{
    {   /* Partition Schedule (Id: 1, Name: p0) */
	    .partition    = &usr_partition_table[0],
	    .set_schedule = 0,
	    .action       = PMK_SCHED_CHANGE_ACTION_IGNORE,
	    .period       = 2000ULL,
	    .duration     = 1000ULL,
    },
    {   /* Partition Schedule (Id: 2, Name: p1) */
	    .partition    = &usr_partition_table[1],
	    .set_schedule = 0,
	    .action       = PMK_SCHED_CHANGE_ACTION_IGNORE,
	    .period       = 2000ULL,
	    .duration     = 1000ULL,
    }
};

/**
 * @brief Schedule 0, core 0: windows
 */
static pmk_window_t usr_schedule0_windows0[2] = \
{
    {   /* Window (Id: 101) */
        .duration     = 1000,
        .period_start = 1,
        .partition    = &usr_partition_table[0],
        .virtual_core = 0,
    },
    {   /* Empty Window */
        .duration     = 1000,
        .period_start = 0,
        .partition    = NULL,
        .virtual_core = 0,
    }
};
/**
 * @brief Schedule 0, core 1: windows
 */
static pmk_window_t usr_schedule0_windows1[2] = \
{
    {   /* Empty Window */
        .duration     = 1000,
        .period_start = 0,
        .partition    = NULL,
        .virtual_core = 0,
    },
    {   /* Window (Id: 201) */
        .duration     = 1000,
        .period_start = 1,
        .partition    = &usr_partition_table[1],
        .virtual_core = 0,
    }
};

/**
 * @brief Schedule 0: cores schedules
 */
static pmk_core_schedule_t usr_schedule0_cores[USR_CORES] = \
{
    { .count = 2, .windows = (void *)usr_schedule0_windows0 },
    { .count = 2, .windows = (void *)usr_schedule0_windows1 }
};

/**
 * @brief Module Schedules Configuration
 */
pmk_schedule_t usr_schedule_table[USR_SCHEDULES] = \
{
    {   /* Schedule (Id: 1, Name: test_sched) */
        .id         = 1,
        .name       = "test_sched",
        .mtf        = 2000,
        .initial    = 1,
        .partitions = { .length =  2, .elements = (void *)usr_schedule0_partitions },
        .cores      = usr_schedule0_cores,
    },
};
