/**
 *  @file
 *  spurious.c
 *
 *  @brief LEON2 Spurious Trap Handler
 *
 *  This is just enough of a trap handler to let us know what
 *  the likely source of the trap was.
 *
 *  Developed as part of the port of RTEMS to the LEON implementation
 *  of the SPARC by On-Line Applications Research Corporation (OAR)
 *  under contract to the European Space Agency (ESA).
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1995. European Space Agency.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  234     | 09/10/2008  | mcoutinho    | IPR 66
 *  2466    | 15/04/2009  | mcoutinho    | IPR 452
 *  4341    | 15/09/2009  | mcoutinho    | IPR 606
 *  4586    | 30/09/2009  | mcoutinho    | IPR 605
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5513    | 11/11/2009  | mcoutinho    | IPR 866
 *  8308    | 21/06/2010  | mcoutinho    | IPR 451
 *  9872    | 18/03/2011  | aconstantino | SPR 2819
 *  $Rev: 9929 $ | $Date: 2011-03-23 12:02:26 +0000 (Wed, 23 Mar 2011) $| $Author: sfaustino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SPARC_LEON2_BSP SPARC LEON2 BSP
 *  @{
 */



#include <bsp.h>

#include <rtems/score/interr.h>
#include <sharedBSPs.h>


rtems_isr bsp_spurious_handler(
                               rtems_vector_number trap
                               )
{
    /* call internal error with :
     *   interrupt not handled
     *   internal set to true
     *   the interrupt vector number */
    _Internal_error_Occurred(INTERNAL_ERROR_INTERRUPTION_NOT_HANDLED ,
                             TRUE ,
                             SPARC_REAL_TRAP_NUMBER(trap));
}


void bsp_spurious_initialize()
{
    /* old ISR handler */
    proc_ptr old_isr_handler;

    /* iterator trap number */
    uint32_t trap;

    /* interrupt level */
    uint32_t level;

    /* interrupt mask */
    uint32_t mask;


    /* disable interrutps */
    level = sparc_disable_interrupts();

    /* save the interrupt mask */
    mask = LEON_REG.Interrupt_Mask;

    /* iterate through all the SPARC traps */
    for(trap = 0; trap < 256; trap++)
    {
        /* in case of an external interrupt */
        if(( trap >= 0x11 ) && ( trap <= 0x1f ))
        {
            /* simply catch the interrupt and not unmask it */
            /*_CPU_ISR_install_vector(trap ,
                                    bsp_spurious_handler ,
                                    &old_isr_handler); */

            /* and move on to the next trap */
            continue;
        }

        /* skip window overflow, underflow as well as software
         * trap 0 which we will use as a shutdown. Also avoid trap 0x70 - 0x7f
         * which cannot happen and where some of the space is used to pass
         * paramaters to the program */
        if(( trap == 5 || trap == 6 ) ||
           ( ( trap >= 0x70 ) && ( trap <= 0x80 ) ))
        {
            /* skip this vector */
            continue;
        }

        /* set the default ISR handler to bsp_spurious_handler */
        /* set_vector(bsp_spurious_handler ,
                   SPARC_SYNCHRONOUS_TRAP(trap)); */
    }

    /* restore previous interrupt mask */
    LEON_REG.Interrupt_Mask = mask;

    /* re-enable interrupts */
    sparc_enable_interrupts(level);
}

/**  
 *  @}
 */
