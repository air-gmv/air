/**
 *
 *  @file
 *  bootcard.c
 *
 *  @brief This is the C entry point for ALL RTEMS BSPs.
 * 
 *  It is invoked from the assembly language initialization file usually called
 *  start.S.  It provides the framework for the BSP initialization
 *  sequence.  The basic flow of initialization is:
 *
 *  + start.S: basic CPU setup (stack, zero BSS)\n
 *    + boot_card\n
 *      + bspstart.c: bsp_start - more advanced initialization\n
 *      + rtems_initialize_executive_early\n
 *        + all device drivers\n
 *      + rtems_initialize_executive_late\n
 *        + 1st task executes C++ global constructors\n
 *        .... appplication runs ...\n
 *        + exit\n
 *     + back to here eventually\n
 *     + bspclean.c: bsp_cleanup\n
 *
 *  This style of initialization insures that the C++ global
 *  constructors are executed after RTEMS is initialized.
 *
 *  Thanks to Chris Johns <cjohns@plessey.com.au> for this idea.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  552     | 17/11/2008  | mcoutinho    | IPR 65
 *  3537    | 29/06/2009  | mcoutinho    | IPR 536
 *  3705    | 09/07/2009  | jsousa       | IPR 153
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8183    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SHARED_BETWEEN_BSPS Shared between BSPs
 *  @{
 */

#include <xky.h>
#include <bsp.h>
#include <sharedBSPs.h>

/**
 * @brief RTEMS API configuration
 **/
rtems_api_configuration_table BSP_RTEMS_Configuration;

/**
 * @brief bsp interrupt level
 **/
rtems_interrupt_level bsp_isr_level;

/**
 * @brief trap table of the system
 */
void *trap_table[CPU_INTERRUPT_NUMBER_OF_VECTORS];

int boot_card(int argc , char **argv , char **envp)
{
    /* clear trap table */
    uint32_t i;
    for (i = 0; i < CPU_INTERRUPT_NUMBER_OF_VECTORS; ++i) {

        trap_table[i] = NULL;
    }

    /* set TBR */
    xky_sparc_set_tbr(&trap_table[0]);

	/* enable traps*/
	xky_sparc_enable_traps();

    /* set default values for the CPU Table fields all ports must have.
     * These values can be overridden in bsp_start() but they are
     * right most of the time */

    /* by default, there is no pretasking hook */
    Cpu_table.pretasking_hook = NULL;

    /* by default, there is no predriving hook */
    Cpu_table.predriver_hook = NULL;

    /* by default, there is no postdriver hook */
    Cpu_table.postdriver_hook = NULL;

    /* by default, there is no idle task */
    Cpu_table.idle_task = NULL;

    /* by default, the system does not need to initialize the workspace to 0 */
    Cpu_table.do_zero_of_workspace = FALSE;

    /* by default, the interrupt stack size is the minimum size */
    Cpu_table.interrupt_stack_size = RTEMS_MINIMUM_STACK_SIZE;

#if defined(RTEMS_MULTIPROCESSING)

    /* by default, there is no multiprocessor server stack */
    Cpu_table.extra_mpci_receive_server_stack = 0;

#endif

    /* by default, there is stack allocate hook */
    Cpu_table.stack_allocate_hook = NULL;

    /* by default, there is no stack free hook */
    Cpu_table.stack_free_hook = NULL;

    /* check if the configuratio parameters are ok */
    rtems_check_configuration_parameters(&Configuration);

    /* Copy the configuration table so we and the BSP wants to change it. */
    BSP_Configuration = Configuration;

    /* copy to the BSP the API configuration */
    BSP_RTEMS_Configuration = *Configuration.RTEMS_api_configuration;

    /* copy the API configuration to the BSP configuration */
    BSP_Configuration.RTEMS_api_configuration = &BSP_RTEMS_Configuration;

    /* Invoke Board Support Package initialization routine */
    bsp_start();

    /* Initialize RTEMS but do NOT start multitasking */
    bsp_isr_level = rtems_initialize_executive_early(&BSP_Configuration ,
                                                     &Cpu_table);

    /* start multitasking here */
    rtems_initialize_executive_late(bsp_isr_level);

    /* APPLICATION RUNS HERE!!!  When it shuts down, we return!!! */


    /* Now return to the start code */
    return 0;
}

/**
 *  @}
 */
