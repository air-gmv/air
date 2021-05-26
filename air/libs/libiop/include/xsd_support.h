/*
 * Copyright (C) 2021  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/air-dev/-/blob/dev/LICENSE
 */
/**
 * @file xsd_support.h
 * @author TTAO
 * @brief XSD support structures and function definitions
 */


#ifndef __XSD_SUPPORT_H__
#define __XSD_SUPPORT_H__



#include <iop.h>
#include <iop_support.h>



/**
 * @brief SD Device.
 */
typedef struct {

    iop_device_driver_t dev;
    unsigned short id;
    unsigned short format;
    unsigned short number_files;
    sd_header_t *header_read;
    unsigned short all_read;

} iop_sd_device_t;



/**
 *
 * @param arg: Pointer to the Physical device struct.
 */
void xsd_writer(air_uptr_t arg);



/**
 *
 * @param arg: Pointer to the Physical device struct.
 */
void xsd_reader(air_uptr_t arg);



/**
 * Copies an SD header to a wrapper.
 * @param iop_dev: Physical device, not used.
 * @param wrapper: IOP wrapper.
 * @param header: SD header to copy to the wrapper.
 */
void xsd_copy_header(iop_physical_device_t *iop_dev, iop_wrapper_t *wrapper, iop_header_t *header);



/**
 * Compares two SD headers.
 * @param wrapper: IOP wrapper with an SD header.
 * @param header: SD header.
 * @return 1 if the SD headers match, 0 otherwise.
 */
uint32_t xsd_compare_header(iop_wrapper_t *wrapper, iop_header_t *header);


#endif /* __XSD_SUPPORT_H__ */
