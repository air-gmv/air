<%
    import air
    import utils.templates as makoutils
%>\
%if header:
${makoutils.cfileHeader('usr_config.h', 'General AIR application configuration')}

#define USR_US_PER_TICK                                         (${int((1.0 / configuration.ticks_per_second) * 1E6)})
#define USR_PARTITIONS                                          (${len(configuration.partitions)})
#define USR_SCHEDULES                                           (${len(configuration.schedules)})
#define USR_CHANNELS                                            (${len(configuration.channels)})
#define USR_CORES                                               (${configuration.core_count})
#define USR_SHARED_MEMORY                                       (${len(configuration.shared_memory)})
%else:
${makoutils.cfileHeader('usr_config.c', 'General AIR application configuration')}

#include <pmk.h>
#include <ports.h>
#include <schedule.h>
#include <partition.h>
#include <sharedmemory.h>
#include <health_monitor.h>
#include <configurations.h>

#include <usr_config.h>

/**
 * @brief Schedule table (defined in usr_schedules.c)
 */
extern pmk_schedule_t  usr_schedule_table[USR_SCHEDULES];
/**
 * @brief Partition table (defined in usr_partitions.c)
 */
extern pmk_partition_t usr_partition_table[USR_PARTITIONS];
/**
 * @brief Module Communication Channels table (defined in usr_channels.c)
 */
extern pmk_channel_t usr_channels_table[USR_CHANNELS];
/**
 * @brief Shared Memory Configuration Table (defined in usr_sharedmemory.c)
 */
pmk_shm_t usr_shared_memory_table[USR_SHARED_MEMORY];
/**
 * @brief System State Health-Monitor table
 */
extern pmk_hm_level_list_t usr_hm_system_table[PMK_HM_STATES_COUNT];
/**
 * @brief Module Health-Monitor table
 */
extern pmk_hm_action_list_t *usr_hm_module_table[PMK_HM_STATES_COUNT];
/**
 * @brief Architecture specific Configuration
 */
extern arch_configuration_t arch_configuration;

/**
 * @brief AIR configuration
 */
const pmk_configuration_t __config = \
{
    .magic              = PMK_CONFIG_MAGIC_NUMBER,
    .cores              = USR_CORES,
    .us_per_tick        = USR_US_PER_TICK,
    .partitions         = { .length = USR_PARTITIONS,    .elements = (void *)usr_partition_table     },
    .schedules          = { .length = USR_SCHEDULES,     .elements = (void *)usr_schedule_table      },
    .sharedareas        = { .length = USR_SHARED_MEMORY, .elements = (void *)usr_shared_memory_table },
    .channels           = { .length = USR_CHANNELS,      .elements = (void *)usr_channels_table      },
    .hm_system_table    = usr_hm_system_table,
    .hm_module_table    = usr_hm_module_table,
    .arch_configuration = &arch_configuration
};
%endif
