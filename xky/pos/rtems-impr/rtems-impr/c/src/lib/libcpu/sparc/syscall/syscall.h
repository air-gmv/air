/**
 *  @file
 *  syscall.h
 *
 *  @brief contains emulated system calls using software trap 0.
 *
 *  The following calls are supported:
 *\n
 *    + syscall\n
 *
 *  Also contains the enable and disable interrupt functions
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
 *  9619    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9877 $ | $Date: 2011-03-18 18:39:36 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SPARC_CPUS SPARC CPUs
 *  @{
 */

#ifndef __SPARC_SYSCALL_h
#define __SPARC_SYSCALL_h

/**
 * @brief determine the value to announce to make a system exit to the SPARC CPUs component
 */
#define SYS_exit        1

/**
 * @brief determine the value to announce to disable interrupts to the SPARC CPUs component
 */
#define SYS_irqdis      2

/**
 * @brief determine the value to announce to enable interrupts to the SPARC CPUs component
 */
#define SYS_irqen       3

/**
 * @brief determine the value of the interrupt level (PIL)
 */
#define SYS_getintlevel 4

#ifndef ASM

/**
 *  @brief enable/disable interrupts or shutdown the system through a software 
 *  trap 0
 *
 *  Calls a software trap. The CPU register g1 indicates the operation to perform:
 *  With g1 = 2, the interrupts will be disabled.\n
 *  With g1 = 3, the interrupts will be enabled.\n
 *  With another g1 value, the system will enter an infinite loop\n
 *  When the function leaves, g0 will be reset to 0.\n
 *
 *  This function should only be called by the sparc_enable_interrupts and
 *  sparc_disable_interrupts functions.
 *  Inputs:\n
 *    CPU register l0 contains the psr\n
 *    CPU register l1 contains the pc\n
 *    CPU register l2 contains the npc\n
 *    CPU register g1 contains the system call id.\n
 */
void syscall(void);

#endif

#endif

/**
 *  @}
 */
