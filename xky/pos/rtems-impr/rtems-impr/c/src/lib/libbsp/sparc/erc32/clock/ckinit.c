/**
 *  @file
 *  ckinit.c
 *
 *  @brief Clock Tick Device Driver
 *
 *  This routine initializes the Real Time Clock Counter Timer which is
 *  part of the MEC on the ERC32 CPU.
 *
 *  The tick frequency is directly programmed to the configured number of
 *  microseconds per tick.
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
 *  4341    | 15/09/2009  | mcoutinho    | IPR 606
 *  4502    | 24/09/2009  | mcoutinho    | IPR 381
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5448    | 06/11/2009  | mcoutinho    | IPR 862
 *  6524    | 09/03/2010  | mcoutinho    | IPR 1944
 *  8183    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9877 $ | $Date: 2011-03-18 18:39:36 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SPARC_ERC32_BSP SPARC ERC32 BSP
 *  @{
 */

#include <bsp.h>
#include <sharedBSPs.h>

/**
 *  @brief The Real Time Clock Counter Timer uses this trap type.
 */
#define CLOCK_VECTOR ERC32_TRAP_TYPE( ERC32_INTERRUPT_REAL_TIME_CLOCK )


void Clock_driver_support_install_isr(rtems_isr_entry new_isr ,
                                      rtems_isr_entry *old_isr)
{
    /* set the interrupt vector for the clock interrupt */
    *old_isr = set_vector(new_isr , CLOCK_VECTOR);
}


void Clock_driver_support_initialize_hardware()
{
    /* the scalar is equal to the clock speed (e.g 14 for 14 MHz)
     * for the scalar to decrease the timer counter each microsecond */
    ERC32_MEC.Real_Time_Clock_Scalar = CLOCK_SPEED - 1;

    /* the counter is equal to the number of microseconds per clock tick */
    ERC32_MEC.Real_Time_Clock_Counter = BSP_Configuration.microseconds_per_tick - 1;

    /* set the erc32 RTC control register to:
     *   enable the counter
     *   load the scalar
     *   load the counter values */
    ERC32_MEC_Set_Real_Time_Clock_Timer_Control(ERC32_MEC_TIMER_COUNTER_ENABLE_COUNTING |
                                                ERC32_MEC_TIMER_COUNTER_LOAD_SCALER |
                                                ERC32_MEC_TIMER_COUNTER_LOAD_COUNTER);

    /* start counting and reload the counter when it reaches zero */
    ERC32_MEC_Set_Real_Time_Clock_Timer_Control(ERC32_MEC_TIMER_COUNTER_ENABLE_COUNTING |
                                                ERC32_MEC_TIMER_COUNTER_RELOAD_AT_ZERO);
}


uint32_t Clock_driver_nanoseconds_since_last_tick(void)
{
    /* number of timer ticks (not clock ticks) */
    uint32_t clicks;

    /* read the timer ticks */
    clicks = ERC32_MEC.Real_Time_Clock_Counter;

    /* Down counter */
    clicks = (uint32_t) ( BSP_Configuration.microseconds_per_tick - clicks );

    /* if an interrupt occurred while interrupts were disabled and the clicks is
     * too small means that a clock interrupt occurred BEFORE the clicks were read */
    if(ERC32_Is_interrupt_pending(ERC32_INTERRUPT_REAL_TIME_CLOCK) != 0 &&
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
