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
#define AHB_MASTERS                             (64)
/**
 * @brief Maximum AHB slave devices to hold in memory
 */
#define AHB_SLAVES                              (64)
/**
 * @brief Maximum APB slave devices to hold in memory
 */
#define APB_SLAVES                              (64)

/**
 * @brief APB device information
 */
typedef struct {

    unsigned int start;           /**< pointer to device's memory area      */
    unsigned int irq;             /**< device's IRQ line                    */

} amba_apb_dev_t;

/**
 * @brief AHB device information
 */
typedef struct {

    unsigned int start[4];      /**< pointer to device's memory area        */
    unsigned int irq;           /**< device's IRQ line                      */
    unsigned int ver;           /**< device's version                       */

} amba_ahb_dev_t;

/**
 * @brief AMBA AHB device lookup table
 */
typedef struct {

    /** number of devices in the table                                      */
    volatile int count;
    /**< addresses to the devices configuration tables                      */
    unsigned int *addr[AHB_MASTERS];

} amba_ahb_table_t;

/**
 * @brief AMBA APB device lookup table
 */
typedef struct {

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
typedef struct {

    unsigned int io_area;          /**< AMBA I/O area                       */
    amba_ahb_table_t ahb_masters;  /**< list of AHB masters                 */
    amba_ahb_table_t ahb_slaves;   /**< list of AHB slaves                  */
    amba_apb_table_t apb_slaves;   /**< list of APB slaves                  */

} amba_confarea_t;

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
int amba_get_ahb_master(
        amba_confarea_t *amba_conf, int vendor, int device, int index,
        amba_ahb_dev_t *dev);

/**
 * @brief Gets an AHB slave device from the lookup table
 * @param amba_conf Pointer to the AMBA configuration area
 * @param vendor Device vendor Id
 * @param device Device Id
 * @param index Index of the device (-1 for the first entry)
 * @param dev Pointer to hold the device information
 * @return 1 if a device was found, 0 otherwise
 */
int amba_get_ahb_slave(
        amba_confarea_t *amba_conf, int vendor, int device, int index,
        amba_ahb_dev_t *dev);

/**
 * @brief Gets an AHB (master or slave) device from the lookup table
 * @param amba_conf Pointer to the AMBA configuration area
 * @param vendor Device vendor Id
 * @param device Device Id
 * @param dev Pointer to hold the device information
 * @return 1 if a device was found, 0 otherwise
 */
int amba_get_ahb_device(
        amba_confarea_t *amba_conf, int vendor, int device, amba_ahb_dev_t *dev);

/**
 * @brief Gets an APB slave device from the lookup table
 * @param amba_conf Pointer to the AMBA configuration area
 * @param vendor Device vendor Id
 * @param device Device Id
 * @param index Index of the device (-1 for the first entry)
 * @param dev Pointer to hold the device information
 * @return 1 if a device was found, 0 otherwise
 */
int amba_get_apb_slave(
        amba_confarea_t *amba_conf, int vendor, int device, int index,
        amba_apb_dev_t *dev);

/*
 * AMBA Plug & Play configuration area
 */
amba_confarea_t amba_confarea;


/** @} */

#endif /* __AMBA_H__ */
