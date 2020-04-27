
/* ***********************************************************************    */
/* ** TEST-DEF-01650 / 1650 ********************************************* */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0430                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Check that the SET_PARTITION_MODE call is unable to set the mode of a
	partition to a value different of NORMAL, IDLE, WARM START or COLD START: it should
	return an INVALID_PARAM value and perform no action. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P0testdef.h>

#include <imaspex.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 60;
int reboot_count	= -1; 	/*  Internal state for mode change monitoring */

RETURN_CODE_TYPE rc = -1;

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
    
    /* Test specific variables  ******************************************	*/
	
    PARTITION_STATUS_TYPE status;

    if (reboot_count < 0)
		/* Test Start ******************************************************    */
		test_enter(1650);
                                        
    /* Test Steps *******************************************************    */

	/* Reboot monitoring */
	reboot_count += 1;

	if (reboot_count > 0) pal_pprint_enter_part(0,NULL);

	rc = TSAL_INIT();


	if (reboot_count == 0) {

		/* Test Step 0 
			TSAL_INIT. Expect NO_ERROR. */
		test_step_announce(0,1);

		/* EXPECTED: NO_ERROR */
		if ((rc == NO_ERROR) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
		} else {    
		    res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
		                                RESULT_DIFF | RESULT_TYPE_VALUE,
		                                ret);
		}

		/* Test Step 1 
			Obtain partition operating mode. Expect OPERATING_MODE ==
		COLD_START. */
		test_step_announce(1,1);

		/* Test step 1 code */

		rc 						= -1;
		status.OPERATING_MODE 	= -1;

		GET_PARTITION_STATUS(&status, &rc);

		/* EXPECTED: NO_ERROR and OPERATING_MODE == COLD_START */
		if (	(COLD_START == status.OPERATING_MODE) 	&&
				(rc == NO_ERROR) 						&&
				(0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
		} else {    
		    res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
		                                RESULT_DIFF | RESULT_TYPE_VALUE,
		                                ret);
		}

		/* Test Step 2 
			Set partition mode to (IDLE-1) as per the values defined in the
		standard OPERATING_MODE_TYPE enumeration. Expect INVALID_PARAM return code. */
		test_step_announce(2,1);

		/* Test step 2 code */

		rc = -1;

		SET_PARTITION_MODE((IDLE-1), &rc);

		/* EXPECTED: INVALID PARAM */
		if ((rc == INVALID_PARAM) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
		} else {    
		    res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
		                                RESULT_DIFF | RESULT_TYPE_VALUE,
		                                ret);
		}

		/* Test Step 3 
			Set partition mode to (NORMAL+1) as per the values defined in the
		standard OPERATING_MODE_TYPE enumeration. Expect INVALID_PARAM return code. */
		test_step_announce(3,1);

	    /* Test step 3 code */

		rc = -1;

		SET_PARTITION_MODE((NORMAL+1), &rc);

		/* EXPECTED: INVALID_PARAM */
		if ((rc == INVALID_PARAM) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
		} else {    
		    res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
		                                RESULT_DIFF | RESULT_TYPE_VALUE,
		                                ret);
		}

		/* Test Step 4 
			Obtain partition operating mode. Expect OPERATING_MODE == COLD_START
		to verify that operating mode wasn't changed as a result of steps 2 and
		3. */
		test_step_announce(4,1);

		/* Test step 4 code */
		rc 						= -1;
		status.OPERATING_MODE 	= -1;

		GET_PARTITION_STATUS(&status, &rc);

		/* EXPECTED: NO_ERROR and OPERATING_MODE == COLD_START */
		if (	(COLD_START == status.OPERATING_MODE) 	&&
				(rc == NO_ERROR) 						&&
				(0 == unexp_error))  {
	        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
		} else {    
		    res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
		                                RESULT_DIFF | RESULT_TYPE_VALUE,
		                                ret);
		}

		/* Test Step 5 
			Set partition mode to NORMAL. Expect NO_ERROR. */
		test_step_announce(5,1);

	    /* Test step 5 code */

		rc = -1;

		SET_PARTITION_MODE(NORMAL, &rc);

		/* EXPECTED: NO_ERROR*/
		if ((rc == NO_ERROR) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
		} else {    
		    res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
		                                RESULT_DIFF | RESULT_TYPE_VALUE,
		                                ret);
		}
		/* WAIT MTF */

		rtems_task_wake_after(mtf_ticks);

		/* Test step 12 code */
		/* step announcement comes after restart */

		SET_PARTITION_MODE(WARM_START, &rc);

	}	/* reboot count == 0*/

	if (reboot_count == 1) {

		/* Test Step 12 
			Previously set partition mode to WARM_START. Obtain partition
		operating mode and expect OPERATING_MODE == WARM_START. */
		test_step_announce(12,1);

		/* clean up the variables to be verified */
	    rc 						= -1;
		status.OPERATING_MODE 	= -1;

		/* obtain mode in partition status */
	    GET_PARTITION_STATUS(&status, &rc);

	    /* EXPECTED: NO_ERROR, operating mode == WARM_START */
	    if ((NO_ERROR == rc) && (status.OPERATING_MODE == WARM_START) && (0 == unexp_error))  {
	        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
	                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
	                                    ret);
	    } else {
	        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
	                                    RESULT_DIFF | RESULT_TYPE_VALUE,
	                                    ret);
	    }

	    /* WAIT MTF */
	    rtems_task_wake_after(mtf_ticks);

		/* Test step 14 code */
		/* step announcement comes after restart */

		SET_PARTITION_MODE(COLD_START, &rc);

	}	/* reboot count == 1*/

	if (reboot_count == 2) {

		/* Test Step 14 
			Previously set partition mode to COLD_START. Obtain partition
		operating mode and expect OPERATING_MODE == COLD_START. */
		test_step_announce(14,1);

		/* Test step 14 code */

		/* clean up the variables to be verified */
	    rc 						= -1;
		status.OPERATING_MODE 	= -1;

		/* obtain mode in partition status */
	    GET_PARTITION_STATUS(&status, &rc);

	    /* EXPECTED: NO_ERROR, operating mode == COLD_START */
	    if ((NO_ERROR == rc) && (status.OPERATING_MODE == COLD_START) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
		} else {    
		    res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
		                                RESULT_DIFF | RESULT_TYPE_VALUE,
		                                ret);
		}
		rtems_task_wake_after(mtf_ticks);

    	/* Test Step 16 
    	P1 previously sets it's own partition mode to IDLE. Obtain P1
		partition operating mode and expect OPERATING_MODE == IDLE. Test ends. */
    	test_step_announce(16,1);

	    /* Test step 16 code */

		/* clean up the variables to be verified */
	    rc 						= -1;
		status.OPERATING_MODE 	= -1;

		/* obtain mode in partition status */
	    GET_A_PARTITION_STATUS(1, &status, &rc);

	    /* EXPECTED: NO_ERROR, operating mode == IDLE */
	    if ((NO_ERROR == rc) && (status.OPERATING_MODE == IDLE) && (0 == unexp_error))  {
        	res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
        	                            RESULT_EQUAL | RESULT_TYPE_VALUE,
        	                            ret);
		} else {    
		    res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
		                                RESULT_DIFF | RESULT_TYPE_VALUE,
		                                ret);
		}

		/* sure that print partition gets all prints done */
		rtems_task_wake_after(mtf_ticks*2);
		
		/* Test End */
		test_exit(res,mtf_ticks);

	}	/* reboot count == 2 */
    return 0;
}


