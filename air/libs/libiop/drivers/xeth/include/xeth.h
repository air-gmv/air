/*
 * Copyright (C) 2021  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/air-dev/-/blob/dev/LICENSE
 */
/**
 * @file xeth.h
 * @author TTAO
 * @brief XETH Ethernet Driver - Macros and Definitions
 *        Based on the Zynq-7000 SoC Technical Reference Manual
 */

#ifndef __XETH_H__
#define __XETH_H__

#include <air.h>
#include <iop.h>
#include <eth_support.h>

/**
 * Xilinx Mac Address Registers
 */
typedef struct xeth_mac_regs
{

    volatile uint32_t mac_addr_lsb;
    volatile uint32_t mac_addr_msb;

} xeth_mac_regs;

/**
 * Xilinx Main Ethernet Registers
 */
typedef struct xeth_regs
{

    volatile uint32_t nw_ctrl;
    volatile uint32_t nw_config;
    volatile uint32_t nw_status;
    volatile uint32_t reserved;
    volatile uint32_t dma_config;
    volatile uint32_t tx_status;
    volatile uint32_t rx_base_addr;
    volatile uint32_t tx_base_addr;
    volatile uint32_t rx_status;
    volatile uint32_t int_status;
    volatile uint32_t int_enable;
    volatile uint32_t int_disable;

} xeth_regs;

/**
 * Buffer Descriptor Entry
 */
typedef struct xeth_bd
{

    uint32_t addr;
    volatile uint32_t config;

} xeth_bd;

/**
 * Ethernet Device Control Structure
 */
typedef struct xeth_struct
{

    xeth_regs *regs;
    xeth_mac_regs *mac_regs;

    unsigned int tx_ptr;
    unsigned int rx_ptr;

    xeth_bd *tx_bd;
    xeth_bd *rx_bd;

    iop_buffer_t *iop_buffers;
    uint8_t *iop_buffers_storage;

    iop_buffer_t **tx_iop_buffer;
    iop_buffer_t **rx_iop_buffer;

} xeth_struct;

/**
 * Initializes and Configures the Ethernet Driver.
 * @param iop_dev Ethernet Device Configuration
 * @param arg Not used
 * @return AIR_DEVICE_NOT_FOUND if an invalid Ethernet Device was selected
 *         AIR_SUCCESSFUL if the Ethernet Driver was successfully initialized
 */
air_u32_t iop_xeth_init(iop_device_driver_t *iop_dev, void *arg);

/**
 * Configures Buffers and Buffers Descriptors.
 * Enables the Ethernet Controller.
 * @param iop_dev Ethernet Device Configuration
 * @param arg Not used
 * @return AIR_SUCCESSFUL if the Ethernet Controller was successfully enabled
 */
air_u32_t iop_xeth_open(iop_device_driver_t *iop_dev, void *arg);

/**
 * Reads from the Ethernet Device.
 * @param iop_dev Ethernet Device Configuration
 * @param arg Struct iop_wrapper_t that should store the received Ethernet Frame.
 * @return AIR_INVALID_PARAM if the IOP wrapper is invalid
 *         AIR_SUCCESSFUL if an Ethernet Frame/Fragment was successfully received and stored in the IOP wrapper
 *         AIR_NO_ACTION if no new Ethernet Frames/Fragments were received
 */
air_u32_t iop_xeth_read(iop_device_driver_t *iop_dev, void *arg);

/**
 * Writes in the Ethernet Device.
 * @param iop_dev Ethernet Device Configuration
 * @param arg Struct iop_wrapper_t with the Ethernet Frame to send
 * @return AIR_INVALID_PARAM if the IOP wrapper is invalid
 *         AIR_INVALID_SIZE if the fragment queue is empty
 *         AIR_SUCCESSFUL if an Ethernet Frame/Fragment was successfully written to the Ethernet Device
 */
air_u32_t iop_xeth_write(iop_device_driver_t *iop_dev, void *arg);

/**
 * Disables the Ethernet Controller.
 * @param iop_dev Ethernet Device Configuration
 * @param arg Not used
 * @return AIR_SUCCESSFUL if the Ethernet Controller was successfully disabled
 */
air_u32_t iop_xeth_close(iop_device_driver_t *iop_dev, void *arg);

#endif /* __XETH_H__ */
