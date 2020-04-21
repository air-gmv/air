
/* ***********************************************************************    */
/* ** TEST-DEF-00022 / 22 *********************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0530                                                            */
/*    AIR_SEP_0540                                                            */
/*    AIR_SEP_0550                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Define a test with two non-system partitions where one calls the
	SET_A_PARTITION_MODE service with the PARTITION_IDENTIFIER of the other. Check that
	INVALID_CONFIG is returned and the target partition mode remains unchanged. */

#include <rtems.h>
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P1testdef.h>

#include <imaspex.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 60;
int reboot_count	= -1; 	/*  Internal state for mode change monitoring */

/* Test auxiliary functions    ********************************************	*/

/* Test HM callbacks        ***********************************************	*/
void hm_part_callback (pmk_hm_err_id_e i_error,void *i_state) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    pmk_hm_state_id_e state = (pmk_hm_state_id_e) i_state;
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
    
    /* Test specific variables  ******************************************	*/

    RETURN_CODE_TYPE		rc 			= -1;
    int						test_err	= 0;
    PARTITION_ID_TYPE		P0id		= 0;
    PARTITION_STATUS_TYPE	status;
                                        
    /* Test Steps *******************************************************    */

	/* Reboot monitoring */
	reboot_count += 1;
	if (reboot_count > 0) pal_pprint_enter_part(1,NULL);

    rc = TSAL_INIT();

	if (reboot_count == 0) {
		/* Test Step 1 
			TSAL_INIT; set partition mode to NORMAL; expect NO_ERROR. */
		test_step_announce(1,1);

		/* Test step 1 code */

		test_err = 0;

		if (rc != NO_ERROR){
			test_err = 1;
		}

		rc = -1;
		SET_PARTITION_MODE(NORMAL, &rc);

		/* EXPECTED: */
		if ((0 == test_err) && (NO_ERROR == rc) && (0 == unexp_error))  {
			res &= test_step_report(    TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_step_report(    TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										ret);
		}
		rtems_task_wake_after(mtf_ticks*2);
	
	} else if (reboot_count == 1) {

		/* Test Step 9 
			Set self mode to NORMAL. */
		test_step_announce(9,1);

		/* Test step 9 code */

		rc = -1;
		status.OPERATING_MODE = -1;

		GET_PARTITION_STATUS(&status, &rc);
		if ((status.OPERATING_MODE != WARM_START) || NO_ERROR != rc){
			test_err = 1;
		}

		rc = -1;
		SET_PARTITION_MODE(NORMAL, &rc);

		/* EXPECTED: NO_ERROR */
		if ((NO_ERROR == rc) && (0 == unexp_error))  {
			res &= test_step_report(    TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_step_report(    TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										ret);
		}

		/* Test Step 10
			Attempt to set partition P0 mode to WARM_START (a valid transition);
		expect INVALID_CONFIG -- the caller partition is not a system partition. */
		test_step_announce(10,1);

		/* Test step 10 code */

		rc = -1;
		SET_A_PARTITION_MODE(P0id, WARM_START, &rc);

		/* EXPECTED: */
		if ((INVALID_CONFIG == rc) && (0 == unexp_error))  {
			res &= test_step_report(    TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {
			res &= test_step_report(    TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										ret);
		}
	
	}

    /* Test End */
    test_exit(res,mtf_ticks);
    return 0;
}


