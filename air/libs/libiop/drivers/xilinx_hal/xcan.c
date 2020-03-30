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

#include <xcan.h>
#include <xcanps.h>
#include <xparameters.h>
#include "xilCanBitTiming.h"

// static XCanPs xilCanPsInst;

air_u32_t iop_can_init(iop_device_driver_t *iop_dev, void *arg) {

    iop_can_device_t *device = (iop_can_device_t *)iop_dev;
    XCanPs *canInstPtr = (XCanPs *)(device->dev.driver);

    air_u32_t ret = AIR_SUCCESSFUL;

    air_i32_t status;
    air_u16_t times = 0;
    air_u8_t mode = 0;
    // XCanPs *canInstPtr = &xilCanPsInst;
    XCanPs_Config *canConfigPtr;

    /* enable CAN transceiver. @TODO Will depend on target board! */

    /* initialize CAN driver */
    canConfigPtr = XCanPs_LookupConfig(XPAR_XCANPS_0_DEVICE_ID);

    if (canConfigPtr == NULL) {

        iop_debug(" CAN :: device does not exist\n");
        ret = AIR_INVALID_CONFIG;

    } else {

        status = XCanPs_CfgInitialize(canInstPtr, canConfigPtr, canConfigPtr->BaseAddr);
        if (status != XST_SUCCESS) {
            iop_debug(" :: CAN :: controller could not be initialized\n");
            ret = AIR_IO_ERROR;
        }

        /*
         * Run self-test on the device, which verifies basic sanity of the
         * device and the driver.
         */
        status = XCanPs_SelfTest(canInstPtr);
        if (status != XST_SUCCESS) {
            iop_debug(" :: CAN :: controller self test FAILED\n");
            ret = AIR_IO_ERROR;
        }

        /*
         * Enter Configuration Mode so we can setup Baud Rate Prescaler
         * Register (BRPR) and Bit Timing Register (BTR).
         */
        XCanPs_EnterMode(canInstPtr, XCANPS_MODE_CONFIG);
        mode = XCanPs_GetMode(canInstPtr);
        while(mode != XCANPS_MODE_CONFIG)
        {
            if (times++ > 100) ret = AIR_IO_ERROR;
            mode = XCanPs_GetMode(canInstPtr);
        }

        times = 0;

        // 40Kbps
        /*
        * Setup Baud Rate Prescaler Register (BRPR) and
        * Bit Timing Register (BTR).
        */
        // status = XCanPs_SetBaudRatePrescaler(canInstPtr, (air_u8_t)29U);
        // if (status != XST_SUCCESS) {
        //     iop_debug(" :: CAN :: SetBaudRatePrescaler FAILED\n");
        //     ret = AIR_IO_ERROR;
        // }

        // status = XCanPs_SetBitTiming(canInstPtr, (air_u8_t)3U, (air_u8_t)2U, (air_u8_t)15U);
        // if (status != XST_SUCCESS) {
        //     iop_debug(" :: CAN :: SetBitTiming FAILED\n");
        //     ret = AIR_IO_ERROR;
        // }

        /*
        * Setup Baud Rate Prescaler Register (BRPR) and
        * Bit Timing Register (BTR).
        */
        // 125Kbps
        // (void)XCanPs_SetBaudRatePrescaler(canInstPtr, (air_u8_t)11U); // 12 - 1
        // (void)XCanPs_SetBitTiming(canInstPtr, (air_u8_t)0U, (air_u8_t)1U, (air_u8_t)12U);
        // 500Kbps
        // (void)XCanPs_SetBaudRatePrescaler(canInstPtr, (air_u8_t)2U); // 3 - 1
        // (void)XCanPs_SetBitTiming(canInstPtr, (air_u8_t)0U, (air_u8_t)1U, (air_u8_t)12U);
        // 1000Kbps
        (void)XCanPs_SetBaudRatePrescaler(canInstPtr, (air_u8_t)2U); // 3 - 1
        (void)XCanPs_SetBitTiming(canInstPtr, (air_u8_t)0U, (air_u8_t)0U, (air_u8_t)5U);

        /*
        * Enter Normal Mode.
        */
        if (ret != AIR_SUCCESSFUL) {
            ;
        } else {
            XCanPs_EnterMode(canInstPtr, XCANPS_MODE_NORMAL);
            mode = XCanPs_GetMode(canInstPtr);
            while(mode != XCANPS_MODE_NORMAL)
            {
                if (times++ > 100) ret = AIR_IO_ERROR;
                mode = XCanPs_GetMode(canInstPtr);
            }
        }
    }

    if (ret == AIR_SUCCESSFUL) iop_debug(" CAN :: device initialized\n");

    return ret;
}

air_u32_t iop_can_open(iop_device_driver_t *iop_dev, void *arg) {

    air_u32_t ret = AIR_SUCCESSFUL;
    return ret;
}

