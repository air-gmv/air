/*
 * Copyright (C) 2018  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */

/* Support function */
#include "system.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <rtems.h>

#include <bsp.h> /* for device driver prototypes */

void print_tasks_info(rtems_id id, int num_id)
{
    char  buffer[10];   /* name assumed to be 10 characters or less */
    char *result;
        
    result = rtems_object_get_name( id, sizeof(buffer), buffer );
    locked_printf("  | Task no. %d | ID=0x%08x | name=%s |\n", num_id, id, ((result) ? result : "no name") );
      locked_printf("  ------------------------------------------\n");
}

void PrintTaskInfo(
  const char         *task_name,
  rtems_time_of_day  *_tb 
) 
{
  uint32_t cpu_num;

  cpu_num = rtems_get_current_processor();

  /* Print the cpu number and task name */
  locked_printf(
    "  CPU %" PRIu32 " running task %s - rtems_clock_get_tod "
    "%02" PRId32 ":%02" PRId32 ":%02" PRId32 "   %02" PRId32 
        "/%02" PRId32 "/%04" PRId32 "\n",
    cpu_num,
    task_name,
    _tb->hour, _tb->minute, _tb->second, 
    _tb->month, _tb->day, _tb->year
  );  
} 

void PrintTask(
  const char         *task_name
)
{
  uint32_t cpu_num;
      char  buffer[10];   /* name assumed to be 10 characters or less */
    char *result;
        
    result = rtems_object_get_name( task_name, sizeof(buffer), buffer );

  cpu_num = rtems_get_current_processor();

  /* Print the cpu number and task name */
  locked_printf(
    "  CPU %" PRIu32 " running task %s \n",
    cpu_num,
    ((result) ? result : "no name")
  ); 
}
