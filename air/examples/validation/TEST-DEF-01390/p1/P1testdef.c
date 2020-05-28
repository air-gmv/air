
/* ***********************************************************************    */
/* ** TEST-DEF-01390 / 1390 ********************************************* */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0560                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	AIR SEP 0560 "During the POS boot-up the partition mode shall be either
	COLD-START or WARM-START. If the system is starting (or rebooting) the mode shall be
	COLD-START. The partition is able to determine which through the GET_PARTITION_STATUS
	service." TEST-DEF Devise a test with three partitions, where the third is not
	included in an initial schedule. The second module schedule shall only assign one
	tick to the third partition, and the number of ticks per second shall be small
	(e.g. 1 ms/tick). Check that a call to GET_PARTITION_STATUS on the newly run
	partition (after a SET_MODULE_SCHEDULE function call) returns the status COLD_START. */

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
	
    RETURN_CODE_TYPE 		rc = -1;
    PARTITION_STATUS_TYPE 	status;
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 3 
    	TSAL_INIT; expect NO_ERROR. */
    test_step_announce(3,1);

    /* Test step 3 code */

    rc = TSAL_INIT();

    /* EXPECTED: NO_ERROR */
    if ((NO_ERROR == rc) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 4 
    	Verify partition mode is the initial mode (COLD_START). */
    test_step_announce(4,1);

    /* Test step 4 code */

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

    /* Test Step 5 
    	Set partition mode to NORMAL; expect NO_ERROR. */
    test_step_announce(5,1);

    /* Test step 5 code */
	/* clean up the variables to be verified */
    rc 						= -1;
	status.OPERATING_MODE 	= -1;

	/* obtain mode in partition status */
    SET_PARTITION_MODE(NORMAL, &rc);

    /* EXPECTED: NO_ERROR*/
    if ((NO_ERROR == rc) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    rtems_task_wake_after(mtf_ticks);


    /* Test Step 7 
    	Verify partition mode is NORMAL. */
    test_step_announce(7,1);

    /* Test step 7 code */

	/* clean up the variables to be verified */
    rc 						= -1;
	status.OPERATING_MODE 	= -1;

	/* obtain mode in partition status */
    GET_PARTITION_STATUS(&status, &rc);

    /* EXPECTED: NO_ERROR, operating mode == COLD_START */
    if ((NO_ERROR == rc) && (status.OPERATING_MODE == NORMAL) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    rtems_task_wake_after(mtf_ticks+mtf_ticks/2);


    /* Test Step 11 
    	Verify partition mode is NORMAL. */
    test_step_announce(11,1);

    /* Test step 11 code */

	/* clean up the variables to be verified */
    rc 						= -1;
	status.OPERATING_MODE 	= -1;

	/* obtain mode in partition status */
    GET_PARTITION_STATUS(&status, &rc);

    /* EXPECTED: NO_ERROR, operating mode == COLD_START */
    if ((NO_ERROR == rc) && (status.OPERATING_MODE == NORMAL) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test End */
    test_return(res);
    return 0;
}


