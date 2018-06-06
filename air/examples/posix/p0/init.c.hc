
/*
 *  COPYRIGHT (c) GMV
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern int main() __attribute__ ((weak));

void *POSIX_Init(
  void *argument
)
{
   main();
}

/* configuration information */
#define CONFIGURE_MAXIMUM_POSIX_THREADS     10
#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_MICROSECONDS_PER_TICK 5000

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
