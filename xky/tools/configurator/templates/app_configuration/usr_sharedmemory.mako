<%
    import xky
    import utils.templates as makoutils
    from tools.configure_app import convertMemoryPermissions

%>\
${makoutils.cfileHeader('usr_sharedmemory.c', 'Shared Memory configuration')}

#include <usr_config.h>

#include <partition.h>
#include <sharedmemory.h>

/**
 * @brief Partition table (defined in usr_partitions.c)
 */
extern pmk_partition_t usr_partition_table[USR_PARTITIONS];

% for i, shm in enumerate(configuration.shared_memory):
/**
 * @brief Shared Memory ${i}: Partition Permissions
 */
static pmk_shm_partition_t usr_shm${i}_partitions[${len(shm.permissions)}] = ${'\\'}
{
    % for j, shm_permission in enumerate(shm.permissions):
    {<% permissions = convertMemoryPermissions(shm_permission.permissions) %>
        .partition      = &usr_partition_table[${shm_permission.partition.index}],
        .permissions    = ${' | '.join(convertMemoryPermissions(shm_permission.permissions))},
    }${"," if j < len(shm.permissions) - 1 else ""}
    % endfor
};
% endfor

/**
 * @brief Shared Memory Configuration Table
 */
pmk_shm_t usr_shared_memory_table[USR_SHARED_MEMORY] = ${'\\'}
{
% for i, shm in enumerate(configuration.shared_memory):
    {   /* ${shm} */
        .id         = ${shm.id},
        .name       = "${shm.name}",
        .addr       = (void *)${hex(shm.addr)},
        .size       = ${hex(shm.size)},
        .unit       = ${hex(shm.unit)},
        .partitions = { .length = ${len(shm.permissions)}, .elements = (void *)usr_shm${i}_partitions },
    }${"," if i < len(configuration.shared_memory) - 1 else ""}
% endfor
};