air_u32_t iop_can_receive(iop_device_driver_t *iop_dev, void *arg) {

    iop_can_device_t *device = (iop_can_device_t *)iop_dev;
    XCanPs *canInstPtr = (XCanPs *)(device->dev.driver);

    air_u32_t ret = AIR_SUCCESSFUL;

    air_i32_t status;
    can_header_t header;
//    XCanPs *canInstPtr = &xilCanPsInst;

    static air_u32_t rxFrame[XCANPS_MAX_FRAME_SIZE/sizeof(air_u32_t)];

    if (arg == NULL) {

        ret = AIR_INVALID_PARAM;
    } else {

        /* get IOP buffer */
        iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;
        iop_buffer_t *iop_buffer = wrapper->buffer;

        if (iop_buffer->v_addr == NULL) {

            iop_debug("  :: xCAN :: iop_can_receive error iop_buffer doesn't exist\n");
            ret = AIR_INVALID_POINTER;
        } else {

            status = XCanPs_Recv(canInstPtr, rxFrame);
            if (status == XST_SUCCESS) {

                iop_debug("  :: xCAN :: receive :: SUCCESS\n");
                iop_debug("%d\n", rxFrame[2]);

                iop_buffer->header_off = 0;
                iop_buffer->header_size = iop_buffer->payload_off = sizeof(can_header_t);
                iop_buffer->payload_size = ((rxFrame[1] & XCANPS_DLCR_DLC_MASK) >> XCANPS_DLCR_DLC_SHIFT) * sizeof(air_u8_t);

                header.extended = (rxFrame[0] & XCANPS_IDR_IDE_MASK) >> XCANPS_IDR_IDE_SHIFT;
                if (header.extended) {
                    header.rtr = rxFrame[0] & XCANPS_IDR_RTR_MASK;
                    header.id = ( ((rxFrame[0] & XCANPS_IDR_ID1_MASK) >> 18U) |
                                ((rxFrame[0] & XCANPS_IDR_ID2_MASK) >> 1U) );
                } else {
                    header.rtr = (rxFrame[0] & XCANPS_IDR_SRR_MASK) >> XCANPS_IDR_SRR_SHIFT;
                    header.id = (rxFrame[0] & XCANPS_IDR_ID1_MASK) >> XCANPS_IDR_ID1_SHIFT;
                }

                memcpy(get_header(iop_buffer), &header, get_header_size(iop_buffer));
                memcpy(get_payload(iop_buffer), &rxFrame[2], get_payload_size(iop_buffer));

                ret = AIR_SUCCESSFUL;
            } else {
                ret = AIR_NO_ACTION;
            }
        }
    }

    return ret;
}

air_u32_t iop_can_transmit(iop_device_driver_t *iop_dev, void *arg) {

    iop_debug("  :: xCAN :: iop_can_transmit running\n");

    iop_can_device_t *device = (iop_can_device_t *)iop_dev;
    XCanPs *canInstPtr = (XCanPs *)(device->dev.driver);

    air_u32_t ret = AIR_SUCCESSFUL;

    air_i32_t status;
    can_header_t *header;
//    XCanPs *canInstPtr = &xilCanPsInst; CAN ::

    static air_u32_t txFrame[XCANPS_MAX_FRAME_SIZE/sizeof(air_u32_t)];

    if (arg == NULL) {

        iop_debug("  :: xCAN :: iop_can_transmit error no arg\n");

        ret = AIR_INVALID_PARAM;
    } else {

        /* get IOP buffer */
        iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;
        iop_buffer_t *iop_buffer = wrapper->buffer;

        if (iop_buffer->v_addr == NULL) {

            iop_debug("  :: xCAN :: iop_can_transmit error iop_buffer doesn't exist\n");
            ret = AIR_INVALID_POINTER;
        } else {

            header = (can_header_t *)get_header(iop_buffer);

            iop_debug("  :: xCAN :: header :: id = %d, extended = %d, rtr = %d, sshot = %d\n",
                header->id, header->extended, header->rtr, header->sshot
            );

            header->id = 2;

            if (header->extended) {
                txFrame[0] = (air_u32_t)XCanPs_CreateIdValue((header->id & 0x1ffc0000), 1, 1, (header->id & 0x3ffff), header->rtr);
            } else {
                txFrame[0] = (air_u32_t)XCanPs_CreateIdValue(header->id, header->rtr, 0, 0, 0);
            }
            txFrame[1] = (air_u32_t)XCanPs_CreateDlcValue(get_payload_size(iop_buffer)/sizeof(air_u8_t));
            memcpy(&txFrame[2], get_payload(iop_buffer), get_payload_size(iop_buffer));

            iop_debug("id = %d, payload_size = %d, payload_off = %d\n",
                      header->id, iop_buffer->payload_size, iop_buffer->payload_off);

            iop_debug("tx0: 0x%x\n", txFrame[0]);
            iop_debug("tx1: 0x%x\n", txFrame[1]);
            iop_debug("tx2: 0x%x\n", txFrame[2]);
            iop_debug("tx3: 0x%x\n", txFrame[3]);

            iop_debug(get_payload(iop_buffer));/*
            for (int i = 0; i < get_payload_size(iop_buffer); i++) {
                iop_debug(get_payload(iop_buffer));
            }*/

            status = XCanPs_Send(canInstPtr, txFrame);
            if (status != XST_SUCCESS) {
                iop_debug("  :: xCAN :: iop_can_transmit error no data transmitted\n");
                ret = AIR_DEVICE_ERROR;
            }
            iop_debug("  :: xCAN :: iop_can_transmit successful transmit\n");
        }
    }

    return ret;
}

air_u32_t iop_can_close(iop_device_driver_t *iop_dev, void *arg) {

    air_u32_t ret = AIR_SUCCESSFUL;
    return ret;
}
