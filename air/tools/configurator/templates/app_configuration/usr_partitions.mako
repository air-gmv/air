<%
    import air
    import utils.templates as makoutils
    import parsers.a653.definitions as a653_definitions

%>\
${makoutils.cfileHeader('usr_partitions.c', 'Partitions configuration')}

#include <ports.h>
#include <partition.h>
#include <segregation.h>
#include <health_monitor.h>
#include <usr_config.h>

/**
 * @brief Partition ELF tables (defined in usr_partitions_data.c)
 */
extern pmk_elf_t usr_partitions_data[USR_PARTITIONS];
% for i, partition in enumerate(configuration.partitions):
/**
 * @brief Partition ${i} Health-Monitor Table (defined in usr_hm.c)
 */
extern pmk_hm_action_list_t *usr_hm_partition${i}_table[PMK_HM_STATES_COUNT];
% endfor
% for i, partition in enumerate(configuration.partitions):
/**
 * @brief Partition ${i} Port Table (defined in usr_channels.c)
 */
extern pmk_port_t *usr_partition${i}_ports[${len(partition.ports)}];
% endfor

/**
 * @brief Partitions Memory Map
 */
static pmk_mmap_t usr_partitions_memmap[USR_PARTITIONS] = ${'\\'}
{
% for i, partition in enumerate(configuration.partitions):
    {
        .v_addr = (void *)${hex(partition.memory_vaddr)},
        .p_unit = ${hex(partition.memory_unit)},
        .size   = ${hex(partition.memory_size)},
    }${"," if i < len(configuration.partitions) - 1 else ""}
% endfor
};

/**
 * @brief Partitions Configuration Table
 */
pmk_partition_t usr_partition_table[USR_PARTITIONS] = ${'\\'}
{
% for i, partition in enumerate(configuration.partitions):
    {   /* ${partition} */
        .id          = ${partition.id},
        .name        = "${partition.name}",
        .state       = AIR_PARTITION_STATE_INIT,
        .window_id   = -1, /*Default: No window assigned (-1)*/
% if len(partition.permissions) > 0:
        .permissions = ${'\\'}
    % for j, permission in enumerate(partition.permissions):
            AIR_PERMISSION_${permission.upper()}${',' if j == len(partition.permissions) - 1 else ' |'}
    % endfor
% else:
        .permissions = 0,
% endif
        .cores       = ${partition.core_count},
<%
    if partition.cache_state[0] and partition.cache_state[1]:
        cache = 'AIR_CACHE_ALL'
    elif partition.cache_state[0]:
        cache = 'AIR_CACHE_DATA'
    elif partition.cache_state[1]:
        cache = 'AIR_CACHE_INSTRUCTION'
    else:
        cache = 'AIR_CACHE_NONE'
%>\
        .init_cache  = ${cache},
        .events      = 0,
        .hm_table    = usr_hm_partition${i}_table,
        .mmap        = &usr_partitions_memmap[${i}],
        .elf         = &usr_partitions_data[${i}],
        .ports = ${'\\'}
        {
            .length =  ${len(partition.ports)},
            .elements = (void *)usr_partition${i}_ports
        }
    }${"," if i < len(configuration.partitions) - 1 else ""}
% endfor
};
