/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file shared_memory.h
 * @author pfnf
 * @brief Structures and functions definitions to handle the shared memory
 *        blocks between partitions
 */

#ifndef __SHARED_MEMORY_H__
#define __SHARED_MEMORY_H__

#include <pmk.h>
#include <partition.h>

/**
 * @brief Shared-memory block
 */
typedef struct {

    air_identifier_t id;    /**< id of shared memory block              */
    air_name_t name;        /**< name of shared memory block            */
    pmk_list_t partitions;  /**< list of partitions                     */
    void *addr;             /**< virtual address of the memory block    */
    air_sz_t size;            /**< size of the memory block               */
    air_sz_t unit;            /**< unit of alocation of the memory block  */

} pmk_shm_t;

/**
 * @brief Shared-memory partition access permissions
 */
typedef struct {

    pmk_partition_t *partition;     /**< partition pointer              */
    pmk_mmu_flags_e permissions;    /**< permissions of the partition   */

} pmk_shm_partition_t;


/**
 * @brief Get shared memory by id
 * @param id id of the shared memory to look for
 * @return shared memory pointer if found, NULL otherwise
 */
pmk_shm_t *pmk_sharedmemory_get_by_id(air_identifier_t id);
/**
 * @brief Get shared memory by name
 * @param name name of the shared memory to look for
 * @return shared memory pointer if found, NULL otherwise
 */
pmk_shm_t *pmk_sharedmemory_get_by_name(air_name_ptr_t name);
/**
 * @brief Checks if a partition have access to a shared memory
 * @param sharedarea shared memory area pointer
 * @param partition current partition to check access
 * @return permissions of the partition if accessible, NULL otherwise
 */
pmk_shm_partition_t *pmk_sharedmemory_is_accessible_to_partition(
        pmk_shm_t *sharedarea,
        pmk_partition_t *partition);

#endif /* __SHARED_MEMORY_H__ */
