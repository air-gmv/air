/**
 *  @file
 *  rtemsapi.h
 *
 *  @brief contains the support routines for the RTEMS API: Common component
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
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_API Super API
 *  @{
 */

/**
 *  @defgroup RTEMSAPI RTEMS API
 * 
 *  @brief The RTEMS API component inside the SUPER API is responsible for the 
 *  initialization of the RTEMS API
 *  @{
 *
 */

#ifndef _RTEMS_RTEMS_RTEMSAPI_H
#define _RTEMS_RTEMS_RTEMSAPI_H

#include <rtems/config.h>

/**
 *  @brief initialize the RTEMS API component
 *  
 *  This routine initializes the RTEMS API by invoking the initialization
 *  routine for each RTEMS manager with the appropriate parameters
 *  from the configuration_table.
 *
 *  @param[in] configuration_table the configuration table
 */
void _RTEMS_API_Initialize(
                           rtems_configuration_table *configuration_table
                           );

#endif
/* end of include file */

/**  
 *  @}
 */

/**
 *  @}
 */
