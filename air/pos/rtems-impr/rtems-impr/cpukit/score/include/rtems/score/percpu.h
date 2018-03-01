/**
 *  @file  rtems/score/percpu.h
 *
 *  This include file defines the per CPU information required
 *  by RTEMS.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2012, 2016 embedded brains GmbH
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_PERCPU_H
#define _RTEMS_PERCPU_H

#include <rtems/score/cpuimpl.h>

#if defined( ASM )
  #include <rtems/asm.h>
#else
  #include <rtems/score/assert.h>
  #include <rtems/score/chain.h>
  #include <rtems/score/isrlock.h>
  #include <rtems/score/smp.h>
  #include <rtems/score/smplock.h>
  #include <rtems/score/timestamp.h>
  #include <rtems/score/watchdog.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(RTEMS_SMP)
  #if defined(RTEMS_PROFILING)
    #define PER_CPU_CONTROL_SIZE_APPROX ( 512 + CPU_INTERRUPT_FRAME_SIZE )
  #elif defined(RTEMS_DEBUG)
    #define PER_CPU_CONTROL_SIZE_APPROX ( 256 + CPU_INTERRUPT_FRAME_SIZE )
  #else
    #define PER_CPU_CONTROL_SIZE_APPROX ( 128 + CPU_INTERRUPT_FRAME_SIZE )
  #endif

  /*
   * This ensures that on SMP configurations the individual per-CPU controls
   * are on different cache lines to prevent false sharing.  This define can be
   * used in assembler code to easily get the per-CPU control for a particular
   * processor.
   */
  #if PER_CPU_CONTROL_SIZE_APPROX > 1024
    #define PER_CPU_CONTROL_SIZE_LOG2 11
  #elif PER_CPU_CONTROL_SIZE_APPROX > 512
    #define PER_CPU_CONTROL_SIZE_LOG2 10
  #elif PER_CPU_CONTROL_SIZE_APPROX > 256
    #define PER_CPU_CONTROL_SIZE_LOG2 9
  #elif PER_CPU_CONTROL_SIZE_APPROX > 128
    #define PER_CPU_CONTROL_SIZE_LOG2 8
  #else
    #define PER_CPU_CONTROL_SIZE_LOG2 7
  #endif

  #define PER_CPU_CONTROL_SIZE ( 1 << PER_CPU_CONTROL_SIZE_LOG2 )
#endif

#if !defined( ASM )

struct _Thread_Control;

struct Scheduler_Context;

/**
 *  @defgroup PerCPU RTEMS Per CPU Information
 *
 *  @ingroup Score
 *
 *  This defines the per CPU state information required by RTEMS
 *  and the BSP.  In an SMP configuration, there will be multiple
 *  instances of this data structure -- one per CPU -- and the
 *  current CPU number will be used as the index.
 */

/**@{*/

#if defined( RTEMS_SMP )

/**
 * @brief State of a processor.
 *
 * The processor state controls the life cycle of processors at the lowest
 * level.  No multi-threading or other high-level concepts matter here.
 *
 * State changes must be initiated via _Per_CPU_State_change().  This function
 * may not return in case someone requested a shutdown.  The
 * _SMP_Send_message() function will be used to notify other processors about
 * state changes if the other processor is in the up state.
 *
 * Due to the sequential nature of the basic system initialization one
 * processor has a special role.  It is the processor executing the boot_card()
 * function.  This processor is called the boot processor.  All other
 * processors are called secondary.
 *
 * @dot
 * digraph states {
 *   i [label="PER_CPU_STATE_INITIAL"];
 *   rdy [label="PER_CPU_STATE_READY_TO_START_MULTITASKING"];
 *   reqsm [label="PER_CPU_STATE_REQUEST_START_MULTITASKING"];
 *   u [label="PER_CPU_STATE_UP"];
 *   s [label="PER_CPU_STATE_SHUTDOWN"];
 *   i -> rdy [label="processor\ncompleted initialization"];
 *   rdy -> reqsm [label="boot processor\ncompleted initialization"];
 *   reqsm -> u [label="processor\nstarts multitasking"];
 *   i -> s;
 *   rdy -> s;
 *   reqsm -> s;
 *   u -> s;
 * }
 * @enddot
 */
