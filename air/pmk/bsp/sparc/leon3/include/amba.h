/*
 * Copyright (C) 2008-2012  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author pfnf
 * @brief AMBA Plug & Play handling functions
 */

#ifndef __AMBA_H__
#define __AMBA_H__

#include <air_arch.h>
#include <gaisler.h>

/**
 * @defgroup bsp_leon_amba BSP - LEON 3 & 4 AMBA
 * @brief Board Support package AMBA scanning functions
 * @{
 */

/**
 * @brief Maximum AHB master devices to hold in memory
 */
#define AHB_MASTERS (16)
/**
 * @brief Maximum AHB slave devices to hold in memory
 */
#define AHB_SLAVES (16)
/**
 * @brief Maximum APB slave devices to hold in memory
 */
#define APB_SLAVES (32)

/**
 * @brief Available unit for clock gating
 */
typedef enum
{
    GATE_ETH0 = 0,
    GATE_ETH1 = 1,
    GATE_SPWR = 2,
    GATE_PCI = 3,
    GATE_1553 = 4,
    GATE_CAN = 5
} clock_gating_device;

/**
 * @brief APB device information
 */
typedef struct
{

    unsigned int start; /**< pointer to device's memory area      */
    unsigned int irq;   /**< device's IRQ line                    */

} amba_apb_dev_t;

/**
 * @brief AHB device information
 */
typedef struct
{

    unsigned int start[4]; /**< pointer to device's memory area        */
    unsigned int irq;      /**< device's IRQ line                      */
    unsigned int ver;      /**< device's version                       */

} amba_ahb_dev_t;

/**
 * @brief AMBA AHB device lookup table
 */
typedef struct
{

    /** number of devices in the table                                      */
    volatile int count;
    /**< addresses to the devices configuration tables                      */
    unsigned int *addr[AHB_MASTERS];

} amba_ahb_table_t;

/**
 * @brief AMBA APB device lookup table
 */
typedef struct
{

    /** number of devices in the table                                      */
    volatile int count;
    /** addresses to the devices configuration                              */
    unsigned int *addr[APB_SLAVES];
    /** pointer to AHB slave (which is a APB master)                        */
    unsigned int apb_masters[APB_SLAVES];

} amba_apb_table_t;

/**
 * @brief AMBA configuration lookup table
 */
typedef struct
{

    unsigned int io_area;         /**< AMBA I/O area                       */
    amba_ahb_table_t ahb_masters; /**< list of AHB masters                 */
    amba_ahb_table_t ahb_slaves;  /**< list of AHB slaves                  */
    amba_apb_table_t apb_slaves;  /**< list of APB slaves                  */

} amba_confarea_t;

/**
 * @brief The Clock Gating register mapping
 */
typedef struct
{
    unsigned int unlock;       /**< 0x00 Unlock register */
    unsigned int clock_enable; /**< 0x04 Clock enable register */
    unsigned int core_reset;   /**< 0x08 Core reset register */
    unsigned int override;     /**< 0x0C CPU/FPU override register */
} clkgate_regs;

/**
 * @brief Setups the AMBA configuration area for easy lookup
 * @param amba_conf Pointer to the AMBA configuration area
 * @param io_area AMBA I/O area address
 */
void amba_setup(amba_confarea_t *amba_conf, air_uptr_t io_area);

/**
 * @brief Gets an AHB master device from the lookup table
 * @param amba_conf Pointer to the AMBA configuration area
 * @param vendor Device vendor Id
 * @param device Device Id
 * @param index Index of the device (-1 for the first entry)
 * @param dev Pointer to hold the device information
 * @return 1 if a device was found, 0 otherwise
 */
int amba_get_ahb_master(amba_confarea_t *amba_conf, int vendor, int device, int index, amba_ahb_dev_t *dev);

/**
 * @brief Gets an AHB slave device from the lookup table
 * @param amba_conf Pointer to the AMBA configuration area
 * @param vendor Device vendor Id
 * @param device Device Id
 * @param index Index of the device (-1 for the first entry)
 * @param dev Pointer to hold the device information
 * @return 1 if a device was found, 0 otherwise
 */
int amba_get_ahb_slave(amba_confarea_t *amba_conf, int vendor, int device, int index, amba_ahb_dev_t *dev);

/**
 * @brief Gets an AHB (master or slave) device from the lookup table
 * @param amba_conf Pointer to the AMBA configuration area
 * @param vendor Device vendor Id
 * @param device Device Id
 * @param dev Pointer to hold the device information
 * @return 1 if a device was found, 0 otherwise
 */
int amba_get_ahb_device(amba_confarea_t *amba_conf, int vendor, int device, amba_ahb_dev_t *dev);

/**
 * @brief Gets an APB slave device from the lookup table
 * @param amba_conf Pointer to the AMBA configuration area
 * @param vendor Device vendor Id
 * @param device Device Id
 * @param index Index of the device (-1 for the first entry)
 * @param dev Pointer to hold the device information
 * @return 1 if a device was found, 0 otherwise
 */
int amba_get_apb_slave(amba_confarea_t *amba_conf, int vendor, int device, int index, amba_apb_dev_t *dev);

/**
 * 	@brief Returns the number of APB Slave devices that have a given vendorid
 *   and deviceid
 *  @param [in] ambaconf: Amba bus plug and play memory structure
 *  @param [in] vendor: VendorID
 *  @param [in] device: DeviceID
 */
int amba_get_number_apbslv_devices(amba_confarea_t *amba_conf, int vendor, int device);

/**
 * @brief Enable device with clock gating
 * @param clk_amba_bus AMBA bus where the clock gating is located
 * @param core_to_enable Which device to enable.
 */
int clock_gating_enable(amba_confarea_t *clk_amba_bus, clock_gating_device core_to_enable);

/**
 * @brief Disable device with clock gating
 * @param clk_amba_bus AMBA bus where the clock gating is located
 * @param core_to_disable Which device to enable.
 */
int clock_gating_disable(amba_confarea_t *clk_amba_bus, clock_gating_device core_to_disable);

/*
 * AMBA Plug & Play configuration area
 */
amba_confarea_t amba_confarea;

/** @} */

#endif /* __AMBA_H__ */
