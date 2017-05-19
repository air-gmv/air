/**
 *  @file
 *  clockdrv_shell.c
 *
 *  @brief Clock Tick Device Driver Shell
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
 *  4335    | 15/09/2009  | mcoutinho    | IPR 615
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  5448    | 06/11/2009  | mcoutinho    | IPR 862
 *  6116    | 18/02/2010  | mcoutinho    | IPR 67
 *  6116    | 18/02/2010  | mcoutinho    | IPR 1818
 *  7253    | 12/04/2010  | mcoutinho    | IPR 2266
 *  8183    | 15/06/2010  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SHARED_BETWEEN_BSPS Shared between BSPs
 *  @{
 */

#include <sharedBSPs.h>
#include <rtems/system.h>
#include <xky.h>

#if defined(CLOCK_DRIVER_USE_FAST_IDLE) && defined(CLOCK_DRIVER_ISRS_PER_TICK)
    #error "clockdrv_shell.c: fast idle and N ISRs per tick is not supported"
#endif


volatile uint64_t Clock_driver_ticks;


rtems_device_major_number rtems_clock_major;


rtems_device_minor_number rtems_clock_minor;


#ifdef CLOCK_DRIVER_ISRS_PER_TICK
/**
 * @brief number of clock ISRs until next clock tick
 */
volatile uint32_t Clock_driver_isrs;
#endif

/**
 * @brief  Clock ticks since initialization
 */
volatile uint64_t Clock_driver_ticks;

/**
 *  @brief ISR formerly installed.
 */
rtems_isr_entry Old_ticker;


/**
 *  @brief Major number.
 */
rtems_device_major_number rtems_clock_major = ~0;

/**
 *  @brief Minor number
 */
rtems_device_minor_number rtems_clock_minor;


/**
 *  @brief clock ISR entry point
 *  
 *  This is the clock tick interrupt handler.
 *  Return values:      NONE
 *  
 *  @param[in] vector vector number
 */
rtems_isr Clock_isr(rtems_vector_number vector)
{

#ifdef TEST_POS_CLOCK_TICK_PERF
    /* end: clock tick measurement  */
    RVS_Ipoint((0 << 6) | (0 << 4) | (0 << 1) | 1);
#endif

    /* get elapsed ticks */
    uint64_t elapsed = xky_syscall_get_elapsed_ticks();
    uint64_t dt = elapsed - Clock_driver_ticks;

    /* accurate count of clock ISRs */
    Clock_driver_ticks += dt;

#ifdef CLOCK_DRIVER_USE_FAST_IDLE

    /* do fast idle, i.e., instead of being in idle, announce the passage of
     * clock ticks to speed up the time */

    /* loop until there is a ready task (other than the idle thread) */
    do
    {
        /* announce the passage of a clock tick */
        rtems_clock_tick();

        /* while the thread executing is the idle thread and the
         * heir thread is the idle thread */
    } while(_Thread_Executing == _Thread_Idle &&
            _Thread_Heir == _Thread_Executing);

    #if (BSP_CLOCK_DRIVER_NEEDS_SUPPORT_AT_CLOCK_TICK == TRUE)

    /* call clock driver support  */
    Clock_driver_support_at_tick();

    #endif

    /* return */
    return;

#else

    /* do the hardware specific per-tick action.
     * the counter/timer may or may not be set to automatically reload */

    #if (BSP_CLOCK_DRIVER_NEEDS_SUPPORT_AT_CLOCK_TICK == TRUE)

    /* call clock driver support  */
    Clock_driver_support_at_tick();

    #endif

    #ifdef CLOCK_DRIVER_ISRS_PER_TICK

    /* The driver is multiple ISRs per clock tick.
     * That is, a clock tick requires multiple clock ISRs to fire */

    /* check that the number of clock ISR needed for a clock tick is 0 */
    if(!Clock_driver_isrs)
    {
        /* can announce the passage of a clock tick */
        rtems_clock_tick();

        /* reset the number of clock ISRs until next clock tick */
        Clock_driver_isrs = CLOCK_DRIVER_ISRS_PER_TICK;
    }

    /* decrement the number of clock ISRs until next clock tick */
    Clock_driver_isrs--;

    #else

    /* The driver is one ISR per clock tick. */

    /* announce the passage of the clock ticks */
    rtems_clock_update(dt);

    #endif

#endif
}


/**
 *  @brief install clock
 *  
 *  This routine actually performs the hardware initialization for the clock.
 *  Return values:      NONE
 *  
 *  @param[in] clock_isr clock interrupt service routine entry point
 */
RTEMS_STATIC_ATTRIBUTE void Install_clock(rtems_isr_entry clock_isr)
{
    /* reset the number of clock ticks */
    Clock_driver_ticks = 0;

#if ( BSP_CLOCK_DRIVER_NEEDS_FIND_TIMER_SUPPORT == TRUE )

    /* Find timer -- some BSPs search buses for hardware timer (e.g. leon3) */
    Clock_driver_support_find_timer();

#endif

    /* Install vector */
	/* GMV PMK : only rtems pmk is allowed to do this */
    Clock_driver_support_install_isr(clock_isr , &Old_ticker);

    /* set nanosecond extension */
    rtems_clock_set_nanoseconds_extension(Clock_driver_nanoseconds_since_last_tick);

    /* Now initialize the hardware that is the source of the tick ISR. */
	/* GMV PMK : only rtems pmk is allowed to do this */
    /* Clock_driver_support_initialize_hardware(); */
}


rtems_device_driver Clock_initialize(
                                     rtems_device_major_number major ,
                                     rtems_device_minor_number minor ,
                                     void *pargp
                                     )
{
    /* install the clock ISR */
    Install_clock(Clock_isr);

    /* make major/minor avail to others such as shared memory driver */

    /* set clock major number */
    rtems_clock_major = major;

    /* set clock minor number */
    rtems_clock_minor = minor;

#ifdef CLOCK_DRIVER_ISRS_PER_TICK

    /* If we are counting ISRs per tick, then initialize the counter */
    Clock_driver_isrs = CLOCK_DRIVER_ISRS_PER_TICK;

#endif

    /* return success always */
    return RTEMS_SUCCESSFUL;
}


/**  
 *  @}
 */
