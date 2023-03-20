/*
 * Copyright (C) 2019-2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file
 * \author lumm
 * \brief GPIO driver adapted for LIBIOP
 */

#include <xgpio.h>
#include <xgpiops.h>
#include <xparameters.h>
#include "gpio.h"

air_u32_t iop_gpio_init(iop_device_driver_t *iop_dev, void *arg) {

    iop_gpio_device_t *device = (iop_gpio_device_t *)iop_dev;
    XGpioPs *gpioInstPtr = (XGpioPs *)(device->dev.driver);

    air_u32_t ret = AIR_SUCCESSFUL;

    air_i32_t status;

    air_u32_t writeMask =  (air_u32_t) device->writeMask;
    
    iop_debug("  :: xGPIO :: device initializing... ");
    iop_debug("  Mask: %04x ", (uint32_t) writeMask);

    if (!gpioInit()) {
            iop_debug(" ERROR: device could not be initialized\n");
            return AIR_IO_ERROR;
    } else {
        iop_debug(" Success.\n");
    }

    applyGPIOMask(writeMask);

    if (ret == AIR_SUCCESSFUL) iop_debug("  :: xGPIO :: device initialized\n");

    return ret;
}

air_u32_t iop_gpio_read(iop_device_driver_t *iop_dev, void *arg) {

    iop_gpio_device_t *device = (iop_gpio_device_t *)iop_dev;
    XGpioPs *gpioInstPtr = (XGpioPs *)(device->dev.driver);

    air_u32_t ret = AIR_SUCCESSFUL;

    air_i32_t status;
    gpio_header_t *header;

    air_u32_t readValue=58;
    air_u32_t readValue;
    if (arg == NULL) {

        ret = AIR_INVALID_PARAM;
    } else {

        /* get IOP buffer */
        iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;
        iop_buffer_t *iop_buffer = wrapper->buffer;

        if (iop_buffer->v_addr == NULL) {

            iop_debug("  :: xGPIO :: iop_gpio_read error iop_buffer doesn't exist\n");
            ret = AIR_INVALID_POINTER;
        } else {
            iop_debug("  :: xGPIO :: iop_gpio_read: Reading bank %d\n", device->port);
            readValue = XGpioPs_Read(gpioInstPtr, device->port);
            iop_debug("  :: xGPIO :: iop_gpio_read: readValue: %04x\n", readValue);
        }
    }

    return ret;
}

air_u32_t iop_gpio_write(iop_device_driver_t *iop_dev, void *arg) {

    iop_debug("  :: xGPIO :: iop_gpio_write running\n");

    iop_gpio_device_t *device = (iop_gpio_device_t *)iop_dev;
    XGpioPs *gpioInstPtr = (XGpioPs *)(device->dev.driver);

    air_u32_t ret = AIR_SUCCESSFUL;

    air_i32_t status;
    gpio_header_t *header;

    static air_u32_t writeValue;

    if (arg == NULL) {

        iop_debug("  :: xGPIO :: iop_gpio_write error no arg\n");

        ret = AIR_INVALID_PARAM;
    } else {

        /* get IOP buffer */
        iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;
        iop_buffer_t *iop_buffer = wrapper->buffer;

        if (iop_buffer->v_addr == NULL) {

            iop_debug("  :: xGPIO :: iop_gpio_write error iop_buffer doesn't exist\n");
            ret = AIR_INVALID_POINTER;
        } else {

            header = (gpio_header_t *)get_header(iop_buffer);

            iop_debug("  :: xGPIO :: header :: pin # = %d\n",
                header->pin);
            if ( ((device->writeMask >> header->pin) && 0x1) == 0) // Error if pin is set to read
            {
                iop_debug("  :: xGPIO :: Error: Trying to write a read-only pin\n");
                ret = AIR_INVALID_CONFIG;
                return;
            }
            if (header->pin > 32) // TODO: Change pin limit depending on BSP
            {
                iop_debug("  :: xGPIO :: Error: Trying to write beyond valid pin number\n");
                ret = AIR_INVALID_CONFIG;
                return;
            }
            memcpy(&writeValue, get_payload(iop_buffer), get_payload_size(iop_buffer));
            iop_debug("  :: xGPIO :: payload: 0x%04x size: %d\n", writeValue, get_payload_size(iop_buffer));

            setGpio(header->pin, writeValue);
            iop_debug("  :: xGPIO :: iop_gpio_write successful transmit\n");
        }
    }

    return ret;
}