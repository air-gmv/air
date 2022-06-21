/*
 * Copyright (C) 2021  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/air-dev/-/blob/dev/LICENSE
 */
/**
 * @file xsd.h
 * @author TTAO
 * @brief XSD SD Driver - Macros and Definitions
 */


#ifndef __XSD_H__
#define __XSD_H__




#include <ff.h>
#include <air.h>
#include <iop.h>
#include <iop_support.h>
#include <xsd_support.h>




#define MAX_NUM_FILES 40
#define MAX_FILENAME_SIZE 30




typedef struct xsd_struct{

	FATFS fat;
	FIL fil[MAX_NUM_FILES];
	char filename[MAX_NUM_FILES][MAX_FILENAME_SIZE];
	unsigned short file_opened[MAX_NUM_FILES];
	unsigned long append_byte[MAX_NUM_FILES];
	unsigned short skip_open;
	unsigned short open_for_read;

} xsd_struct;




air_u32_t iop_xsd_init(iop_device_driver_t *iop_dev, void *arg);
air_u32_t iop_xsd_open(iop_device_driver_t *iop_dev, void *arg);
air_u32_t iop_xsd_read(iop_device_driver_t *iop_dev, void *arg);
air_u32_t iop_xsd_write(iop_device_driver_t *iop_dev, void *arg);
air_u32_t iop_xsd_close(iop_device_driver_t *iop_dev, void *arg);


#endif /* __XSD_H__ */
