/*
 *  Partition Initialization
 */

#include <bsp.h>
% if "RTEMS481" in personality:	
#include <stdlib.h>
#include <stdio.h>
% endif

#include <pal.h>
#include <pmk_plugin.h>

${init_func_include}

/* Kernel Threads / Main Init launcher */
rtems_task Init (rtems_task_argument ignored) {
	rtems_status_code 	rc;

% if "AIRPEX" in personality:
	/* start kernel thread */
	pmk_plugin_kernel_thread_launch( );
	/* configure granularity and mtf	*/
	apx_usr_set_grain	   	(${ticks_per_ns}ll);		
	apx_usr_set_parperiod	(   ${mtf_in_ns}ll);
	apx_pos_start();
% else:
	% if init_func:
	${init_func}();
	% endif
% endif

	rc = rtems_task_delete( RTEMS_SELF );

	/* SKY 2010/04 : Dead Code to avoid code removal : TODO review this */
	rc = rtems_task_suspend		( RTEMS_SELF );
	rc = rtems_task_is_suspended( RTEMS_SELF );
	rc = rtems_task_resume		( RTEMS_SELF );
	
% if "RTEMS481" in personality:	
	/* SKY 2011/01 DUMB CALLS TO SOLVE LINKING PROBLEMS */
	(void) fstat	(0,NULL);
	(void) isatty	(0);
	(void) read		(0,NULL,0);
	(void) _read_r	(0,NULL,0);
	(void) write	(0,NULL,0);
	(void) _write_r	(0,NULL,0);	
	(void) lseek	(0,NULL,0);
% endif
}


/* configuration information */

#define CONFIGURE_MAXIMUM_TASKS 			15
#define CONFIGURE_MAXIMUM_SEMAPHORES		15

#define CONFIGURE_MAXIMUM_POSIX_THREADS		24

#include "../config/usr_rtems_config.h"

#include <rtems/confdefs.h>
