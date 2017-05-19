/**
 *  @file
 *  sharedBSPs.h
 *
 *  @brief This file defines some prototypes of the Shared between BSPs
 *  component
 *
 *  COPYRIGHT (c) 2010.
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5475    | 09/11/2009  | mcoutinho    | IPR 862
 *  6145    | 22/02/2010  | mcoutinho    | IPR 42
 *  9619    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @defgroup SHARED_BETWEEN_BSPS Shared between BSPs
 *
 *  @brief The Shared Between BSPs component defines the code shared among all
 *  the BSPs.
 *  @{
 */

#ifndef __SHARED_BSPS_h
#define __SHARED_BSPS_h

#include <bsp.h>

/**
 *  @brief where to put the workspace in case remote debugger is present
 */
extern uint32_t rdb_start;

/**
 *  @brief the CPU table configuration for each BSP
 **/
extern rtems_cpu_table Cpu_table;

/**
 *  @brief system configuration
 **/
extern rtems_configuration_table Configuration;

/**
 *  @brief copy of the configuration for each BSP
 **/
extern rtems_configuration_table BSP_Configuration;

/**
 * @brief clock speed
 **/
extern int CLOCK_SPEED;

/**
 * @brief initialize the BSP and starts RTEMS core and application
 *
 * First C function to be invoked durint the system operation. Invokes the 
 * bsp initialization and RTEMS initialization
 *
 * @param[in] argc not used
 * @param[in] argv not used
 * @param[in] envp not used
 *
 * @return returns 0
 **/
int boot_card(int argc , char **argv , char **envp);

/**
 *  @brief initialize all BSP necessary operations after driver initialization
 */
void bsp_postdriver_hook(void);

/**
 *  @brief initialize all BSP necessary operations before tasking is started
 *
 *  BSP pretasking hook.  Called just before drivers are initialized.
 *  Used to setup libc and install any BSP extensions.
 */
void bsp_pretasking_hook(void);

/**
 *  @brief perform the bulk of the system initialization
 */
void bsp_start(void);

/**
 *  @brief default ISR for all interrupts (except for the system interrupts)
 * 
 *  This function invokes the _Internal_error_Occurred function with the trap
 *  type
 *
 *  @param[in] trap the trap number
 */
rtems_isr bsp_spurious_handler(
                               rtems_vector_number trap
                               );

/**
 *  @brief install the spurious handler for most traps.
 *
 *  Note that set_vector() will unmask the corresponding asynchronous interrupt,
 *  so the initial interrupt mask is restored after the handlers are installed.
 */
void bsp_spurious_initialize(void);

/**
 *  @brief install an interrupt vector
 *
 *  @param[in] handler interrupt handler entry point
 *  @param[in] vector  vector number
 *
 *  @return address of previous interrupt handler
 */
rtems_isr_entry set_vector(/* returns old vector */
                           rtems_isr_entry handler , /* isr routine        */
                           rtems_vector_number vector /* vector number      */
                           );

#ifdef BSP_HAS_CLOCK_DRIVER

/**
 *  @brief install clock ISR
 *
 *  @param[in] clock_isr the clock ISR
 *  @param[out] Old_ticker old clock ISR
 */
void Clock_driver_support_install_isr(rtems_isr_entry clock_isr , rtems_isr_entry *Old_ticker);


#if (BSP_CLOCK_DRIVER_NEEDS_SUPPORT_AT_CLOCK_TICK == TRUE)

/**
 * @brief support for clock driver at each clock tick
 */
void Clock_driver_support_at_tick();

#endif

/**
 *  @brief initialize clock driver
 */
void Clock_driver_support_initialize_hardware();

/**
 *  @brief get the number of nanoseconds since last clock tick
 *
 *  @return returns the number of nanoseconds since the last clock tick
 */
uint32_t Clock_driver_nanoseconds_since_last_tick(void);

#endif

#endif

/**
 *  @}
 */
