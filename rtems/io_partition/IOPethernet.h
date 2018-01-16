/** 
 * @file IOPethernet.h
 * 
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT 
 *
 * @author Cláudio Silva
 * @brief  Access points to the greth driver
 *
 *
 */

#ifndef _IOPETHERNET_H_
#define _IOPETHERNET_H_

#include "iop.h"


/** 
 *  \param [in]  major : not used
 *  \param [in]  minor : not used
 *  \param [in]  arg : not used. 
 *
 *  \return Status of the operation
 *
 *	returns RTEMS_INTERNAL_ERROR if no greth device was found.	
 *	
 *  \brief Initializes the Greth Core
 **/
rtems_device_driver greth_initialize(rtems_device_major_number  major,
								     rtems_device_minor_number  minor,
								     void *arg);

/** 
 *  \param [in]  major : not used
 *  \param [in]  minor : not used
 *  \param [in]  arg : not used. 
 *
 *  \return Status of the operation
 *  \return RTEMS_NOT_CONFIGURED if the device was not initialized
 *	
 *  \brief prepares the device for communication
 **/									 
rtems_device_driver greth_open(rtems_device_major_number  major,
							   rtems_device_minor_number  minor,
							   void *arg);

/** 
 *  \param [in]  major : not used
 *  \param [in]  minor : not used
 *  \param [in]  arg : not used. 
 *
 *  \return Status of the operation \n
 *	\return RTEMS_NOT_CONFIGURED if the device was not initialized
 *	
 *  \brief stops the device
 **/		
rtems_device_driver greth_close(rtems_device_major_number  major,
							   rtems_device_minor_number  minor,
							   void *arg);

/** 
 *  \param [in]  major : not used
 *  \param [in]  minor : not used
 *  \param [in]  arg : struct greth_args with read buffer
 *
 *  \return Status of the operation: \n
 *  RTEMS_NOT_CONFIGURED if the device was not initialized \n
 *  RTEMS_INVALID_NAME if the user argument (void *arg) is inconsistent \n
 *  RTEMS_RESOURCE_IN_USE there is no data to be read and we can't block \n
 *  RTEMS_SUCCESSFUL otherwise \n
 *	
 *  \brief stops the device 
 **/		
rtems_device_driver greth_read(rtems_device_major_number  major,
							   rtems_device_minor_number  minor,
							   void *arg);

/** 
 *  \param [in]  major : not used
 *  \param [in]  minor : not used
 *  \param [in]  arg : struct greth_args containing the read buffer
 *
 *  \return Status of the operation:\n
 *  RTEMS_NOT_CONFIGURED if the device was not initialized\n
 *  RTEMS_INVALID_NAME if the user argument (void *arg) is inconsistent\n
 *  RTEMS_SUCCESSFUL otherwise\n
 *	
 *  \brief stops the device 
 **/	
rtems_device_driver greth_write(rtems_device_major_number  major,
							   rtems_device_minor_number  minor,
							   void *arg);

#endif

