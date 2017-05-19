/**
 *  @file
 *  io.h
 *
 *  @brief all the constants and structures associated with the Input/Output
 *  Manager. This manager provides a well defined mechanism for accessing device
 *  drivers and a structured methodology for organizing device drivers.
 *
 *  Directives provided are:
 *\n
 *     + initialize a device driver\n
 *     + open a device driver\n
 *     + close a device driver\n
 *     + read from a device driver\n
 *     + write to a device driver\n
 *     + special device services\n
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  4546    | 28/09/2009  | mcoutinho    | IPR 778
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6116    | 18/02/2010  | mcoutinho    | IPR 1818
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_API Super API
 *  @{
 */

/**
 *  @defgroup SUPER_API_IO Input/Output Manager
 *
 *  @brief The Input/Output Manager consists on an API that allows the user to
 *  interface with the device drivers.
 *  @{
 */

#ifndef _RTEMS_IO_H
#define _RTEMS_IO_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/rtems/status.h>
#include <string.h>
#include <stddef.h>
    
   /*
    *
    *  The following defines the types for:
    *
    *    + major and minor numbers
    *    + the return type of a device driver entry
    *    + a pointer to a device driver entry
    *    + an entry in the the Device Driver Address Table.  Each entry in this
    *      table corresponds to an application provided device driver and
    *      defines the entry points for that device driver.
    */

   /**
    * @brief major number type
    **/
   typedef uint32_t rtems_device_major_number;

   /**
    * @brief minor number type
    **/
   typedef uint32_t rtems_device_minor_number;

   /**
    * @brief return of a device driver entry
    **/
   typedef rtems_status_code rtems_device_driver;

   /**
    * @brief prototype for a entry on the device driver table
    **/
   typedef rtems_device_driver(*rtems_device_driver_entry)(
                                                            rtems_device_major_number ,
                                                            rtems_device_minor_number ,
                                                            void *
                                                            );

   /**
    * @brief table for a device driver
    **/
   typedef struct
   {
      /**
       * @brief initialization procedure
       **/
      rtems_device_driver_entry initialization_entry;
      /**
       * @brief open request procedure
       **/
      rtems_device_driver_entry open_entry;
      /**
       * @brief close request procedure
       **/
      rtems_device_driver_entry close_entry;
      /**
       * @brief read request procedure
       **/
      rtems_device_driver_entry read_entry;
      /**
       * @brief write request procedure
       **/
      rtems_device_driver_entry write_entry;
      /**
       * @brief special functions procedure
       **/
      rtems_device_driver_entry control_entry;
   } rtems_driver_address_table;

   /**
    * @brief table of Input/Output device names
    **/
   typedef struct
   {
      /**
       * @brief device name
       **/
      char *device_name;
      /**
       * @brief device name length
       **/
      size_t device_name_length;
      /**
       * @brief device major number
       **/
      rtems_device_major_number major;
      /**
       * @brief device minor number
       **/
      rtems_device_minor_number minor;
   } rtems_driver_name_t;

   /*
    *  The following declare the data required to manage the Driver
    *  Address Table and Device Name Table.
    */

   /**
    * @brief number of Input/Output Device Drivers
    **/
   extern uint32_t _IO_Number_of_drivers;

   /**
    * @brief Input/Output driver address table
    **/
   extern rtems_driver_address_table *_IO_Driver_address_table;

   /**
    *  @brief initialize Input/Ouptut Manager
    *
    *  The IO manager has been extended to support runtime driver
    *  registration. The driver table is now allocated in the
    *  workspace.
    *
    *  @param[in] driver_table the driver table to initialize
    *  @param[in] drivers_in_table the number of drivers in the table
    *  @param[in] number_of_drivers the number of drivers to initialize
    */
   void _IO_Manager_initialization(
                                   rtems_driver_address_table *driver_table ,
                                   uint32_t drivers_in_table ,
                                   uint32_t number_of_drivers
                                   );

   /**
    *  @brief initialize a device driver
    *
    *  This routine is the initialization directive of the IO manager.
    *
    *  @param[in] major device driver number
    *  @param[in] minor device number
    *  @param[in] argument pointer to argument(s)
    *
    *  @return returns the status
    */
   rtems_status_code rtems_io_initialize(
                                         rtems_device_major_number major ,
                                         rtems_device_minor_number minor ,
                                         void *argument
                                         );

   /**
    *  @brief open a device driver
    *
    *  This routine is the open directive of the IO manager.
    *
    *  @param[in] major device driver number
    *  @param[in] minor device number
    *  @param[in] argument pointer to argument(s)
    *
    *  @return returns the status
    */
   rtems_status_code rtems_io_open(
                                   rtems_device_major_number major ,
                                   rtems_device_minor_number minor ,
                                   void *argument
                                   );

   /**
    *  @brief close a device driver
    *
    *  This routine is the close directive of the IO manager.
    *
    *  @param[in] major device driver number
    *  @param[in] minor device number
    *  @param[in] argument pointer to argument(s)
    *
    *  @return returns the status
    */
   rtems_status_code rtems_io_close(
                                    rtems_device_major_number major ,
                                    rtems_device_minor_number minor ,
                                    void *argument
                                    );

   /**
    *  @brief read from a device driver
    *
    *  This routine is the read directive of the IO manager.
    *
    *  @param[in] major device driver number
    *  @param[in] minor device number
    *  @param[in] argument pointer to argument(s)
    *
    *  @return returns the status
    */
   rtems_status_code rtems_io_read(
                                   rtems_device_major_number major ,
                                   rtems_device_minor_number minor ,
                                   void *argument
                                   );

   /**
    *  @brief write to a device driver
    *
    *  This routine is the write directive of the IO manager.
    *
    *  @param[in] major device driver number
    *  @param[in] minor device number
    *  @param[in] argument pointer to argument(s)
    *
    *  @return returns the status
    */
   rtems_status_code rtems_io_write(
                                    rtems_device_major_number major ,
                                    rtems_device_minor_number minor ,
                                    void *argument
                                    );

   /**
    *  @brief control a device driver
    *
    *  This routine is the control directive of the IO manager.
    *
    *  @param[in] major device driver number
    *  @param[in] minor device number
    *  @param[in] argument pointer to argument(s)
    *
    *  @return returns the status
    */
   rtems_status_code rtems_io_control(
                                      rtems_device_major_number major ,
                                      rtems_device_minor_number minor ,
                                      void *argument
                                      );

   /**
    *  @brief initialize all device drivers
    *
    *  This routine initializes all device drivers
    *  Input Paramters:   NONE
    */
   void _IO_Initialize_all_drivers(void);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

/**  
 *  @}
 */

/**  
 *  @}
 */

