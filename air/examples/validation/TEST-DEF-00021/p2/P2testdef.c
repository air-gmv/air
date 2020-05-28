
/* ***********************************************************************    */
/* ** TEST-DEF-00021 / 21 *********************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0530                                                            */
/*    AIR_SEP_0540                                                            */
/*    AIR_SEP_0550                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Define a system with four partitions: * The first partition is a system
	partition, capable of calling SET_A_PARTITION_MODE to set the mode of another
	partition; this partition uses GET_A_PARTITION_STATUS to perform verification by
	checking the status of the target partitions. * Secondary (target) partitions are
	user partitions, where the mode changes will be applied; The test shall call
	SET_A_PARTITION_MODE in the first partition for each of the valid mode transitions indicated in
	AIR_SEP_0530 with the PARTITION_IDENTIFIER set to the target partitions. Invalid
	transitions (N/A) transitions are also tested for errors. On the following cycle the
	first partition shall check that a mode was either effectively changed or
	remained unchanged (for the error cases). */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P2testdef.h>

#include <imaspex.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 100;
int reboot_count	= -1; 	/*  Internal state for mode change monitoring */

/* Test auxiliary functions    ********************************************	*/

/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id,air_error_e i_error) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    
	/* signal error ocurrence	*/
    unexp_error	= 1;
    return;
}

/* Test execution *******************************************************    */
int test_main (void) {
    /* Test generic variables  ******************************************	*/
	/* repeatition iteration counter	*/ 
	int repeat     = 0;                
	/* function to test return code     */
    rtems_status_code ret    = RTEMS_SUCCESSFUL; 
	/* total test result                */
    int res     = TEST_SUCCESS;     
	/* Target partition iterator 		*/
	int i;
    
    /* Test specific variables  ******************************************	*/
	RETURN_CODE_TYPE rc = -1;
    
    /* Test Start ******************************************************    */
    test_enter(21);
    debug_libtest();
    /* Test Steps *******************************************************    */

	/* Reboot monitoring */
    air_status_code_e status_code;
    air_partition_status_t partition_status;
    status_code = air_syscall_get_partition_status(-1, &partition_status);
    //printf ("P2 REBOOTS %d\n", partition_status.restart_count);
    reboot_count = partition_status.restart_count;


	/* Test step 4 code */
	rc = TSAL_INIT();

	if (reboot_count == 0) {
		/* Test Step 4 
			Initialize P2 TSAL. */
		test_step_announce(4,1);

		/* EXPECTED: */
		if ((NO_ERROR == rc) && (0 == unexp_error))  {
			res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										ret);
		}
		
		for (i=0; i<2; i++){
			rtems_task_wake_after(mtf_ticks);
		}
	
	} else if (reboot_count == 1) {

		/* Test Step 13 
			Set partition mode to normal (via SET_PARTITION_MODE); trans:
		COLD_START->NORMAL. */
		test_step_announce(13,1);

		/* Test step 13 code */

		rc = -1;
		SET_PARTITION_MODE(NORMAL, &rc);

		/* EXPECTED: */
		if ((NO_ERROR == rc) && (0 == unexp_error))  {
			res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										ret);
		}

		for (i=0; i<3; i++){
			rtems_task_wake_after(mtf_ticks);
		}

	} else if ((reboot_count == 2) || (reboot_count == 3)){

		/* Test Step 25 
			Set partition mode to normal (via SET_PARTITION_MODE); trans:
		WARM_START->NORMAL. */
		test_step_announce(25,1);

		/* Test step 25 code */
		rc = -1;
		SET_PARTITION_MODE(NORMAL, &rc);

		/* EXPECTED: */
		if ((NO_ERROR == rc) && (0 == unexp_error))  {
			res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										ret);
		}

		for (i=0; i<2; i++){
			rtems_task_wake_after(mtf_ticks);
		}
	
	} else if (reboot_count == 4) {

		/* Test Step 31 
			Set partition mode to NORMAL (via SET_PARTITION_MODE); trans:
		COLD_START->NORMAL (repeated). */
		test_step_announce(31,1);

		/* Test step 31 code */
		rc = -1;
		SET_PARTITION_MODE(NORMAL, &rc);

		/* EXPECTED: */
		if ((NO_ERROR == rc) && (0 == unexp_error))  {
			res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										ret);
		}

		for (i=0; i<3; i++){
			rtems_task_wake_after(mtf_ticks);
		}
	
	} else if ((reboot_count == 5) || (reboot_count == 6)) {
	

		/* Test Step 39 
			Set up the transitions to idle -- 2: Set partition mode to NORMAL
		(via SET_PARTITION_MODE); trans: COLD_START->NORMAL (repeated). */
		test_step_announce(39,1);

		/* Test step 39 code */
		rc = -1;
		SET_PARTITION_MODE(NORMAL, &rc);

		/* EXPECTED: */
		if ((NO_ERROR == rc) && (0 == unexp_error))  {
			res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										ret);
		}

//		for (i=0; i<100; i++){
//			rtems_task_wake_after(mtf_ticks);
//		}
	}   else if (reboot_count == 7) {
    }
    /* Test End */
    test_finish(res);
    return 0;

}


