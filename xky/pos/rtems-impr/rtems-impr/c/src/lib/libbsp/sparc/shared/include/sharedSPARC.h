/**
 *  @file
 *  sharedSPARC.h
 *
 *  @brief shared between the SPARC BSPs prototypes
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
 *  8183    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9877 $ | $Date: 2011-03-18 18:39:36 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @defgroup SHARED_BETWEEN_SPARC_BSPS Shared between SPARC BSPs
 *
 *  @brief The Shared Betwen SPARC BSPs component contains the code common to all SPARC
 *  BSPs
 *  @{
 */

#ifndef __SHARED_SPARC_BSPS_h
#define __SHARED_SPARC_BSPS_h

/**
 * @brief default interrupt stack
 */
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (16 * 1024)

/**
 * @brief start the system. Boot entry point
 *
 * This function corresponds to entry point of the executable.
 * It is the first code to be executed on board startup
 **/
void start(void);


/**
 * @brief trap table of the system
 *
 * The trap table has to be the first code in a boot PROM
 **/
//void trap_table(void);

/**
 * @brief reset the hardware
 *
 * This function is the trap for the hard reset. It is the first code invoked
 * by the start function.
 **/
void hard_reset(void);

/**
 *  @brief shutdown the system
 *
 *  The app has "exited" (rtems_shutdown_executive returns control to main)
 */
void bsp_cleanup(void);


/*
 *  Information placed in the linkcmds file.
 */

/**
 * @brief start of RAM memory
 **/
extern int RAM_START;

/**
 * @brief end of RAM memory
 **/
extern int RAM_END;

/**
 * @brief RAM memory size
 **/
extern int RAM_SIZE;

/**
 * @brief start of PROM memory
 **/
extern int PROM_START;

/**
 * @brief end of PROM memory
 **/
extern int PROM_END;

/**
 * @brief size of PROM memory
 **/
extern int PROM_SIZE;

/**
 * @brief last address in the program
 **/
extern int end;

#endif

/**
 *  @}
 */


