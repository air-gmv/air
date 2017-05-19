/** 
 *  @file
 *  setvec.c
 *
 *  @brief Contains routines to install a interrupt routine and idle body
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
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995.
 *  European Space Agency.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  4341    | 15/09/2009  | mcoutinho    | IPR 606
 *  4413    | 21/09/2009  | mcoutinho    | IPR 607
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  8183    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SPARC_ERC32_BSP SPARC ERC32 BSP
 *  @{
 */

#include <bsp.h>
#include <rtems/score/cpu.h>


rtems_isr_entry set_vector(
                           rtems_isr_entry handler ,
                           rtems_vector_number vector
                           )
{
    /* previous interrupt handler */
    rtems_isr_entry previous_isr;

    /* vector number */
    uint32_t real_trap;

    /* trap source */
    uint32_t source;


    /* catch the interrupt */
    rtems_interrupt_catch(handler , vector , &previous_isr);

    /* determine the real trap number */
    real_trap = SPARC_REAL_TRAP_NUMBER(vector);

    /* if it is an external interrupt trap */
    if(ERC32_Is_MEC_Trap(real_trap))
    {
        /* then get the source number */
        source = ERC32_TRAP_SOURCE(real_trap);

        /* clear the interrupt */
        ERC32_Clear_interrupt(source);

        /* and unmask the interrupt */
        ERC32_Unmask_interrupt(source);
    }

    /* return the previous ISR */
    return previous_isr;
}


#if (CPU_PROVIDES_IDLE_THREAD_BODY == TRUE)


void _CPU_Thread_Idle_body(void)
{
    /* loop forever */
    while(1)
    {
        /* write something to the ERC 32 power down register */
        /* value is irrelevant */
        ERC32_MEC.Power_Down = 0;
    }
}
#endif

/**  
 *  @}
 */
