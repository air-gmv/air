/**
 *  @file
 *  clockdrv.h
 *
 *  @brief describes the Clock Driver for all boards.
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
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9877 $ | $Date: 2011-03-18 18:39:36 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SHARED_BETWEEN_BSPS Shared between BSPs
 *  @{
 */

#ifndef _RTEMS_CLOCKDRV_H
#define _RTEMS_CLOCKDRV_H

#ifdef __cplusplus
extern "C"
{
#endif

   /**
    * @brief number of clock driver ticks
    */
   extern volatile uint64_t Clock_driver_ticks;

   /**
    * @brief major number of the clock driver
    */
   extern rtems_device_major_number rtems_clock_major;

   /**
    * @brief minor number of the clock device
    */
   extern rtems_device_minor_number rtems_clock_minor;

   /**
    * @brief default clock driver entry
    */
#define CLOCK_DRIVER_TABLE_ENTRY \
  { Clock_initialize, NULL, NULL, NULL, NULL, NULL }

   /**
    *  @brief initialize the clock driver
    *
    *  This routine initializes the clock driver.
    *  Return values:
    *  rtems_device_driver status code
    *
    *  @param[in] major clock device major number
    *  @param[in] minor clock device minor number
    *  @param[in] pargp pointer to optional device driver arguments. Not used
    *
    *  @return returns RTEMS_SUCCESSFUL if the operation was successful. Returns
    *  an error in other cases.
    */
   rtems_device_driver Clock_initialize(
                                        rtems_device_major_number major,
                                        rtems_device_minor_number minor,
                                        void *pargp
                                        );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

/**  
 *  @}
 */
