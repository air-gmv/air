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
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  626     | 17/11/2008  | mcoutinho    | IPR 64
 *  4341    | 15/09/2009  | mcoutinho    | IPR 606
 *  4503    | 24/09/2009  | mcoutinho    | IPR 381
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5475    | 09/11/2009  | mcoutinho    | IPR 862
 *  6524    | 09/03/2010  | mcoutinho    | IPR 1944
 *  8578    | 21/07/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SPARC_LEON3_BSP SPARC LEON3 BSP
 *  @{
 */

#include <bsp.h>
#include <leon.h>
#include <sharedBSPs.h>

#if SIMSPARC_FAST_IDLE==1
    #define CLOCK_DRIVER_USE_FAST_IDLE
#endif


#if defined(RTEMS_MULTIPROCESSING)
    #define LEON3_CLOCK_INDEX \
  (Configuration.User_multiprocessing_table ? LEON3_Cpu_Index : 0)
#else
    #define LEON3_CLOCK_INDEX 0
#endif

volatile LEON3_Timer_Regs_Map *LEON3_Timer_Regs = 0;

int _leon3_clkirq;

/**
 *  @brief Real Time Clock Counter Timer trap type.
 */
#define CLOCK_VECTOR LEON_TRAP_TYPE( _leon3_clkirq )


void Clock_driver_support_install_isr(rtems_isr_entry new_isr ,
                                      rtems_isr_entry *old_isr)
{
    /* install the new clock isr and get old one */
    *old_isr = set_vector(new_isr , CLOCK_VECTOR);
}


void Clock_driver_support_initialize_hardware()
{
    /* set the timer reload value to the number of microseconds per clock tick
     * (minus 1) */
    LEON3_Timer_Regs->timer[LEON3_CLOCK_INDEX].reload =
        BSP_Configuration.microseconds_per_tick - 1;

    /* set the leon3 timer control register to:
     *   enable counting
     *   reload at zero
     *   load counter with the reload value
     *   enable clock interrupt generation */
    LEON3_Timer_Regs->timer[LEON3_CLOCK_INDEX].conf =
        LEON3_GPTIMER_EN | LEON3_GPTIMER_RL |
        LEON3_GPTIMER_LD | LEON3_GPTIMER_IRQEN;
}


uint32_t Clock_driver_nanoseconds_since_last_tick(void)
{
    /* number of timer ticks (not clock ticks) */
    uint32_t clicks;


    /* check if leon3 timer register have been initialized */
    if(!LEON3_Timer_Regs)
    {
        /* return 0 if the timer register have not been initialized */
        return 0;
    }

    /* read the timer ticks */
    clicks = LEON3_Timer_Regs->timer[0].value;

    /* Down counter */
    clicks = ( BSP_Configuration.microseconds_per_tick - clicks );

    /* if an interrupt occurred while interrupts were disabled and the clicks is
     * too little means that a clock interrupt occurred BEFORE the clicks were read */
    if(( LEON3_IrqCtrl_Regs->ipend & ( 1 << ( _leon3_clkirq ) ) ) &&
       clicks < BSP_Configuration.microseconds_per_tick / 2)
    {
        /* increase the number of clicks by one clock tick */
        clicks += BSP_Configuration.microseconds_per_tick;
    }

    /* return the number of microseconds times 1000 (give in nanoseconds) */
    return clicks * 1000;
}

/**
 *  @}
 */
