/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright (C) 2018  GMVIS Skysoft S.A.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include <system.h>

#include <inttypes.h>

const char rtems_test_name[] = "SMP 1";

void Loop() {
  volatile int i;

  for (i=0; i<300000; i++);
}

void entry_point(
  void
)
{
  uint32_t           i;
  char               ch;
  uint32_t           cpu_self;
  rtems_id           id;
  rtems_status_code  status;
  bool               allDone;

  cpu_self = rtems_get_current_processor();

  /* XXX - Delay a bit to allow debug messages from
   * startup to print.  This may need to go away when
   * debug messages go away.
   */ 
  Loop();

  TEST_BEGIN();

  locked_print_initialize();

  /* Initialize the TaskRan array */
  for ( i=0; i<rtems_get_processor_count() ; i++ ) {
    TaskRan[i] = false;
  }
printf("Start tasks\n");
  /* Create and start tasks for each processor */
  for ( i=0; i< rtems_get_processor_count() ; i++ ) {
    if ( i != cpu_self ) {
      ch = '0' + i;

      status = rtems_task_create(
        rtems_build_name( 'T', 'A', ch, ' ' ),
        1,
        RTEMS_MINIMUM_STACK_SIZE,
        RTEMS_DEFAULT_MODES,
        RTEMS_DEFAULT_ATTRIBUTES,
        &id
      );
      directive_failed( status, "task create" );

      locked_printf(" CPU %" PRIu32 " start task TA%c\n", cpu_self, ch);
      status = rtems_task_start( id, Test_task, i+1 );
      directive_failed( status, "task start" );

      Loop();
    }
  }
  
  /* Wait on the all tasks to run */
  while (1) {
    allDone = true;
    for ( i=0; i<rtems_get_processor_count() ; i++ ) {
      if ( i != cpu_self && TaskRan[i] == false)
        allDone = false;
    }
    if (allDone) {
      TEST_END();
      //rtems_test_exit( 0 );
      air_syscall_shutdown_module();
    }
  }

}
