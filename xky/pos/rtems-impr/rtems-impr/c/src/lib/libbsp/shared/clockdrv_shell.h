/**
 * @file
 *
 * @ingroup bsp_clock
 *
 * @brief Clock Tick Device Driver Shell
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>

#include <bsp.h>
#include <rtems/clockdrv.h>
#include <rtems/score/percpu.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/watchdogimpl.h>

#ifdef Clock_driver_nanoseconds_since_last_tick
#error "Update driver to use the timecounter instead of nanoseconds extension"
#endif

/**
 * @defgroup bsp_clock Clock Support
 *
 * @ingroup bsp_shared
 *
 * @brief Clock support 
 *
 */
#if CLOCK_DRIVER_USE_FAST_IDLE && CLOCK_DRIVER_ISRS_PER_TICK
#error "clockdrv_shell.h: Fast Idle PLUS n ISRs per tick is not supported"
#endif

/**
 * @brief Do nothing by default.
 */
#ifndef Clock_driver_support_install_isr
  #define Clock_driver_support_install_isr(isr)
#endif

/**
 * @brief This method is rarely used so default it.
 */
#ifndef Clock_driver_support_find_timer
  #define Clock_driver_support_find_timer()
#endif

/**
 * @brief Do nothing by default.
 */
#ifndef Clock_driver_support_at_tick
  #define Clock_driver_support_at_tick()
#endif

/**
 * @brief Do nothing by default.
 */
#ifndef Clock_driver_support_set_interrupt_affinity
  #define Clock_driver_support_set_interrupt_affinity(online_processors)
#endif

/*
 * A specialized clock driver may use for example rtems_timecounter_tick_simple()
 * instead of the default.
 */
#ifndef Clock_driver_timecounter_tick
static void Clock_driver_timecounter_tick( void )
{
#if defined(CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER)
  rtems_clock_tick();
#elif defined(RTEMS_SMP) && defined(CLOCK_DRIVER_USE_ONLY_BOOT_PROCESSOR)
  uint32_t cpu_count = _SMP_Get_processor_count();
  uint32_t cpu_index;

  for ( cpu_index = 0 ; cpu_index < cpu_count ; ++cpu_index ) {
    Per_CPU_Control *cpu;

    cpu = _Per_CPU_Get_by_index( cpu_index );

    if ( _Per_CPU_Is_boot_processor( cpu ) ) {
      rtems_timecounter_tick();
    } else if ( _Processor_mask_Is_set( _SMP_Get_online_processors(), cpu_index ) ) {
      _Watchdog_Tick( cpu );
    }
  }
#else
  rtems_timecounter_tick();
#endif
}
#endif

/**
 * @brief ISRs until next clock tick
 */
#if CLOCK_DRIVER_ISRS_PER_TICK
  volatile uint32_t  Clock_driver_isrs;
#endif

/**
 * @brief Clock ticks since initialization
 */
volatile uint32_t    Clock_driver_ticks;

#ifdef Clock_driver_support_shutdown_hardware
void Clock_exit( void );
#endif

/**
 *  @brief Clock_isr
 *
 *  This is the clock tick interrupt handler.
 *
 *  @param vector Vector number.
 */
#if defined(BSP_FEATURE_IRQ_EXTENSION) || \
    (CPU_SIMPLE_VECTORED_INTERRUPTS != TRUE)
void Clock_isr(void *arg);
void Clock_isr(void *arg)
{
#else
rtems_isr Clock_isr(rtems_vector_number vector);
rtems_isr Clock_isr(
  rtems_vector_number vector
)
{
#endif
  /*
   *  Accurate count of ISRs
   */
  Clock_driver_ticks += 1;

  #if CLOCK_DRIVER_USE_FAST_IDLE
    {
      struct timecounter *tc = _Timecounter;
      uint64_t us_per_tick = rtems_configuration_get_microseconds_per_tick();
      uint32_t interval = (uint32_t)
        ((tc->tc_frequency * us_per_tick) / 1000000);

      Clock_driver_timecounter_tick();

      if (!rtems_configuration_is_smp_enabled()) {
        while (
          _Thread_Heir == _Thread_Executing && _Thread_Executing->is_idle
        ) {
          ISR_lock_Context lock_context;

          _Timecounter_Acquire(&lock_context);
          _Timecounter_Tick_simple(
            interval,
            (*tc->tc_get_timecount)(tc),
            &lock_context
          );
        }
      }

      Clock_driver_support_at_tick();
    }
  #else
    /*
     *  Do the hardware specific per-tick action.
     *
     *  The counter/timer may or may not be set to automatically reload.
     */
    Clock_driver_support_at_tick();

    #if CLOCK_DRIVER_ISRS_PER_TICK
      /*
       *  The driver is multiple ISRs per clock tick.
       */
      if ( !Clock_driver_isrs ) {
        Clock_driver_timecounter_tick();

        Clock_driver_isrs = CLOCK_DRIVER_ISRS_PER_TICK;
      }
      Clock_driver_isrs--;
    #else
      /*
       *  The driver is one ISR per clock tick.
       */
      Clock_driver_timecounter_tick();
    #endif
  #endif
}

#ifdef Clock_driver_support_shutdown_hardware
/**
 *  @brief Clock_exit
 *
 *  This routine allows the clock driver to exit by masking the interrupt and
 *  disabling the clock's counter.
 */
void Clock_exit( void )
{
  Clock_driver_support_shutdown_hardware();

  /* do not restore old vector */
}
#endif

/**
 * @brief Clock_initialize
 *
 * This routine initializes the clock driver.
 *
 * @param[in] major Clock device major number.
 * @param[in] minor Clock device minor number.
 * @param[in] parg  Pointer to optional device driver arguments
 *
 * @retval rtems_device_driver status code
 */
rtems_device_driver Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  Clock_driver_ticks = 0;

  /*
   *  Find timer -- some BSPs search buses for hardware timer
   */
  Clock_driver_support_find_timer();

  /*
   *  Install vector
   */
  Clock_driver_support_install_isr( Clock_isr );

  #ifdef RTEMS_SMP
    Clock_driver_support_set_interrupt_affinity(
      _SMP_Get_online_processors()
    );
  #endif

  /*
   *  Now initialize the hardware that is the source of the tick ISR.
   */
  Clock_driver_support_initialize_hardware();

#ifdef Clock_driver_support_shutdown_hardware
  atexit( Clock_exit );
#endif

  /*
   *  If we are counting ISRs per tick, then initialize the counter.
   */
  #if CLOCK_DRIVER_ISRS_PER_TICK
    Clock_driver_isrs = CLOCK_DRIVER_ISRS_PER_TICK_VALUE;
  #endif

  return RTEMS_SUCCESSFUL;
}