typedef enum {
  /**
   * @brief The per CPU controls are initialized to zero.
   *
   * The boot processor executes the sequential boot code in this state.  The
   * secondary processors should perform their basic initialization now and
   * change into the PER_CPU_STATE_READY_TO_START_MULTITASKING state once this
   * is complete.
   */
  PER_CPU_STATE_INITIAL,

  /**
   * @brief Processor is ready to start multitasking.
   *
   * The secondary processor performed its basic initialization and is ready to
   * receive inter-processor interrupts.  Interrupt delivery must be disabled
   * in this state, but requested inter-processor interrupts must be recorded
   * and must be delivered once the secondary processor enables interrupts for
   * the first time.  The boot processor will wait for all secondary processors
   * to change into this state.  In case a secondary processor does not reach
   * this state the system will not start.  The secondary processors wait now
   * for a change into the PER_CPU_STATE_REQUEST_START_MULTITASKING state set
   * by the boot processor once all secondary processors reached the
   * PER_CPU_STATE_READY_TO_START_MULTITASKING state.
   */
  PER_CPU_STATE_READY_TO_START_MULTITASKING,

  /**
   * @brief Multitasking start of processor is requested.
   *
   * The boot processor completed system initialization and is about to perform
   * a context switch to its heir thread.  Secondary processors should now
   * issue a context switch to the heir thread.  This normally enables
   * interrupts on the processor for the first time.
   */
  PER_CPU_STATE_REQUEST_START_MULTITASKING,

  /**
   * @brief Normal multitasking state.
   */
  PER_CPU_STATE_UP,

  /**
   * @brief This is the terminal state.
   */
  PER_CPU_STATE_SHUTDOWN
} Per_CPU_State;

#endif /* defined( RTEMS_SMP ) */

/**
 * @brief Per-CPU statistics.
 */
typedef struct {
#if defined( RTEMS_PROFILING )
  /**
   * @brief The thread dispatch disabled begin instant in CPU counter ticks.
   *
   * This value is used to measure the time of disabled thread dispatching.
   */
  CPU_Counter_ticks thread_dispatch_disabled_instant;

  /**
   * @brief The maximum time of disabled thread dispatching in CPU counter
   * ticks.
   */
  CPU_Counter_ticks max_thread_dispatch_disabled_time;

  /**
   * @brief The maximum time spent to process a single sequence of nested
   * interrupts in CPU counter ticks.
   *
   * This is the time interval between the change of the interrupt nest level
   * from zero to one and the change back from one to zero.
   */
  CPU_Counter_ticks max_interrupt_time;

  /**
   * @brief The maximum interrupt delay in CPU counter ticks if supported by
   * the hardware.
   */
  CPU_Counter_ticks max_interrupt_delay;

  /**
   * @brief Count of times when the thread dispatch disable level changes from
   * zero to one in thread context.
   *
   * This value may overflow.
   */
  uint64_t thread_dispatch_disabled_count;

  /**
   * @brief Total time of disabled thread dispatching in CPU counter ticks.
   *
   * The average time of disabled thread dispatching is the total time of
   * disabled thread dispatching divided by the thread dispatch disabled
   * count.
   *
   * This value may overflow.
   */
  uint64_t total_thread_dispatch_disabled_time;

  /**
   * @brief Count of times when the interrupt nest level changes from zero to
   * one.
   *
   * This value may overflow.
   */
  uint64_t interrupt_count;

  /**
   * @brief Total time of interrupt processing in CPU counter ticks.
   *
   * The average time of interrupt processing is the total time of interrupt
   * processing divided by the interrupt count.
   *
   * This value may overflow.
   */
  uint64_t total_interrupt_time;
#endif /* defined( RTEMS_PROFILING ) */
} Per_CPU_Stats;

