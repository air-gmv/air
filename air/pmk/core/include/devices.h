/*
 * Copyright (C) 2013  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/AIR/raw/master/air/LICENSE
 */
/**
 * @file devices.h
 * @author pfnf
 * @brief Definition related with the mapping of hardware devices to partitions
 */

#ifndef __DEVICES_H__
#define __DEVICES_H__

#include <partition.h>

/**
 *  @brief PMK device to group mapping structure
 */
typedef struct {

    air_u32_t vendor;                        /**< Device vendor Id  */
    air_u32_t device;                        /**< Device Id         */
    air_u32_t minor;                         /**< Device Minor      */
    air_u32_t partition;                     /**< Owner partition   */

} pmk_device_map_t;

void pmk_iommu_initialize(pmk_partition_t *, pmk_device_map_t *);

#endif /* __DEVICES_H__ */
