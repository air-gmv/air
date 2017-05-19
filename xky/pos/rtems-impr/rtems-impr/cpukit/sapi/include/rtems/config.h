/**
 *  @file
 *  config.h
 *
 *  @brief the table of user defined configuration parameters.
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
 *  592     | 17/11/2008  | mcoutinho    | IPR 64
 *  592     | 17/11/2008  | mcoutinho    | IPR 65
 *  1879    | 13/03/2009  | mcoutinho    | IPR 59
 *  3522    | 29/06/2009  | mcoutinho    | IPR 531
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_API Super API
 *  @{
 */

/**
 *  @defgroup SUPER_API_CONFIG Configuration
 *
 *  @brief The Configuration component contains the functionality required to allow the
 *  application to configure the resources need of RTEMS
 *  @{
 */

#ifndef _RTEMS_CONFIG_H
#define _RTEMS_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/score/object.h>

#include <rtems/rtems/config.h>

#include <rtems/extension.h>
#include <rtems/io.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif

#if defined(RTEMS_MULTIPROCESSING)

      /**
    *  @brief Multiprocessor Configuration Table
    *
    *  This table defines the multiprocessor system
    *  characteristics which must be known by RTEMS in a multiprocessor
    *  system.
    */
   typedef struct
   {
      uint32_t node; /* local node number */
      uint32_t maximum_nodes; /* maximum # nodes in system */
      uint32_t maximum_global_objects; /* maximum # global objects */
      uint32_t maximum_proxies; /* maximum # proxies */
      rtems_mpci_table *User_mpci_table; /* pointer to MPCI table */
      void *User_mpci_table; /* pointer to MPCI table */
   } rtems_multiprocessing_table;
#endif

   /**
    *  @brief Configuration Table
    *
    *  The information contained in this table is required in all
    *  RTEMS systems, whether single or multiprocessor.  This
    *  table primarily defines the following:
    *\n
    *     + location and size of the RTEMS Workspace\n
    *     + microseconds per clock tick\n
    *     + clock ticks per task timeslice\n
    *     + required number of each object type for each API configured\n
    */
   typedef struct
   {
      void *work_space_start;
      uint32_t work_space_size;
      uint32_t maximum_extensions;
      uint32_t microseconds_per_tick;
      uint32_t ticks_per_timeslice;
      uint32_t maximum_drivers;
      uint32_t number_of_device_drivers;
      rtems_driver_address_table *Device_driver_table;
      uint32_t number_of_initial_extensions;
      rtems_extensions_table *User_extension_table;
#if defined(RTEMS_MULTIPROCESSING)
      rtems_multiprocessing_table *User_multiprocessing_table;
#endif
      rtems_api_configuration_table *RTEMS_api_configuration;
   } rtems_configuration_table;

   /*
    *  The following are provided strictly for the convenience of
    *  the user.  They are not used in RTEMS itself.
    */

   /**
    * @brief configuration table
    **/
   extern rtems_configuration_table *_Configuration_Table;


#if defined(RTEMS_MULTIPROCESSING)
   /**
    * @brief configuration multiprocessor table
    **/
   extern rtems_multiprocessing_table *_Configuration_MP_table;
#endif

   /*
    *  Some handy macros to avoid dependencies on either the BSP
    *  or the exact format of the configuration table.
    */

   /**
    * @brief get the configuration table
    **/
#define rtems_configuration_get_table() \
        (&_Configuration_Table)

   /**
    * @brief get the workspace start of the configuration table
    **/
#define rtems_configuration_get_work_space_start() \
        (_Configuration_Table->work_space_start)

   /**
    * @brief get the workspace size of the configuration table
    **/
#define rtems_configuration_get_work_space_size() \
        (_Configuration_Table->work_space_size)
   /**
    * @brief get the workspace number of maximum extensions of the configuration table
    **/
#define rtems_configuration_get_maximum_extensions() \
        (_Configuration_Table->maximum_extensions)
   /**
    * @brief get the workspace microseconds per clock tick of the configuration table
    **/
#define rtems_configuration_get_microseconds_per_tick() \
        (_Configuration_Table->microseconds_per_tick)

   /**
    * @brief get the workspace miliseconds per clock tick of the configuration table
    **/
#define rtems_configuration_get_milliseconds_per_tick() \
        (_Configuration_Table->microseconds_per_tick / 1000)

   /**
    * @brief get the workspace number of clock ticks per timeslice of the configuration table
    **/
#define rtems_configuration_get_ticks_per_timeslice() \
        (_Configuration_Table->ticks_per_timeslice)

   /**
    * @brief get the workspace number of device drivers of the configuration table
    **/
#define rtems_configuration_get_number_of_device_drivers() \
        (_Configuration_Table->number_of_device_drivers)

   /**
    * @brief get the workspace device driver table of the configuration table
    **/
#define rtems_configuration_get_device_driver_table() \
        (_Configuration_Table->device_driver_table)

   /**
    * @brief get the workspace number of initial extensions of the configuration table
    **/
#define rtems_configuration_get_number_of_initial_extensions() \
        (_Configuration_Table->number_of_initial_extensions)

   /**
    * @brief get the workspace user extension table of the configuration table
    **/
#define rtems_configuration_get_user_extension_table() \
        (_Configuration_Table->user_extension_table)

#if defined(RTEMS_MULTIPROCESSING)
#define rtems_configuration_get_user_multiprocessing_table() \
        (_Configuration_Table->User_multiprocessing_table)
#endif

   /**
    * @brief get the workspace RTEMS API configuration of the configuration table
    **/
#define rtems_configuration_get_rtems_api_configuration() \
        (_Configuration_Table->RTEMS_api_configuration)

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
