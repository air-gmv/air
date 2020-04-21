/*
 *  Dummy Init for RTEMS partition
 */

#include <rtems.h>
#include <bsp.h>

#include <rtems/rtems/tasks.h> 
#include <rtems/rtems/sem.h> 
#include <rtems/rtems/clock.h> 

#include <pal.h>
#include <pmk_plugin.h>

/* Kernel Threads / Main Init launcher */
rtems_task Init (rtems_task_argument ignored) {
	rtems_status_code 	rc;


	/* SKY 2010/04 : Dead Code to avoid code removal : TODO review this */
	rc = rtems_task_suspend( RTEMS_SELF );
	rc = rtems_task_is_suspended( RTEMS_SELF );
	rc = rtems_task_resume( RTEMS_SELF );  	
}


/* configuration information */

#define CONFIGURE_MAXIMUM_TASKS 			18
#define CONFIGURE_MAXIMUM_SEMAPHORES		19

#define CONFIGURE_MAXIMUM_POSIX_THREADS		24	//128

#include "../config/usr_rtems_config.h"

#include <rtems/confdefs.h>

