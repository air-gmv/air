
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

#include <P1testdef.h>

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
                                        
    /* Test Steps *******************************************************    */
	
	/* Reboot monitoring */
	reboot_count += 1;

	if (reboot_count > 0) pal_pprint_enter_part(1,NULL);

    rc = TSAL_INIT();

	if (reboot_count == 0) {
		/* Test Step 6 
			TSAL_INIT. Expect NO_ERROR. */
		test_step_announce(6,1);

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

		/* Test Step 7 
			Obtain partition operating mode. Expect OPERATING_MODE ==
		COLD_START. */
		test_step_announce(7,1);

		/* Test step 7 code */

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

		/* Test Step 8 
			Set partition mode to (IDLE-1) as per the values defined in the
		standard OPERATING_MODE_TYPE enumeration. Expect INVALID_PARAM return code. */
		test_step_announce(8,1);

		/* Test step 8 code */

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

		/* Test Step 9 
			Set partition mode to (NORMAL+1) as per the values defined in the
		standard OPERATING_MODE_TYPE enumeration. Expect INVALID_PARAM return code. */
		test_step_announce(9,1);

		/* Test step 9 code */

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

		/* Test Step 10 
			Obtain partition operating mode. Expect OPERATING_MODE == COLD_START
		to verify that operating mode wasn't changed as a result of steps 8 and
		9. */
		test_step_announce(10,1);

		/* Test step 10 code */

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

		/* Test Step 11 
			Set partition mode to NORMAL. Expect NO_ERROR. */
		test_step_announce(11,1);

		/* Test step 11 code */

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

		/* Test step 13 code */
		/* step announcement comes after restart */

		SET_PARTITION_MODE(WARM_START, &rc);

	}	/* reboot count == 0*/


	if (reboot_count == 1) {

		/* Test Step 13 
			Previously set partition mode to WARM_START. Obtain partition
		operating mode and expect OPERATING_MODE == WARM_START. */
		test_step_announce(13,1);

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

		rtems_task_wake_after(mtf_ticks);

		/* Test step 15 code */
		/* step announcement comes after restart */

		SET_PARTITION_MODE(COLD_START, &rc);

	}	/* reboot count == 1*/

    
	if (reboot_count == 2) {

		/* Test Step 15 
			Previously set partition mode to COLD_START. Obtain partition
		operating mode and expect OPERATING_MODE == COLD_START. */
		test_step_announce(15,1);

	    /* Test step 15 code */

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
    
	    /* Test Step 16 
	    	After partition becomes idle no more operations can be performed,
	    	test step 16 is implemented at P0 to verify the op mode of this
	    	partition */
	
	    SET_PARTITION_MODE(IDLE, &rc);
		
	}	/* reboot count == 2 */

    /* gcc likes this */
    return 0;
}


