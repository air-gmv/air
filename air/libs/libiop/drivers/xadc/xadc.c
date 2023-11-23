/*
 * Copyright (C) 2021  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/air-dev/-/blob/dev/LICENSE
 */
/**
 * @file xadc.c
 * @author TTAO
 * @brief XADC Driver
 */

#include <xadc.h>

air_u32_t iop_xadc_init(iop_device_driver_t *iop_dev, void *arg)
{

    iop_adc_device_t *device = (iop_adc_device_t *)iop_dev;
    XAdcPs *xadc = (XAdcPs *)(device->dev.driver);

    XAdcPs_Config *ConfigPtr;

    ConfigPtr = XAdcPs_LookupConfig(XPAR_XADCPS_0_DEVICE_ID);
    if (ConfigPtr == NULL)
    {
        return AIR_NOT_AVAILABLE;
    }

    (void)XAdcPs_CfgInitialize(xadc, ConfigPtr, ConfigPtr->BaseAddress);

    return AIR_SUCCESSFUL;
}

air_u32_t iop_xadc_open(iop_device_driver_t *iop_dev, void *arg)
{

    iop_adc_device_t *device = (iop_adc_device_t *)iop_dev;
    XAdcPs *xadc = (XAdcPs *)(device->dev.driver);

    int Status;

    Status = XAdcPs_SelfTest(xadc);
    if (Status != XST_SUCCESS)
    {
        return AIR_DEVICE_ERROR;
    }

    XAdcPs_SetSequencerMode(xadc, XADCPS_SEQ_MODE_SAFE);

    return AIR_SUCCESSFUL;
}

air_u32_t iop_xadc_read(iop_device_driver_t *iop_dev, void *arg)
{

    iop_adc_device_t *device = (iop_adc_device_t *)iop_dev;
    XAdcPs *xadc = (XAdcPs *)(device->dev.driver);

    iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;
    iop_buffer_t *iop_buffer = wrapper->buffer;

    float data[12];
    air_u16_t raw_data;

    raw_data = XAdcPs_GetAdcData(xadc, XADCPS_CH_TEMP);
    data[0] = XAdcPs_RawToTemperature(raw_data);

    raw_data = XAdcPs_GetMinMaxMeasurement(xadc, XADCPS_MAX_TEMP);
    data[1] = XAdcPs_RawToTemperature(raw_data);

    raw_data = XAdcPs_GetMinMaxMeasurement(xadc, XADCPS_MIN_TEMP);
    data[2] = XAdcPs_RawToTemperature(raw_data & 0xFFF0);

    raw_data = XAdcPs_GetAdcData(xadc, XADCPS_CH_VCCPINT);
    data[3] = XAdcPs_RawToVoltage(raw_data);

    raw_data = XAdcPs_GetMinMaxMeasurement(xadc, XADCPS_MAX_VCCPINT);
    data[4] = XAdcPs_RawToVoltage(raw_data);

    raw_data = XAdcPs_GetMinMaxMeasurement(xadc, XADCPS_MIN_VCCPINT);
    data[5] = XAdcPs_RawToVoltage(raw_data);

    raw_data = XAdcPs_GetAdcData(xadc, XADCPS_CH_VCCPAUX);
    data[6] = XAdcPs_RawToVoltage(raw_data);

    raw_data = XAdcPs_GetMinMaxMeasurement(xadc, XADCPS_MAX_VCCPAUX);
    data[7] = XAdcPs_RawToVoltage(raw_data);

    raw_data = XAdcPs_GetMinMaxMeasurement(xadc, XADCPS_MIN_VCCPAUX);
    data[8] = XAdcPs_RawToVoltage(raw_data);

    raw_data = XAdcPs_GetAdcData(xadc, XADCPS_CH_VCCPDRO);
    data[9] = XAdcPs_RawToVoltage(raw_data);

    raw_data = XAdcPs_GetMinMaxMeasurement(xadc, XADCPS_MAX_VCCPDRO);
    data[10] = XAdcPs_RawToVoltage(raw_data);

    raw_data = XAdcPs_GetMinMaxMeasurement(xadc, XADCPS_MIN_VCCPDRO);
    data[11] = XAdcPs_RawToVoltage(raw_data);

    adc_header_t header;
    header.id = 1;

    xadc_copy_header(iop_dev, wrapper, &header);
    iop_buffer->payload_off = iop_buffer->header_size;
    iop_buffer->payload_size = sizeof(data);

    (void)memcpy(get_payload(iop_buffer), data, sizeof(data));

    return AIR_SUCCESSFUL;
}

air_u32_t iop_xadc_close(iop_device_driver_t *iop_dev, void *arg)
{

    iop_adc_device_t *device = (iop_adc_device_t *)iop_dev;
    XAdcPs *xadc = (XAdcPs *)(device->dev.driver);

    return AIR_SUCCESSFUL;
}
