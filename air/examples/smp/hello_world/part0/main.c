/*
 * Copyright (C) 2018  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */

#define CONFIGURE_INIT
#include <air.h>

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <rtems/score/timespec.h>
#include <rtems/test.h>
#include "tmacros.h"
#include "test_support.h"

struct
{
  char       name[5];
  rtems_id   id;
  int        expected_cpu;
  cpu_set_t  cpuset;
  bool       run;
  int        actual_cpu;
}task_param[2] = {
/*  name task  id task   expected cpu     cpuset   run     cpu  */
  {   "TA01"  ,  0x0   ,       1       , {{0x2}} , false  , -1},
  {   "TA02"  ,  0x0   ,       0       , {{0x1}} , false  , -1}
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

    rtems_name        name;
    rtems_id          period;
    rtems_status_code status;
    name = rtems_build_name( 'P', 'E', 'R', '1' );
    status = rtems_rate_monotonic_create( name, &period );

    if ( status != 0 ) {
        printf( "rtems_monotonic_create failed with status of %d.\n", status );
        exit( 1 );
    }
    while ( 1 ) {
        if ( rtems_rate_monotonic_period( period, ticks_per_sec ) == RTEMS_TIMEOUT )
            break;

        clock_gettime( CLOCK_REALTIME, &start );

        /* Print the cpu number and task name */
        locked_printf(
            "HELLO WORLD CPU %lu running task %s @ %s:%ld\n",
            rtems_get_current_processor(),
            rtems_object_get_name( task_param[arg].id, sizeof(buffer), buffer ),
            my_ctime(start.tv_sec), start.tv_nsec) ;

        /* Perform some periodic actions */
    }

    /* missed period so delete period and SELF */
    status = rtems_rate_monotonic_delete( period );
    if ( status != 0 ) {
        printf( "rtems_rate_monotonic_delete failed with status of %d.\n", status );
        exit( 1 );
    }
    status = rtems_task_delete( RTEMS_SELF );    /* should not return */
    printf( "rtems_task_delete returned with status of %d.\n", status );
    exit( 1 );
}

void entry_point(void) 
{
    rtems_status_code status;    // status of rtems function

    locked_print_initialize();

    for (int i=0; i <= 1; i++ ) 
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
