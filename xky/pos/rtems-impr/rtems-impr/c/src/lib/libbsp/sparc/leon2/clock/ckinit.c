/**
 *  @file
 *  ckinit.c
 *
 *  @brief Clock Tick Device Driver
 *
 *  This routine initializes LEON timer 1 which used for the clock tick.
 *
 *  The tick frequency is directly programmed to the configured number of
 *  microseconds per tick.
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON BSP
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  4341    | 15/09/2009  | mcoutinho    | IPR 606
 *  4503    | 24/09/2009  | mcoutinho    | IPR 381
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5448    | 06/11/2009  | mcoutinho    | IPR 862
 *  6524    | 09/03/2010  | mcoutinho    | IPR 1944
 *  8183    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9877 $ | $Date: 2011-03-18 18:39:36 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/



/**
 *  @addtogroup SPARC_LEON2_BSP SPARC LEON2 BSP
 *  @{
 */


#include <bsp.h>
#include <rtems/config.h>
#include <sharedBSPs.h>

/**
 *  @brief Real Time Clock Counter Timer trap type.
 */
#define CLOCK_VECTOR LEON_TRAP_TYPE( LEON_INTERRUPT_TIMER1 )


void Clock_driver_support_install_isr(rtems_isr_entry new_isr ,
                                      rtems_isr_entry *old_isr)
{
    /* set the interrupt vector for the clock interrupt */
    *old_isr = set_vector(new_isr , CLOCK_VECTOR);
}


void Clock_driver_support_initialize_hardware()
{
    /* set the timer reload value to the number of microseconds per clock tick
     * (minus 1) */
    LEON_REG.Timer_Reload_1 = BSP_Configuration.microseconds_per_tick - 1;

    /* set the leon2 timer control register to:
     *   enable counting
     *   reload at zero
     *   load counter with reload value */
    LEON_REG.Timer_Control_1 = ( LEON_REG_TIMER_COUNTER_ENABLE_COUNTING |
                                 LEON_REG_TIMER_COUNTER_RELOAD_AT_ZERO |
                                 LEON_REG_TIMER_COUNTER_LOAD_COUNTER );
}


uint32_t Clock_driver_nanoseconds_since_last_tick(void)
{
    /* number of timer ticks (not clock ticks) */
    uint32_t clicks;


    /* read the timer ticks */
    clicks = LEON_REG.Timer_Counter_1;

    /* Down counter */
    clicks = (uint32_t) ( BSP_Configuration.microseconds_per_tick - clicks );

    /* if an interrupt occurred while interrupts were disabled and the clicks is too little */
    /* means that a clock interrupt occurred BEFORE the clicks were read */
    if(LEON_Is_interrupt_pending(LEON_INTERRUPT_TIMER1) &&
       clicks < BSP_Configuration.microseconds_per_tick / 2U)
    {
        /* increase the number of clicks by one clock tick */
        clicks += BSP_Configuration.microseconds_per_tick;
    }

    /* return the number of microseconds times 1000 (give in nanoseconds) */
    return clicks * 1000U;
}

/**
 *  @}
 */
