
/*
 *  COPYRIGHT (c) GMV
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

#include <pthread.h>

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *TestThread(void *argument);
void *TestThread2(void *argument);

pthread_t ThreadId ,ThreadId2;

void *TestThread(
  void *argument
)
{
  printf ("Pthread1 1 1 1 1 running!\n");
  return NULL;
}

void *TestThread2(
  void *argument
)
{
  printf ("Pthread2 2 2 2 2 running!\n");
  return NULL;
}



void *POSIX_Init(
  void *argument
)
{
  int status = 0;
   printf ("Pthread test2!\n");
   status = pthread_create( &ThreadId, NULL, TestThread, NULL );
//  status = pthread_create( &ThreadId2, NULL, TestThread2, NULL );

}

/* configuration information */
#define CONFIGURE_MAXIMUM_POSIX_THREADS     10
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
//#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER




#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
//#define CONFIGURE_MAXIMUM_TASKS 4
#define CONFIGURE_MAXIMUM_TIMERS 1


#define CONFIGURE_INIT

#include <rtems/confdefs.h>
