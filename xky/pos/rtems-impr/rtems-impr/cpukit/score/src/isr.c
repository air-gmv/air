/**
 *  @file
 *  isr.c
 *
 *  @brief initialize the ISR Handler
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
 *  4434    | 21/09/2009  | mcoutinho    | IPR 751
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  6325    | 01/03/2010  | mcoutinho    | IPR 1931
 *  6356    | 02/03/2010  | mcoutinho    | IPR 1935
 *  8184    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SUPER_CORE Super Core
 *  @{
 */

/**
 *  @addtogroup ScoreISR ISR Handler
 *  @{
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/stack.h>
#include <rtems/score/interr.h>
#include <rtems/score/wkspace.h>


volatile uint32_t _ISR_Nest_level;


ISR_Handler_entry *_ISR_Vector_table;


void _ISR_Handler_initialization(void)
{
    /* initialize the interrupt nesting level to 0 (not inside an interrupt) */
    _ISR_Nest_level = 0;

    /* initialize the ISR Vector Table for RTEMS internal usage 
     * @todo: this shouldnt have to be dynamically allocated */
    _ISR_Vector_table = _Workspace_Allocate_or_fatal_error(sizeof (ISR_Handler_entry ) *
                                                           ISR_NUMBER_OF_VECTORS);

    /* initialize the CPU vectors */
    _CPU_Initialize_vectors();

#if ( CPU_ALLOCATE_INTERRUPT_STACK == TRUE )

    /* if the interrupt stack size is too little */
    if(_CPU_Table.interrupt_stack_size < STACK_MINIMUM_SIZE)
    {
        /* raise internal error */
        _Internal_error_Occurred(INTERNAL_ERROR_CORE ,
                                 TRUE ,
                                 INTERNAL_ERROR_INTERRUPT_STACK_TOO_SMALL);
    }

    /* otherwise, allocate space for the interrupt stack */
    _CPU_Interrupt_stack_low = _Workspace_Allocate_or_fatal_error(_CPU_Table.interrupt_stack_size);

    /* determine the high address of the interrupt stack */
    _CPU_Interrupt_stack_high = _Addresses_Add_offset(_CPU_Interrupt_stack_low ,
                                                      _CPU_Table.interrupt_stack_size);

#endif

    /* if CPU has dedicated hardware interrupt stack */
#if ( CPU_HAS_HARDWARE_INTERRUPT_STACK == TRUE )

    /* then install interrupt stack */
    _CPU_Install_interrupt_stack();
#endif

}

/**  
 *  @}
 */

/**
 *  @}
 */
