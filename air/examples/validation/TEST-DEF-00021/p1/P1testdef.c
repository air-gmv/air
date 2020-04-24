
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

#include <P1testdef.h>

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
                                        
    /* Test Steps *******************************************************    */

	/* Reboot monitoring */
	reboot_count += 1;
	if (reboot_count > 0) pal_pprint_enter_part(1,NULL);


	/* Test step 3 code */
	rc = TSAL_INIT();

	if (reboot_count == 0)  {
		/* Test Step 3 
			Initialize P1 TSAL. */
		test_step_announce(3,1);

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

		/* Test Step 12 
			Set partition mode to normal (via SET_PARTITION_MODE); trans:
		COLD_START->NORMAL. */
		test_step_announce(12,1);

		/* Test step 12 code */

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

	} else if (reboot_count == 2) {

		/* Test Step 24 
			Set partition mode to normal (via SET_PARTITION_MODE); trans:
		WARM_START->NORMAL. */
		test_step_announce(24,1);

		/* Test step 24 code */
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

	} else if (reboot_count == 3) {

		/* Test Step 30 
			Set partition mode to NORMAL (via SET_PARTITION_MODE); trans:
		COLD_START->NORMAL (repeated). */
		test_step_announce(30,1);

		/* Test step 30 code */
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

	} else if (reboot_count == 4) {

		/* Test Step 38 
			Set up the transitions to idle -- 1: Set partition mode to NORMAL
		(via SET_PARTITION_MODE); trans: COLD_START->NORMAL (repeated). */
		test_step_announce(38,1);

		/* Test step 38 code */
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

		for (i=0; i<100; i++){
			rtems_task_wake_after(mtf_ticks);
		}

	}

    /* Test End */
    test_return();
    return 0;
}


