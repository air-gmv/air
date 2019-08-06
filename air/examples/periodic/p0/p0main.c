/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2008-2013
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file 
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE. 
 * ==========================================================================*/
 
#include <rtems.h>
 
#include <rtems/rtems/tasks.h> 
#include <rtems/rtems/sem.h> 
#include <rtems/rtems/clock.h> 
#include <rtems/cpuuse.h>  
#include <a653.h>
#include <imaspex.h>


SAMPLING_PORT_ID_TYPE SEND_PORT;

/*---------------------------------------------------------	
 *		function: test										*
 *			outputs a simple string via a sampling port		* 
------------------------------------------------------------*/

void test2(PARTITION_ID_TYPE self_id) {
  rtems_name        name_1, name_2;
  rtems_id          period_1, period_2;
  rtems_status_code status;

  name_2 = rtems_build_name( 'P', 'E', 'R', '2' );
  name_1 = rtems_build_name( 'P', 'E', 'R', '1' );

  status=  rtems_rate_monotonic_create( name_1, &period_1 );
  status=  rtems_rate_monotonic_create( name_2, &period_2 );
  	    if( RTEMS_SUCCESSFUL != status ) {
			printf("create failed with status: %d\n", status);

      }

  while ( 1 ) {


      status = rtems_rate_monotonic_period( period_1, 100 );
	    if( RTEMS_SUCCESSFUL != status ) {
			printf("RM failed with status: %d\n", status);

      }

	  status = rtems_rate_monotonic_period( period_2, 40 );
	    if( RTEMS_SUCCESSFUL != status ) {
			printf("RM failed with status: %d\n", status);

      }
      

	 printf("  *  Perform first set of actions between clock     *  ticks 0 and 39 of every 100 ticks.     \n");

    status = rtems_rate_monotonic_period( period_2, 30 );
	    if( RTEMS_SUCCESSFUL != status ) {
			printf("RM2 failed with status: %d\n", status);
      }

    printf("  Perform second set of actions between clock 40 and 69 iss the period_2 period.     \n");


    (void) rtems_rate_monotonic_cancel( period_2 );
  }

  /* missed period so delete period and SELF */

  (void ) rtems_rate_monotonic_delete( period_2 );
 // (void ) task_delete( SELF );

}


int producer() {

	RETURN_CODE_TYPE rc;

	rtems_name 	name 		= 10000;
	rtems_mode	mode		= RTEMS_PREEMPT ;
	rtems_mode	mode_mask	= RTEMS_PREEMPT_MASK;
	rtems_id	id;
	
	PARTITION_ID_TYPE self_id;
	
	
	/*Getting my own partition id*/
	GET_PARTITION_ID(&self_id, &rc);
	if(NO_ERROR != rc) {
		//printf("GET_PARTITION_ID error %d\n", rc);
	}
	
	//for (;;)
	//{
	  printf("In partition PO loop t20\n");
	  rtems_task_wake_after(20);
  	  printf("P0 : Waking up\n");
	//}
	
	/*Creating Source sampling Port */
	SAMPLING_PORT_NAME_TYPE NAME = "ssampling";
	MESSAGE_SIZE_TYPE SIZE = 1024;
	SYSTEM_TIME_TYPE PERIOD= 1000000000ll;
	
	CREATE_SAMPLING_PORT (NAME, SIZE, SOURCE, PERIOD, &SEND_PORT, &rc);
	if (NO_ERROR != rc) {
		printf("CREATE_SAMPLING_PORT error %d\n", rc);
	}
	
	
	if (RTEMS_SUCCESSFUL == rtems_task_create (name, 15, 4096, mode, mode_mask, &id)) {
		rtems_task_start (id, test2, self_id);
	}
	
	SET_PARTITION_MODE(NORMAL, &rc);
	if (NO_ERROR != rc) {
		printf("SET_PARTITION_MODE error %d\n", rc);
	}
	
	return RTEMS_SUCCESSFUL;
}


