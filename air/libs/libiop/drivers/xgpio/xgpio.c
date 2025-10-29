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

#define GPIO_DEVICE_ID XPAR_XGPIOPS_0_DEVICE_ID

air_u32_t iop_gpio_init(iop_device_driver_t *iop_dev, void *arg)
{

    iop_gpio_device_t *device = (iop_gpio_device_t *)iop_dev;
    XGpioPs *gpioInstPtr = (XGpioPs *)(device->dev.driver);

    air_u32_t ret = AIR_SUCCESSFUL;

    air_i32_t status;
    air_u16_t times = 0;
    air_u8_t mode = 0;
    // XCanPs *canInstPtr = &xilCanPsInst;
    XGpioPs_Config *ConfigPtr;

    air_u32_t writeMask = ~(device->writeMask);

    iop_debug("  :: xGPIO :: device initializing... ");

    ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
    if (ConfigPtr == NULL)
    {

        iop_debug("ERROR: device does not exist\n");
        ret = AIR_INVALID_CONFIG;
    }
    else
    {

        status = XGpioPs_CfgInitialize(gpioInstPtr, ConfigPtr, ConfigPtr->BaseAddr);
        if (status != XST_SUCCESS)
        {
            iop_debug(" ERROR: device could not be initialized\n");
            ret = AIR_IO_ERROR;
        }
    }
    else
    {
        iop_debug(" Success.\n");
    }

    iop_debug("  :: xGPIO :: Setting pin directions (Value:0x%04x)\n", writeMask);
    XGpioPs_SetDirection(gpioInstPtr, (u8)0, writeMask);

    if (ret == AIR_SUCCESSFUL)
    {
        iop_debug("  :: xGPIO :: device initialized\n");
    }

    return ret;
}

air_u32_t iop_gpio_read(iop_device_driver_t *iop_dev, void *arg)
{

    iop_gpio_device_t *device = (iop_gpio_device_t *)iop_dev;
    XGpioPs *gpioInstPtr = (XGpioPs *)(device->dev.driver);

    air_u32_t ret = AIR_SUCCESSFUL;

    air_i32_t status;
    gpio_header_t *header;

    air_u32_t readValue = 58;
    if (arg == NULL)
    {

        ret = AIR_INVALID_PARAM;
    }
    else
    {

        /* get IOP buffer */
        iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;
        iop_buffer_t *iop_buffer = wrapper->buffer;

        if (iop_buffer->v_addr == NULL)
        {

            iop_debug("  :: xGPIO :: iop_gpio_read error iop_buffer doesn't exist\n");
            ret = AIR_INVALID_POINTER;
        }
        else
        {

            status = XGpioPs_Read(gpioInstPtr, device->port);
            if (status == XST_SUCCESS)
            {

                iop_debug("  :: xGPIO :: fake read :: SUCCESS\n");

                iop_buffer->header_off = 0;
                iop_buffer->header_size = iop_buffer->payload_off = sizeof(gpio_header_t);
                iop_buffer->payload_size = sizeof(readValue);

                header = (gpio_header_t *)get_header(iop_buffer);
                iop_debug("  :: xGPIO :: iop_gpio_read: Reading pin#%d (%x)\n", header->pin, &header);
                if ((header->pin > 32) != 0) // TODO: Change pin limit depending on BSP
                {
                    iop_debug("  :: xGPIO :: iop_gpio_read: Error: Trying to read beyond valid pin number\n");
                    ret = AIR_INVALID_CONFIG;
                    return;
                }
                readValue = XGpioPs_ReadPin(gpioInstPtr, header->pin);
                (void)memcpy(get_payload(iop_buffer), &readValue, get_payload_size(iop_buffer));

                iop_debug("  :: xGPIO :: iop_gpio_read :: SUCCESS: %04x\n", readValue);
                ret = AIR_SUCCESSFUL;
            }
            else
            {
                ret = AIR_NO_ACTION;
            }
        }
    }

    return ret;
}

air_u32_t iop_gpio_write(iop_device_driver_t *iop_dev, void *arg)
{

    iop_debug("  :: xGPIO :: iop_gpio_write running\n");

    iop_gpio_device_t *device = (iop_gpio_device_t *)iop_dev;
    XGpioPs *gpioInstPtr = (XGpioPs *)(device->dev.driver);

    air_u32_t ret = AIR_SUCCESSFUL;

    air_i32_t status;
    gpio_header_t *header;

    static air_u32_t writeValue;

    if (arg == NULL)
    {

        iop_debug("  :: xGPIO :: iop_gpio_write error no arg\n");

        ret = AIR_INVALID_PARAM;
    }
    else
    {

        /* get IOP buffer */
        iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;
        iop_buffer_t *iop_buffer = wrapper->buffer;

        if (iop_buffer->v_addr == NULL)
        {

            iop_debug("  :: xGPIO :: iop_gpio_write error iop_buffer doesn't exist\n");
            ret = AIR_INVALID_POINTER;
        }
        else
        {

            header = (gpio_header_t *)get_header(iop_buffer);

            iop_debug("  :: xGPIO :: header :: pin # = %d\n", header->pin);
            if (((device->writeMask >> header->pin) && 0x1) == 0) // Error if pin is set to read
            {
                iop_debug("  :: xGPIO :: Error: Trying to write a read-only pin\n");
                ret = AIR_INVALID_CONFIG;
                return;
            }
            if ((header->pin > 32) != 0) // TODO: Change pin limit depending on BSP
            {
                iop_debug("  :: xGPIO :: Error: Trying to write beyond valid pin number\n");
                ret = AIR_INVALID_CONFIG;
                return;
            }
            (void)memcpy(&writeValue, get_payload(iop_buffer), get_payload_size(iop_buffer));

            iop_debug("  :: xGPIO :: payload: 0x%04x size: %d\n", writeValue, get_payload_size(iop_buffer));

            XGpioPs_WritePin(gpioInstPtr, header->pin, (uint32_t)writeValue);
            iop_debug("  :: xGPIO :: iop_gpio_write successful transmit\n");
        }
    }

    return ret;
}