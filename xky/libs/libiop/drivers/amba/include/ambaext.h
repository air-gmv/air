/** 
 *  @file ambaext.h
 * 
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT 
 *
 *  @author Cláudio Silva
 *
 *  The license and distribution terms for this file may be 
 *  found in the file LICENSE at: 
 *  http://www.rtems.com/license/LICENSE.
 * 
 *  @brief Functions that perform amba bus scanning
 * 
 */

#ifndef __AMBAEXT_H__
#define __AMBAEXT_H__

#include <ambapp.h>

int amba_find_next_ahbslv (amba_confarea_type * amba_conf, int vendor, int device,
						   amba_ahb_device * dev, int index);

int amba_get_number_apbslv_devices (amba_confarea_type * amba_conf, int vendor,
									int device);
									
int amba_find_next_apbslv (amba_confarea_type * amba_conf, int vendor, int device,
						   amba_apb_device * dev, int index);
						   
int amba_find_next_ahbmst(amba_confarea_type * amba_conf, int vendor, int device,
                          amba_ahb_device * dev, int index);

#endif
