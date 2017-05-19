/**
 *  @file
 *  bspstart.c
 *
 *  @brief This set of routines starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before any of these are invoked.
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
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995.
 *  European Space Agency.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  234     | 09/10/2008  | mcoutinho    | IPR 66
 *  2469    | 15/04/2009  | mcoutinho    | IPR 452
 *  3592    | 02/07/2009  | mcoutinho    | IPR 97
 *  3592    | 02/07/2009  | mcoutinho    | IPR 541
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6116    | 18/02/2010  | mcoutinho    | IPR 1818
 *  6145    | 22/02/2010  | mcoutinho    | IPR 42
 *  7253    | 12/04/2010  | mcoutinho    | IPR 2266
 *  9619    | 23/02/2011  | mcoutinho    | IPR 67
 *  9619    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SHARED_BETWEEN_SPARC_BSPS Shared between SPARC BSPs
 *  @{
 */

#include <string.h>
#include <bsp.h>
#include <sharedBSPs.h>
#include <sharedSPARC.h>
#include <rtems/system.h>
#include <rtems/asm.h>
#include <xky.h>

rtems_configuration_table BSP_Configuration;


rtems_cpu_table Cpu_table;

/* temporary stack*/
static uint8_t __ISR_stack[CONFIGURE_INTERRUPT_STACK_MEMORY];


void *_partition_ISR_stack = &__ISR_stack[CONFIGURE_INTERRUPT_STACK_MEMORY-1];

#ifdef LEON2


/**
 *  @brief read the data cache configuration register to determine if
 *  bus snooping is available
 *
 *  This is needed for some drivers so
 *  that they can select the most efficient copy routines.
 *
 *  @return returns the set snooping bit of the data cache configuration
 *  register
 */
RTEMS_INLINE_ROUTINE int set_snooping(void)
{
    /* read cache control register */
    unsigned int tmp = *(unsigned int *) 0x80000014;

    /* return the set snooping bit (bit 23) */
    return (( tmp >> 23 ) & 1 );
}

#endif


void bsp_pretasking_hook(void)
{
    /* initialize the interrupt handlers */
    bsp_spurious_initialize();
}


void bsp_start(void)
{
    /* address of the start of the worspace */
    unsigned char *work_space_start;

    /* set pretasking hook */
    Cpu_table.pretasking_hook = bsp_pretasking_hook;

    /* set post driver hook */
    Cpu_table.postdriver_hook = bsp_postdriver_hook;

    /* set the interrupt stack (should be enough) */
    Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;

    /* set the start address of the workspace:
     *   equal to the end of the RAM (equal to rdb_start) minus the
     *   space for the remote debugger
     *   the BSP stack
     *   the RTEMS configuration memory (for application tasks, timers, etc) */
    work_space_start = (unsigned char *) rdb_start - RDB_MON_MEM - STACK_SIZE -
        BSP_Configuration.work_space_size;

    /* if workspace is too small */
    if(work_space_start <= (unsigned char *) & end)
    {
        /* interrupt level */
        rtems_interrupt_level level;


        /* get system state */
        System_state_Codes old_state = _System_state_Get();

        /* disable interrupts */
        rtems_interrupt_disable(level);

        /* if the user fatal extension could not handle the error */
        /* set the system to the safe state mode */
        _System_state_Set(SYSTEM_STATE_APP_SAFE);

        /* call the application safe state handler until it returns true */
        while(!app_safe_state_handler(INTERNAL_ERROR_CORE ,
                                      TRUE ,
                                      INTERNAL_ERROR_NOT_ENOUGH_MEMORY));

        /* restore the old state */
        _System_state_Set(old_state);

        /* re-enable interrupts
         * if the program reached here, the problem was somehow solved */
        rtems_interrupt_enable(level);
    }

	/* get the microseconds per tick from the host */
	BSP_Configuration.microseconds_per_tick =
	        (uint32_t)xky_syscall_get_us_per_tick();

    /* set the workspace start */
    BSP_Configuration.work_space_start = work_space_start;
}

/**  
 *  @}
 */
