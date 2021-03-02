/*
 * Copyright (C) 2011  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */

#ifndef __TASTE_API_H__
#define __TASTE_API_H__

/**
 *  @return AIR_SUCCESSFUL if the operation was completed correctly
 *
 *  @brief Initializes IOP support features (send/receive queues, mms, configuration devices, ports)
 *
 */
void init_iopsupport(air_u32_t);

void iop_init_devs(iop_logical_device_t *ldev, iop_physical_device_t *pdev);

/**
 *  @return AIR_SUCCESSFUL if the operation was completed correctly
 *  @param[in] dev_id Physical Device index on physical_device_list
 *  @brief Opens physical device
 *
 */
air_status_code_e open_device(iop_physical_device_t *pdev);

/**
 *  @return AIR_SUCCESSFUL if the operation was completed correctly
 *  @param[in] dev_id Physical Device index on physical_device_list
 *  @brief Closes physical device
 *
 */
air_status_code_e close_device(iop_physical_device_t *pdev);

/**
 *  @return AIR_SUCCESSFUL if the operation was completed correctly
 *  @param[in] dev_id Logical Device index on physical_device_list
 *  @param[in] route_id Logical route index on devices' logical_routes
 *  @param[in] buffer Pointer to message
 *  @param[in] len    Message length to send
 *  @brief Writes message to logical device and logical route specified.
 *
 */
air_status_code_e write_to_device(iop_logical_device_t *dev, int route_id, char* buffer, int len);

/**
 *  @return AIR_SUCCESSFUL if the operation was completed correctly
 *  @param[in] dev_id Physical Device index on physical_device_list
 *  @param[out] route_id Physical route index on devices' physical_routes for message received
 *  @param[out] buffer Pointer to message received
 *  @param[out] len    Message length received
 *  @brief Reads message from device.
 *
 */
air_status_code_e read_from_device(iop_physical_device_t *dev, int* route_id, char* buffer, unsigned int* len);
    
    
#endif
