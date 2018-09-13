/*  Main
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id: init.c,v 1.13 2003/09/04 18:53:41 joel Exp $
 */

#define CONFIGURE_INIT

#ifdef RTEMS48I
	#include <pprintf.h>
#else
#include <time.h>
#include <sys/time.h>
#include <rtems/score/timespec.h>

#include <stdio.h>
#include <string.h>
#include "rtems_config.h"
#endif

int andp=7;

static char *my_ctime( time_t t )
{
  static char b[32];
  ctime_r(&t, b);
  b[ strlen(b) - 1] = '\0';
  return b;
}


rtems_task Init(rtems_task_argument ignored) 
{
    #ifdef RTEMS_SMP
        printf( "\n\n*** RTEMS_SMP***\n" );
    #else
        printf( "\n\n*** RTEMS_SMP NOT DEFINED***\n" );
    #endif

	while(1)
	{

        #ifdef RTEMS48I
            pprintf( "\n\n*** RTEMS48I HELLO WORLD TEST **********\n" );
        #else
            struct timespec start;
            clock_gettime( CLOCK_REALTIME, &start );

            printf( "\n\n*** RTEMS5 HELLO WORLD TEST **********\n" );
            printf( "Time is : %s:%ld\n", my_ctime(start.tv_sec), start.tv_nsec);
        //    printf("TOTAL PROCESSORS %d\nCURRENT PROCESSOR %d\n",(int)rtems_get_processor_count(), (int)rtems_get_current_processor());
            
        #endif

        rtems_task_wake_after(10);

    }
}


