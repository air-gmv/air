/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2008-2019
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file 
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE. 
 * ==========================================================================*/

#define CONFIGURE_INIT
#include <air.h>

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <rtems/score/timespec.h>
#include <rtems/test.h>
#include "tmacros.h"
#include "test_support.h"

#define TASKS 4
struct
{
  char       name[5];
  rtems_id   id;
  int        expected_cpu;
  cpu_set_t  cpuset;
  bool       run;
  int        actual_cpu;
}task_param[TASKS] = {
/*  name task  id task   expected cpu     cpuset   run     cpu  */
  {   "cT01"  ,  0x0   ,       1       , {{0x2}} , false  , -1},
  {   "cT02"  ,  0x0   ,       1       , {{0x2}} , false  , -1},
  {   "cT03"  ,  0x0   ,       1       , {{0x2}} , false  , -1},
  {   "sT51"  ,  0x0   ,       0       , {{0x1}} , false  , -1}
};

static char *my_ctime( time_t t )
{
  static char b[32];
  ctime_r(&t, b);
  b[ strlen(b) - 1] = '\0';
  return b;
}

rtems_task Periodic_task(rtems_task_argument arg)
{
    struct timespec start;
    char  buffer[10];   /* name assumed to be 10 characters or less */
    int ticks_per_sec = 1000000 / air_syscall_get_us_per_tick();

    while ( 1 ) {

        clock_gettime( CLOCK_REALTIME, &start );

        /* Print the cpu number and task name */
        locked_printf(
            "HELLO WORLD CPU %lu running task %s @ %s:%ld\n",
            rtems_get_current_processor(),
            rtems_object_get_name( task_param[arg].id, sizeof(buffer), buffer ),
            my_ctime(start.tv_sec), start.tv_nsec);

        rtems_task_wake_after(ticks_per_sec);
    }
}

void entry_point(void) 
{
    rtems_status_code status;    // status of rtems function

    locked_print_initialize();

    for (int i=0; i < TASKS; i++ ) 
    {
        status = rtems_task_create(
                                rtems_build_name( task_param[i].name[0],task_param[i].name[1],task_param[i].name[2], task_param[i].name[3]),
                                2,
                                RTEMS_MINIMUM_STACK_SIZE,
                                RTEMS_DEFAULT_MODES,
                                RTEMS_FLOATING_POINT,
                                &task_param[i].id
                              );

        directive_failed( status, "task create" );

        status = rtems_task_set_affinity( task_param[i].id, sizeof(cpu_set_t), &task_param[i].cpuset );

        rtems_test_assert(status == RTEMS_SUCCESSFUL);
        printf("> Set %s affinity to cpu %d\n", task_param[i].name, task_param[i].expected_cpu );

        status = rtems_task_start( task_param[ i ].id, Periodic_task, i );
        directive_failed( status, "task start" );
    }

    /* Delete Init task*/
    rtems_task_delete( RTEMS_SELF );
}
