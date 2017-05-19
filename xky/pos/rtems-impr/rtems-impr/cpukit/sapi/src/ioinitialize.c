/**
 *  @file
 *  ioinitialize.c
 *
 *  @brief initialize a device driver
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
 *  5247    | 30/10/2009  | mcoutinho    | IPR 828
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  7132    | 09/04/2010  | mcoutinho    | IPR 1931
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
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


rtems_status_code rtems_io_initialize(
                                      rtems_device_major_number major ,
                                      rtems_device_minor_number minor ,
                                      void *argument
                                      )
{
    /* function to call */
    rtems_device_driver_entry callout;

    /* return value (default is success if there is no callout function) */
    rtems_status_code status = RTEMS_SUCCESSFUL;


    /* check the major number */
    if(major >= _IO_Number_of_drivers)
    {
        /* return invalid number */
        status = RTEMS_INVALID_NUMBER;
    }
    else
    {
        /* get the io intialization entry for the device driver */
        callout = _IO_Driver_address_table[major].initialization_entry;

        /* if the entry is not null */
        if(callout != NULL)
        {
            /* then call it */
            status = callout(major , minor , argument);
        }
    }

    /* return the status */
    return status;
}

/**  
 *  @}
 */

/**  
 *  @}
 */

