/**
 *  @file
 *  init.h
 *
 *  @brief all the constants and structures associated with the Initialization
 *  Manager.  This manager is responsible for initializing RTEMS, creating and
 *  starting all configured initialization tasks, invoking the initialization
 *  routine for each user-supplied device driver, and initializing the optional
 *  multiprocessor layer.
 *
 *  This manager provides directives to:
 *\n
 *     + initialize the RTEMS executive\n
 *     + shutdown the RTEMS executive\n
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
 *  593     | 17/11/2008  | mcoutinho    | IPR 64
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  9617    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_API Super API
 *  @{
 */

/**
 *  @defgroup SUPER_API_INITIALIZATION Initialization Manager
 *
 *  @brief The Initialization Manager component initializes and shutsdown
 *  the RTEMS API, Super API and Super Core structures
 *  @{
 */

#ifndef _RTEMS_INIT_H
#define _RTEMS_INIT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <rtems/rtems/types.h>
#include <rtems/config.h>
#include <rtems/rtems/intr.h>

#if defined(RTEMS_MULTIPROCESSING)

   /**
    *  @brief Multiprocessing Configuration
    *  Table.  This table is used in a single processor system.
    */
   extern const rtems_multiprocessing_table
   _Initialization_Default_multiprocessing_table;
#endif

   /**
    *  @brief initialize RTEMS without starting multitasking
    *
    *  This routine initializes the RTEMS Super Core, RTEMS API and Super API.
    *  It also initializes the BSP and device drivers.
    *
    *  @param[in] configuration_table the system configuration table
    *  @param[in] cpu_table  the system CPU table
    *
    *  @return returns the interrupt level to set so that interrutps will become
    *  enabled
    */
   rtems_interrupt_level rtems_initialize_executive_early(
                                                          rtems_configuration_table *configuration_table ,
                                                          rtems_cpu_table *cpu_table
                                                          );

   /**
    *  @brief finish initializing RTEMS and start multitasking
    *
    *  This routine sets the system state to "UP" and starts multitasking
    *
    *  @param[in] bsp_level interrupt level to set
    */
   void rtems_initialize_executive_late(
                                        rtems_interrupt_level bsp_level
                                        );

   /**
    *  @brief shutsdown RTEMS
    *
    *  The invocation of this directive results in the RTEMS environment being
    *  shutdown and multitasking halted.  From the application's perspective,
    *  invocation of this directive results in the rtems_initialize_executive
    *  directive exitting to the startup code which invoked it.
    *
    *  @param[in] result the application result
    */
   void rtems_shutdown_executive(
                                 uint32_t result
                                 );

   /**
    * @brief check the configuration parameters correctness
    *
    * This directive checks the configuration values and sees if they are invalid.
    * If a error is found, an internal error will be raised.
    *
    * @param[in] configuration_table the configuration table
    **/
   void rtems_check_configuration_parameters(rtems_configuration_table *configuration_table);
   
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

