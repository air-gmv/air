/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
#ifndef SYSTEM_H
#define SYSTEM_H

#include "tmacros.h"
#include "test_support.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

/* functions */

rtems_task Periodic_task(rtems_task_argument arg);
rtems_task Periodic_task1(rtems_task_argument arg);
rtems_task Periodic_task2(rtems_task_argument arg);
rtems_task Periodic_task3(rtems_task_argument arg);
rtems_task Periodic_task4(rtems_task_argument arg);
rtems_task Periodic_task5(rtems_task_argument arg);
rtems_task Periodic_task6(rtems_task_argument arg);
rtems_task Periodic_task7(rtems_task_argument arg);
rtems_task Periodic_task8(rtems_task_argument arg);
rtems_task Periodic_task9(rtems_task_argument arg);

rtems_task Init(
  rtems_task_argument argument
);

void print_tasks_info(rtems_id id, int num_id);

/* configuration information */

#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_PROCESSORS   4 

#define CONFIGURE_MAXIMUM_TASKS     20
#define CONFIGURE_MAXIMUM_TIMERS    32

#define CONFIGURE_MAXIMUM_PERIODS    20

#define CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_EXTRA_TASK_STACKS         (64 * RTEMS_MINIMUM_STACK_SIZE)

#include <rtems/confdefs.h>

void PrintTaskInfo(
  const char         *task_name,
  rtems_time_of_day  *_tb 
);

void PrintTask(
  const char         *task_name
);

typedef struct  
{
  char       name[5];
  rtems_id   id;
  int        expected_cpu;
  cpu_set_t  cpuset;
  bool       ran;
  int        actual_cpu;
}task_param_t;


#endif
/* end of include file */
