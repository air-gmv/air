
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

#include <P0testdef.h>

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
	
    RETURN_CODE_TYPE 		rc		= -1;
    PARTITION_ID_TYPE		p2id 	= -1;
    int 				test_err	= 0;
    SCHEDULE_ID_TYPE	altsched	= -1;
    PARTITION_STATUS_TYPE 	selfstatus, p2status;


    /* Test Start ******************************************************    */
    test_enter(1390);
                                        
    /* Test Steps *******************************************************    */

    /* Test Step 0
    	Verify partition mode is the initial mode (COLD_START). */
    test_step_announce(0,1);

    /* Test step 0 code */

	/* clean up the variables to be verified */
    rc 						= -1;
	selfstatus.OPERATING_MODE 	= -1;

	/* obtain mode in partition status */
    GET_PARTITION_STATUS(&selfstatus, &rc);

    /* EXPECTED: NO_ERROR, operating mode == COLD_START */
    if ((NO_ERROR == rc) && (selfstatus.OPERATING_MODE == COLD_START) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 1 
    	Set partition mode to NORMAL; expect NO_ERROR. */
    test_step_announce(1,1);

    /* Test step 1 code */

	/* clean up the variables to be verified */
    rc 						= -1;
	selfstatus.OPERATING_MODE 	= -1;

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


    /* Test Step 4 
    	Verify partition mode is NORMAL. */
    test_step_announce(4,1);

    /* Test step 4 code */

	/* clean up the variables to be verified */
    rc 						= -1;
	selfstatus.OPERATING_MODE 	= -1;

	/* obtain mode in partition status */
    GET_PARTITION_STATUS(&selfstatus, &rc);

    /* EXPECTED: NO_ERROR, operating mode == COLD_START */
    if ((NO_ERROR == rc) && (selfstatus.OPERATING_MODE == NORMAL) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    rtems_task_wake_after(mtf_ticks);


    /* Test Step 6 
    	Verify yet unscheduled partition p2 mode is COLD_START. */
    test_step_announce(6,1);

    /* Test step 6 code */

	/* clean up the variables to be verified */
    rc 						= -1;
	p2status.OPERATING_MODE	= -1;

	GET_A_PARTITION_ID("p2", &p2id, &rc);
	if (NO_ERROR != rc){
		test_err = 1;
	}

	rc = -1;

	/* obtain mode in partition status */
    GET_A_PARTITION_STATUS(p2id, &p2status, &rc);

    /* EXPECTED: NO_ERROR, operating mode == COLD_START */
    if (	(0 == test_err) 					&&
    		(NO_ERROR == rc) 					&&
    		(p2status.OPERATING_MODE == COLD_START)	&&
    		(0 == unexp_error))  {

    	res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 9 
    	SET_MODULE_SCHEDULE to the second schedule, the one including
	partition p2. */
    test_step_announce(7,1);

    /* Test step 9 code */

	/* clean up the variables to be verified */
    rc 			= -1;
    test_err 	= 0;


    GET_MODULE_SCHEDULE_ID("test_sched2", &altsched, &rc);
    if (NO_ERROR != rc){
    	test_err = 1;
    }

    rc = -1;
    SET_MODULE_SCHEDULE(altsched, &rc);

    /* EXPECTED: schedule id obtained, and module schedule changed with NO_ERR */
    if ((NO_ERROR == rc) && (test_err == 0) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    rtems_task_wake_after(mtf_ticks);


    /* Test Step 8 
    	Verify partition mode is NORMAL. */
    test_step_announce(8,1);

    /* Test step 8 code */

	/* clean up the variables to be verified */
    rc 							= -1;
	selfstatus.OPERATING_MODE	= -1;

	/* obtain mode in partition status */
    GET_PARTITION_STATUS(&selfstatus, &rc);

    /* EXPECTED: NO_ERROR, operating mode == COLD_START */
    if ((NO_ERROR == rc) && (selfstatus.OPERATING_MODE == NORMAL) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    rtems_task_wake_after(mtf_ticks);

    
    /* Test End */
    test_exit(res,mtf_ticks);
    return 0;
}


