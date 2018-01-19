/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2015
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file
 * @author pfnf
 * @brief Functions required to handle the shared memory blocks between
 *        partitions
 */

#include <pmk.h>
#include <partition.h>
#include <sharedmemory.h>
#include <configurations.h>

pmk_shm_t *pmk_sharedmemory_get_by_id(air_identifier_t id) {

    int i;
    pmk_shm_t *found = NULL;
    pmk_list_t *list = pmk_get_shared_memory_areas();
    for(i = 0; i < list->length; ++i) {
        pmk_shm_t *sharedmemory = pmk_get_from_list(pmk_shm_t, list, i);
        if (sharedmemory->id == id) {
            found = sharedmemory;
        }
    }
    return found;
}

pmk_shm_t *pmk_sharedmemory_get_by_name(air_name_ptr_t name) {

    int i;
    pmk_shm_t *found = NULL;
    pmk_list_t *list = pmk_get_shared_memory_areas();
    for(i = 0; i < list->length; ++i) {
        pmk_shm_t *sharedmemory = pmk_get_from_list(pmk_shm_t, list, i);
        if (strncmp(name, sharedmemory->name, sizeof(air_name_t)) == 0) {
            found = sharedmemory;
        }
    }
    return found;
}

pmk_shm_partition_t *pmk_sharedmemory_is_accessible_to_partition(
        pmk_shm_t *sharedarea,
        pmk_partition_t *partition) {

    int i;
    pmk_shm_partition_t *found = NULL;
    pmk_list_t *list = &sharedarea->partitions;

    for (i = 0; i < list->length; ++i) {

        /* get partition permissions from list */
        pmk_shm_partition_t *partition_permissions =
                pmk_get_from_list(pmk_shm_partition_t, list, i);

        /* check if the permissions refer to the current partition */
        if (partition_permissions->partition == partition) {
            found = partition_permissions;
        }
    }

    return found;
}
