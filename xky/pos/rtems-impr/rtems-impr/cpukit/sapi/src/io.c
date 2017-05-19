/**
 *  @file
 *  io.c
 *
 *  @brief initialize device driver subsystem
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  3510    | 19/06/2009  | mcoutinho    | IPR 527
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6116    | 18/02/2010  | mcoutinho    | IPR 1818
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  7131    | 09/04/2010  | mcoutinho    | IPR 1931
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_API Super API
 *  @{
 */

/**
 *  @addtogroup SUPER_API_IO Input/Output Manager
 *  @{
 */

#include <rtems/system.h>
#include <rtems/io.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>



uint32_t _IO_Number_of_drivers;


rtems_driver_address_table *_IO_Driver_address_table;


void _IO_Manager_initialization(
                                rtems_driver_address_table *driver_table ,
                                uint32_t drivers_in_table ,
                                uint32_t number_of_drivers
                                )
{
    /* If the user claims there are less drivers than are actually in
     * the table, then let's just go with the table's count */
    if(number_of_drivers <= drivers_in_table)
    {
        /* update the number of device drivers */
        number_of_drivers = drivers_in_table;
    }

    /* copy the data to the specific io driver table */

    /* copy the driver table to the IO Manager */
    _IO_Driver_address_table = driver_table;

    /* set the number of device drivers of the IO Manager */
    _IO_Number_of_drivers = number_of_drivers;

    /* return */
    return;
}


void _IO_Initialize_all_drivers(void)
{
    /* iterator through the device drivers */
    rtems_device_major_number major;

    /* for every driver in the system */
    for(major = 0; major < _IO_Number_of_drivers; major++)
    {
        /* initialize it */
        (void) rtems_io_initialize(major , 0 , NULL);
    }
}

/**  
 *  @}
 */

/**  
 *  @}
 */

