/*
 * ============================================================================
 * This file was generated by the GMV's Configurator Tool and is part of an
 * AIR - ARINC 653 Interface in RTEMS - application configuration.
 * File Purpose  : RTEMS configuration
 * ============================================================================
 */

#include <rtems.h>

/**
 * @brief Standard RTEMS configuration
 */

#define SCHEDULER_A rtems_build_name(' ', ' ', ' ', 'A')

#define SCHEDULER_B rtems_build_name(' ', ' ', ' ', 'B')

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_SCHEDULER_SIMPLE_SMP

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_SIMPLE_SMP(a);
RTEMS_SCHEDULER_SIMPLE_SMP(b);

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(a, SCHEDULER_A), \
  RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(b, SCHEDULER_B)

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN(0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY), \
  RTEMS_SCHEDULER_ASSIGN(1, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL)


#define CONFIGURE_INIT_TASK_ATTRIBUTES                  RTEMS_FLOATING_POINT

#define CONFIGURE_MICROSECONDS_PER_TICK 2000

#define CONFIGURE_MAXIMUM_TASKS                         20
#define CONFIGURE_MAXIMUM_TIMERS 4

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_PROCESSORS 2

#define CONFIGURE_INIT


#include <rtems/confdefs.h>