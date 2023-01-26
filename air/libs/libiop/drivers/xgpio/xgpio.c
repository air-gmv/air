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
 * \brief CAN driver adaptar for LIBIOP
 */

#include <xgpio.h>
#include <xgpiops.h>
#include <xparameters.h>

#define GPIO_DEVICE_ID     XPAR_XGPIOPS_0_DEVICE_ID

air_u32_t iop_gpio_init(iop_device_driver_t *iop_dev, void *arg) {

    iop_gpio_device_t *device = (iop_gpio_device_t *)iop_dev;
    XGpioPs *gpioInstPtr = (XGpioPs *)(device->dev.driver);

    air_u32_t ret = AIR_SUCCESSFUL;

    air_i32_t status;
    air_u16_t times = 0;
    air_u8_t mode = 0;
    // XCanPs *canInstPtr = &xilCanPsInst;
    XGpioPs_Config *ConfigPtr;
    
    iop_debug(" GPIO :: device initializing\n");

    ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
    if (ConfigPtr == NULL) {

        iop_debug(" GPIO :: device does not exist\n");
        ret = AIR_INVALID_CONFIG;

    } else {

        status = XGpioPs_CfgInitialize(gpioInstPtr, ConfigPtr, ConfigPtr->BaseAddr);
        if (status != XST_SUCCESS) {
            iop_debug(" :: GPIO :: device could not be initialized\n");
            ret = AIR_IO_ERROR;
        }
    }

    if (ret == AIR_SUCCESSFUL) iop_debug(" GPIO :: device initialized\n");

    return ret;
}

air_u32_t iop_gpio_read(iop_device_driver_t *iop_dev, void *arg) {

    iop_gpio_device_t *device = (iop_gpio_device_t *)iop_dev;
    XGpioPs *gpioInstPtr = (XGpioPs *)(device->dev.driver);

    air_u32_t ret = AIR_SUCCESSFUL;

    air_i32_t status;
    gpio_header_t header;

    static air_u32_t readValue;

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
            
            status = XGpioPs_Read(gpioInstPtr, device->port);
            if (status == XST_SUCCESS) {

                iop_debug("  :: xGPIO :: fake read :: SUCCESS\n");


                iop_buffer->header_off = 0;
                iop_buffer->header_size = iop_buffer->payload_off = sizeof(gpio_header_t);
                iop_buffer->payload_size = sizeof(readValue);
                readValue = 1;

                memcpy(get_header(iop_buffer), &header, get_header_size(iop_buffer));
                memcpy(get_payload(iop_buffer), &readValue, get_payload_size(iop_buffer));

                ret = AIR_SUCCESSFUL;
            } else {
                ret = AIR_NO_ACTION;
            }
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
//    XCanPs *canInstPtr = &xilCanPsInst; CAN ::

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

            iop_debug("  :: xGPIO :: header :: pin # = %d, write = %d\n",
                header->pin, header->write);

            if (header->write == 0) // Error if pin is set to read
            {
                iop_debug("  :: xGPIO :: Error: Trying to write a read-only pin\n");
                ret = AIR_INVALID_CONFIG;
                return;
            }
            else if (header->pin > 117)
            {
                iop_debug("  :: xGPIO :: Error: Trying to beyond valid pin number\n");
                ret = AIR_INVALID_CONFIG;
                return;
            }

            memcpy(&writeValue, get_payload(iop_buffer), get_payload_size(iop_buffer));

            XGpioPs_WritePin(gpioInstPtr, header->pin, writeValue);
            iop_debug("  :: xGPIO :: iop_gpio_write successful transmit\n");
        }
    }

    return ret;
}