/**
 * @brief Per-CPU watchdog header index.
 */
typedef enum {
  /**
   * @brief Index for monotonic clock per-CPU watchdog header.
   *
   * The reference time point for the monotonic clock is the system start.  The
   * clock resolution is one system clock tick.  It is used for the system
   * clock tick based time services and the POSIX services using
   * CLOCK_MONOTONIC.
   */
  PER_CPU_WATCHDOG_MONOTONIC,

  /**
   * @brief Index for realtime clock per-CPU watchdog header.
   *
   * The reference time point for the realtime clock is the POSIX Epoch.  The
   * clock resolution is one nanosecond.  It is used for the time of day
   * services and the POSIX services using CLOCK_REALTIME.
   */
  PER_CPU_WATCHDOG_REALTIME,

  /**
   * @brief Count of per-CPU watchdog headers.
   */
  PER_CPU_WATCHDOG_COUNT
} Per_CPU_Watchdog_index;

/**
 *  @brief Per CPU Core Structure
 *
 *  This structure is used to hold per core state information.
 */
typedef struct Per_CPU_Control {
  #if CPU_PER_CPU_CONTROL_SIZE > 0
    /**
     * @brief CPU port specific control.
     */
    CPU_Per_CPU_control cpu_per_cpu;
  #endif

  #if (CPU_ALLOCATE_INTERRUPT_STACK == TRUE) || \
      (CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
    /**
     * This contains a pointer to the lower range of the interrupt stack for
     * this CPU.  This is the address allocated and freed.
     */
    void  *interrupt_stack_low;

    /**
     * This contains a pointer to the interrupt stack pointer for this CPU.
     * It will be loaded at the beginning on an ISR.
     */
    void  *interrupt_stack_high;
  #endif

  /**
   *  This contains the current interrupt nesting level on this
   *  CPU.
   */
  uint32_t isr_nest_level;

  /**
   * @brief Indicetes if an ISR thread dispatch is disabled.
   *
   * This flag is context switched with each thread.  It indicates that this
   * thread has an interrupt stack frame on its stack.  By using this flag, we
   * can avoid nesting more interrupt dispatching attempts on a previously
   * interrupted thread's stack.
   */
  uint32_t isr_dispatch_disable;

  /**
   * @brief The thread dispatch critical section nesting counter which is used
   * to prevent context switches at inopportune moments.
   */
  volatile uint32_t thread_dispatch_disable_level;

  /**
   * @brief This is set to true when this processor needs to run the thread
   * dispatcher.
   *
   * It is volatile since interrupts may alter this flag.
   *
   * This field is not protected by a lock and must be accessed only by this
   * processor.  Code (e.g. scheduler and post-switch action requests) running
   * on another processors must use an inter-processor interrupt to set the
   * thread dispatch necessary indicator to true.
   *
   * @see _Thread_Get_heir_and_make_it_executing().
   */
  volatile bool dispatch_necessary;

  /*
   * Ensure that the executing member is at least 4-byte aligned, see
   * PER_CPU_OFFSET_EXECUTING.  This is necessary on CPU ports with relaxed
   * alignment restrictions, e.g. type alignment is less than the type size.
   */
  bool reserved_for_executing_alignment[ 3 ];

  /**
   * @brief This is the thread executing on this processor.
   *
   * This field is not protected by a lock.  The only writer is this processor.
   *
   * On SMP configurations a thread may be registered as executing on more than
   * one processor in case a thread migration is in progress.  On SMP
   * configurations use _Thread_Is_executing_on_a_processor() to figure out if
   * a thread context is executing on a processor.
   */
  struct _Thread_Control *executing;

  /**
   * @brief This is the heir thread for this processor.
   *
   * This field is not protected by a lock.  The only writer after multitasking
   * start is the scheduler owning this processor.  It is assumed that stores
   * to pointers are atomic on all supported SMP architectures.  The CPU port
   * specific code (inter-processor interrupt handling and
   * _CPU_SMP_Send_interrupt()) must guarantee that this processor observes the
   * last value written.
   *
   * A thread can be a heir on at most one processor in the system.
   *
   * @see _Thread_Get_heir_and_make_it_executing().
   */
  struct _Thread_Control *heir;

#if defined(RTEMS_SMP)
  CPU_Interrupt_frame Interrupt_frame;
#endif

  /**
   * @brief The CPU usage timestamp contains the time point of the last heir
   * thread change or last CPU usage update of the executing thread of this
   * processor.
   *
   * Protected by the scheduler lock.
   *
   * @see _Scheduler_Update_heir(), _Thread_Dispatch_update_heir() and
   * _Thread_Get_CPU_time_used().
   */
  Timestamp_Control cpu_usage_timestamp;

  /**
   * @brief Watchdog state for this processor.
   */
  struct {
    /**
     * @brief Protects all watchdog operations on this processor.
     */
    ISR_LOCK_MEMBER( Lock )

    /**
     * @brief Watchdog ticks on this processor used for monotonic clock
     * watchdogs.
     */
    uint64_t ticks;

    /**
     * @brief Header for watchdogs.
     *
     * @see Per_CPU_Watchdog_index.
     */
    Watchdog_Header Header[ PER_CPU_WATCHDOG_COUNT ];
  } Watchdog;

  #if defined( RTEMS_SMP )
    /**
     * @brief This lock protects some parts of the low-level thread dispatching.
     *
     * We must use a ticket lock here since we cannot transport a local context
     * through the context switch.
     *
     * @see _Thread_Dispatch().
     */
    SMP_ticket_lock_Control Lock;

    #if defined( RTEMS_PROFILING )
      /**
       * @brief Lock statistics for the per-CPU lock.
       */
      SMP_lock_Stats Lock_stats;

      /**
       * @brief Lock statistics context for the per-CPU lock.
       */
      SMP_lock_Stats_context Lock_stats_context;
    #endif

    /**
     * @brief Chain of threads in need for help.
     *
     * This field is protected by the Per_CPU_Control::Lock lock.
     */
    Chain_Control Threads_in_need_for_help;

    /**
     * @brief Bit field for SMP messages.
     *
     * This bit field is not protected locks.  Atomic operations are used to
     * set and get the message bits.
     */
    Atomic_Ulong message;

    struct {
      /**
       * @brief The scheduler control of the scheduler owning this processor.
       *
       * This pointer is NULL in case this processor is currently not used by a
       * scheduler instance.
       */
      const struct _Scheduler_Control *control;

      /**
       * @brief The scheduler context of the scheduler owning this processor.
       *
       * This pointer is NULL in case this processor is currently not used by a
       * scheduler instance.
       */
      const struct Scheduler_Context *context;

      /**
       * @brief The idle thread for this processor in case it is online and
       * currently not used by a scheduler instance.
       */
      struct _Thread_Control *idle_if_online_and_unused;
    } Scheduler;

    /**
     * @brief Indicates the current state of the CPU.
     *
     * This field is protected by the _Per_CPU_State_lock lock.
     *
     * @see _Per_CPU_State_change().
     */
    Per_CPU_State state;

    /**
     * @brief Action to be executed by this processor in the
     * SYSTEM_STATE_BEFORE_MULTITASKING state on behalf of the boot processor.
     *
     * @see _SMP_Before_multitasking_action().
     */
    Atomic_Uintptr before_multitasking_action;

    /**
     * @brief Indicates if the processor has been successfully started via
     * _CPU_SMP_Start_processor().
     */
    bool online;

    /**
     * @brief Indicates if the processor is the one that performed the initial
     * system initialization.
     */
    bool boot;
  #endif

  Per_CPU_Stats Stats;
} Per_CPU_Control;

#if defined( RTEMS_SMP )
typedef struct {
  Per_CPU_Control per_cpu;
  char unused_space_for_cache_line_alignment
    [ PER_CPU_CONTROL_SIZE - sizeof( Per_CPU_Control ) ];
} Per_CPU_Control_envelope;
#else
typedef struct {
  Per_CPU_Control per_cpu;
} Per_CPU_Control_envelope;
#endif

/**
 *  @brief Set of Per CPU Core Information
 *
 *  This is an array of per CPU core information.
 */
extern Per_CPU_Control_envelope _Per_CPU_Information[] CPU_STRUCTURE_ALIGNMENT;

#if defined( RTEMS_SMP )
#define _Per_CPU_Acquire( cpu ) \
  _SMP_ticket_lock_Acquire( \
    &( cpu )->Lock, \
    &( cpu )->Lock_stats, \
    &( cpu )->Lock_stats_context \
  )
#else
#define _Per_CPU_Acquire( cpu ) \
  do { \
    (void) ( cpu ); \
  } while ( 0 )
#endif

#if defined( RTEMS_SMP )
#define _Per_CPU_Release( cpu ) \
  _SMP_ticket_lock_Release( \
    &( cpu )->Lock, \
    &( cpu )->Lock_stats_context \
  )
#else
#define _Per_CPU_Release( cpu ) \
  do { \
    (void) ( cpu ); \
  } while ( 0 )
#endif

#if defined( RTEMS_SMP )
#define _Per_CPU_ISR_disable_and_acquire( cpu, isr_cookie ) \
  do { \
    _ISR_Local_disable( isr_cookie ); \
    _Per_CPU_Acquire( cpu ); \
  } while ( 0 )
#else
#define _Per_CPU_ISR_disable_and_acquire( cpu, isr_cookie ) \
  do { \
    _ISR_Local_disable( isr_cookie ); \
    (void) ( cpu ); \
  } while ( 0 )
#endif

#if defined( RTEMS_SMP )
#define _Per_CPU_Release_and_ISR_enable( cpu, isr_cookie ) \
  do { \
    _Per_CPU_Release( cpu ); \
    _ISR_Local_enable( isr_cookie ); \
  } while ( 0 )
#else
#define _Per_CPU_Release_and_ISR_enable( cpu, isr_cookie ) \
  do { \
    (void) ( cpu ); \
    _ISR_Local_enable( isr_cookie ); \
  } while ( 0 )
#endif

#if defined( RTEMS_SMP )
#define _Per_CPU_Acquire_all( isr_cookie ) \
  do { \
    uint32_t ncpus = _SMP_Get_processor_count(); \
    uint32_t cpu; \
    _ISR_Local_disable( isr_cookie ); \
    for ( cpu = 0 ; cpu < ncpus ; ++cpu ) { \
      _Per_CPU_Acquire( _Per_CPU_Get_by_index( cpu ) ); \
    } \
  } while ( 0 )
#else
#define _Per_CPU_Acquire_all( isr_cookie ) \
  _ISR_Local_disable( isr_cookie )
#endif

#if defined( RTEMS_SMP )
#define _Per_CPU_Release_all( isr_cookie ) \
  do { \
    uint32_t ncpus = _SMP_Get_processor_count(); \
    uint32_t cpu; \
    for ( cpu = 0 ; cpu < ncpus ; ++cpu ) { \
      _Per_CPU_Release( _Per_CPU_Get_by_index( cpu ) ); \
    } \
    _ISR_Local_enable( isr_cookie ); \
  } while ( 0 )
#else
#define _Per_CPU_Release_all( isr_cookie ) \
  _ISR_Local_enable( isr_cookie )
#endif

/*
 * If we get the current processor index in a context which allows thread
 * dispatching, then we may already run on another processor right after the
 * read instruction.  There are very few cases in which this makes sense (here
 * we can use _Per_CPU_Get_snapshot()).  All other places must use
 * _Per_CPU_Get() so that we can add checks for RTEMS_DEBUG.
 */
#if defined( _CPU_Get_current_per_CPU_control )
  #define _Per_CPU_Get_snapshot() _CPU_Get_current_per_CPU_control()
#else
  #define _Per_CPU_Get_snapshot() \
    ( &_Per_CPU_Information[ _SMP_Get_current_processor() ].per_cpu )
#endif

#if defined( RTEMS_SMP )
static inline Per_CPU_Control *_Per_CPU_Get( void )
{
  Per_CPU_Control *cpu_self = _Per_CPU_Get_snapshot();

  _Assert(
    cpu_self->thread_dispatch_disable_level != 0 || _ISR_Get_level() != 0
  );

  return cpu_self;
}
#else
#define _Per_CPU_Get() _Per_CPU_Get_snapshot()
#endif

static inline Per_CPU_Control *_Per_CPU_Get_by_index( uint32_t index )
{
  return &_Per_CPU_Information[ index ].per_cpu;
}

static inline uint32_t _Per_CPU_Get_index( const Per_CPU_Control *cpu )
{
  const Per_CPU_Control_envelope *per_cpu_envelope =
    ( const Per_CPU_Control_envelope * ) cpu;

  return ( uint32_t ) ( per_cpu_envelope - &_Per_CPU_Information[ 0 ] );
}

static inline struct _Thread_Control *_Per_CPU_Get_executing(
  const Per_CPU_Control *cpu
)
{
  return cpu->executing;
}

static inline bool _Per_CPU_Is_processor_online(
  const Per_CPU_Control *cpu
)
{
#if defined( RTEMS_SMP )
  return cpu->online;
#else
  (void) cpu;

  return true;
#endif
}

static inline bool _Per_CPU_Is_boot_processor(
  const Per_CPU_Control *cpu
)
{
#if defined( RTEMS_SMP )
  return cpu->boot;
#else
  (void) cpu;

  return true;
#endif
}

#if defined( RTEMS_SMP )

/**
 *  @brief Allocate and Initialize Per CPU Structures
 *
 *  This method allocates and initialize the per CPU structure.
 */
void _Per_CPU_Initialize(void);

void _Per_CPU_State_change(
  Per_CPU_Control *cpu,
  Per_CPU_State new_state
);

/**
 * @brief Waits for a processor to change into a non-initial state.
 *
 * This function should be called only in _CPU_SMP_Start_processor() if
 * required by the CPU port or BSP.
 *
 * @code
 * bool _CPU_SMP_Start_processor(uint32_t cpu_index)
 * {
 *   uint32_t timeout = 123456;
 *
 *   start_the_processor(cpu_index);
 *
 *   return _Per_CPU_State_wait_for_non_initial_state(cpu_index, timeout);
 * }
 * @endcode
 *
 * @param[in] cpu_index The processor index.
 * @param[in] timeout_in_ns The timeout in nanoseconds.  Use a value of zero to
 * wait forever if necessary.
 *
 * @retval true The processor is in a non-initial state.
 * @retval false The timeout expired before the processor reached a non-initial
 * state.
 */
bool _Per_CPU_State_wait_for_non_initial_state(
  uint32_t cpu_index,
  uint32_t timeout_in_ns
);

#endif /* defined( RTEMS_SMP ) */

/*
 * On a non SMP system, the _SMP_Get_current_processor() is defined to 0.
 * Thus when built for non-SMP, there should be no performance penalty.
 */
#define _Thread_Dispatch_disable_level \
  _Per_CPU_Get()->thread_dispatch_disable_level
#define _Thread_Heir \
  _Per_CPU_Get()->heir

#if defined(_CPU_Get_thread_executing)
#define _Thread_Executing \
  _CPU_Get_thread_executing()
#else
#define _Thread_Executing \
  _Per_CPU_Get_executing( _Per_CPU_Get() )
#endif

#define _ISR_Nest_level \
  _Per_CPU_Get()->isr_nest_level
#define _CPU_Interrupt_stack_low \
  _Per_CPU_Get()->interrupt_stack_low
#define _CPU_Interrupt_stack_high \
  _Per_CPU_Get()->interrupt_stack_high
#define _Thread_Dispatch_necessary \
  _Per_CPU_Get()->dispatch_necessary

/**
 * @brief Returns the thread control block of the executing thread.
 *
 * This function can be called in any thread context.  On SMP configurations,
 * interrupts are disabled to ensure that the processor index is used
 * consistently if no CPU port specific method is available to get the
 * executing thread.
 *
 * @return The thread control block of the executing thread.
 */
RTEMS_INLINE_ROUTINE struct _Thread_Control *_Thread_Get_executing( void )
{
  struct _Thread_Control *executing;

  #if defined(RTEMS_SMP) && !defined(_CPU_Get_thread_executing)
    ISR_Level level;

    _ISR_Local_disable( level );
  #endif

  executing = _Thread_Executing;

  #if defined(RTEMS_SMP) && !defined(_CPU_Get_thread_executing)
    _ISR_Local_enable( level );
  #endif

  return executing;
}

/**@}*/

#endif /* !defined( ASM ) */

#if defined( ASM ) || defined( _RTEMS_PERCPU_DEFINE_OFFSETS )

#if (CPU_ALLOCATE_INTERRUPT_STACK == TRUE) || \
    (CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE)
  /*
   *  If this CPU target lets RTEMS allocates the interrupt stack, then
   *  we need to have places in the per CPU table to hold them.
   */
  #define PER_CPU_INTERRUPT_STACK_LOW \
    CPU_PER_CPU_CONTROL_SIZE
  #define PER_CPU_INTERRUPT_STACK_HIGH \
    PER_CPU_INTERRUPT_STACK_LOW + CPU_SIZEOF_POINTER
  #define PER_CPU_END_STACK             \
    PER_CPU_INTERRUPT_STACK_HIGH + CPU_SIZEOF_POINTER

  #define INTERRUPT_STACK_LOW \
    (SYM(_Per_CPU_Information) + PER_CPU_INTERRUPT_STACK_LOW)
  #define INTERRUPT_STACK_HIGH \
    (SYM(_Per_CPU_Information) + PER_CPU_INTERRUPT_STACK_HIGH)
#else
  #define PER_CPU_END_STACK \
    CPU_PER_CPU_CONTROL_SIZE
#endif

/*
 *  These are the offsets of the required elements in the per CPU table.
 */
#define PER_CPU_ISR_NEST_LEVEL \
  PER_CPU_END_STACK
#define PER_CPU_ISR_DISPATCH_DISABLE \
  PER_CPU_ISR_NEST_LEVEL + 4
#define PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL \
  PER_CPU_ISR_DISPATCH_DISABLE + 4
#define PER_CPU_DISPATCH_NEEDED \
  PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL + 4
#define PER_CPU_OFFSET_EXECUTING \
  PER_CPU_DISPATCH_NEEDED + 4
#define PER_CPU_OFFSET_HEIR \
  PER_CPU_OFFSET_EXECUTING + CPU_SIZEOF_POINTER
#if defined(RTEMS_SMP)
#define PER_CPU_INTERRUPT_FRAME_AREA \
  PER_CPU_OFFSET_HEIR + CPU_SIZEOF_POINTER
#endif

#define THREAD_DISPATCH_DISABLE_LEVEL \
  (SYM(_Per_CPU_Information) + PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL)
#define ISR_NEST_LEVEL \
  (SYM(_Per_CPU_Information) + PER_CPU_ISR_NEST_LEVEL)
#define DISPATCH_NEEDED \
  (SYM(_Per_CPU_Information) + PER_CPU_DISPATCH_NEEDED)

#endif /* defined( ASM ) || defined( _RTEMS_PERCPU_DEFINE_OFFSETS ) */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
