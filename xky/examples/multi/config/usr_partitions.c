/*
 * ============================================================================
 * This file was generated by the GMV's Configurator Tool and is part of an
 * AIR - ARINC 653 Interface in RTEMS - application configuration.
 * File Purpose  : Partitions configuration
 * ============================================================================
 */

#include <ports.h>
#include <partition.h>
#include <segregation.h>
#include <health_monitor.h>
#include <usr_config.h>

/**
 * @brief Partition ELF tables (defined in usr_partitions_data.c)
 */
extern pmk_elf_t usr_partitions_data[USR_PARTITIONS];
/**
 * @brief Partition 0 Health-Monitor Table (defined in usr_hm.c)
 */
extern pmk_hm_action_list_t *usr_hm_partition0_table[PMK_HM_STATES_COUNT];
/**
 * @brief Partition 1 Health-Monitor Table (defined in usr_hm.c)
 */
extern pmk_hm_action_list_t *usr_hm_partition1_table[PMK_HM_STATES_COUNT];
/**
 * @brief Partition 0 Port Table (defined in usr_channels.c)
 */
extern pmk_port_t *usr_partition0_ports[2];
/**
 * @brief Partition 1 Port Table (defined in usr_channels.c)
 */
extern pmk_port_t *usr_partition1_ports[2];

/**
 * @brief Partitions Memory Map
 */
static pmk_mmap_t usr_partitions_memmap[USR_PARTITIONS] = \
{
    {
        .v_addr = (void *)0x41000000,
        .p_unit = 0x40000,
        .size   = 0x500000,
    },
    {
        .v_addr = (void *)0x41000000,
        .p_unit = 0x40000,
        .size   = 0x500000,
    }
};

/**
 * @brief Partitions Configuration Table
 */
pmk_partition_t usr_partition_table[USR_PARTITIONS] = \
{
    {   /* Partition (Id: 1, Name: p0) */
        .id          = 1,
        .name        = "p0",
        .permissions = \
            XKY_PERMISSION_CACHE_CONTROL |
            XKY_PERMISSION_GLOBAL_TIME |
            XKY_PERMISSION_SET_TOD |
            XKY_PERMISSION_SET_PARTITION_MODE,
        .cores       = 1,
        .init_cache  = XKY_CACHE_ALL,
        .hm_table    = usr_hm_partition0_table,
        .mmap        = &usr_partitions_memmap[0],
        .elf         = &usr_partitions_data[0],
        .ports = \
        {
            .length =  2,
            .elements = (void *)usr_partition0_ports
        }
    },
    {   /* Partition (Id: 2, Name: p1) */
        .id          = 2,
        .name        = "p1",
        .permissions = \
            XKY_PERMISSION_CACHE_CONTROL |
            XKY_PERMISSION_GLOBAL_TIME |
            XKY_PERMISSION_SET_TOD |
            XKY_PERMISSION_SET_PARTITION_MODE,
        .cores       = 1,
        .init_cache  = XKY_CACHE_ALL,
        .hm_table    = usr_hm_partition1_table,
        .mmap        = &usr_partitions_memmap[1],
        .elf         = &usr_partitions_data[1],
        .ports = \
        {
            .length =  2,
            .elements = (void *)usr_partition1_ports
        }
    }
};
