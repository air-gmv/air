/**
 *  @file  rtems/score/watchdog.h
 *
 *  @brief Constants and Structures Associated with Watchdog Timers
 *
 *  This include file contains all the constants and structures associated
 *  with watchdog timers.   This Handler provides mechanisms which can be
 *  used to initialize and manipulate watchdog timers.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_WATCHDOG_H
#define _RTEMS_SCORE_WATCHDOG_H

#include <rtems/score/basedefs.h>
#include <rtems/score/chain.h>
#include <rtems/score/rbtree.h>

struct Per_CPU_Control;

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreWatchdog Watchdog Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality related to the scheduling of
 *  watchdog functions to be called at specific times in the future.
 *
 *  @note This handler does not have anything to do with hardware watchdog
 *        timers.
 */
/**@{*/

typedef struct Watchdog_Control Watchdog_Control;

/**
 *  @brief Type is used to specify the length of intervals.
 *
 *  This type is used to specify the length of intervals.
 */
typedef uint32_t   Watchdog_Interval;

/**
 * @brief Special watchdog ticks value to indicate an infinite wait.
 */
#define WATCHDOG_NO_TIMEOUT 0

/**
 *  @brief Return type from a Watchdog Service Routine.
 *
 *  This type defines the return type from a Watchdog Service Routine.
 */
typedef void Watchdog_Service_routine;

/**
 *  @brief Pointer to a watchdog service routine.
 *
 *  This type define a pointer to a watchdog service routine.
 */
typedef Watchdog_Service_routine
  ( *Watchdog_Service_routine_entry )( Watchdog_Control * );

/**
 * @brief The watchdog header to manage scheduled watchdogs.
 */
typedef struct {
  /**
   * @brief Red-black tree of scheduled watchdogs sorted by expiration time.
   */
  RBTree_Control Watchdogs;

  /**
   * @brief The scheduled watchdog with the earliest expiration time or NULL in
   * case no watchdog is scheduled.
   */
  RBTree_Node *first;
} Watchdog_Header;

/**
 *  @brief The control block used to manage each watchdog timer.
 *
 *  The following record defines the control block used
 *  to manage each watchdog timer.
 */
struct Watchdog_Control {
  /**
   * @brief Nodes for the watchdog.
   */
  union {
    /**
     * @brief this field is a red-black tree node structure and allows this to
     * be placed on a red-black tree used to manage the scheduled watchdogs.
     */
    RBTree_Node RBTree;

    /**
     * @brief this field is a chain node structure and allows this to be placed
     * on a chain used to manage pending watchdogs by the timer server.
     */
    Chain_Node Chain;
  } Node;

#if defined(RTEMS_SMP)
  /** @brief This field references the processor of this watchdog control. */
  struct Per_CPU_Control *cpu;
#endif

  /** @brief This field is the function to invoke. */
  Watchdog_Service_routine_entry routine;

  /** @brief This field is the expiration time point. */
  uint64_t expire;
};

/**
 * @brief The watchdog ticks counter.
 *
 * With a 1ms watchdog tick, this counter overflows after 50 days since boot.
 */
extern volatile Watchdog_Interval _Watchdog_Ticks_since_boot;

/**
 * @brief The watchdog nanoseconds per tick.
 *
 * This constant is defined by the application configuration via
 * <rtems/confdefs.h>.
 */
extern const uint32_t _Watchdog_Nanoseconds_per_tick;

/**
 * @brief The watchdog ticks per second.
 *
 * This constant is defined by the application configuration via
 * <rtems/confdefs.h>.
 */
extern const uint32_t _Watchdog_Ticks_per_second;

/**
 * @brief The maximum number of seconds representable in the monotonic watchdog
 * format.
 *
 * This constant is defined by the application configuration via
 * <rtems/confdefs.h>.
 */
extern const uint64_t _Watchdog_Monotonic_max_seconds;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
