
/* ***********************************************************************    */
/* ** TEST-DEF-01750 / 1750 ********************************************* */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0120                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Test the GET_MODULE_SCHEDULE_ID and GET_MODULE_SCHEDULE_STATUS functions
	work from a regular user mode partition without any special permissions, and
	that the returned values are correct before and after a call to
	SET_MODULE_SCHEDULE. Test this immediately after the call to SET_MODULE_SCHEDULE, and in the
	next Major Time Frame. */

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
	
    int 					test_err			= -1;
    RETURN_CODE_TYPE 		rc 					= -1;
    SCHEDULE_NAME_TYPE 		altsched_name 		= "alt_sched";
	SCHEDULE_NAME_TYPE 		invalidsched_name 	= "invalid_sched";
    SCHEDULE_ID_TYPE 		altsched_id 		= -1;
    SCHEDULE_STATUS_TYPE 	curr_sched_status;
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 1 
    	TSAL_INIT; set partition mode to NORMAL; expect NO_ERROR. */
    test_step_announce(1,1);

    /* Test step 1 code */
    test_err = 0;
    rc = TSAL_INIT();

    if (rc != NO_ERROR){
    	test_err = 1;
    }

    rc = -1;
    SET_PARTITION_MODE(NORMAL, &rc);

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
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
    	Attempt to obtain a schedule id using a name that doesn't identify
	an existing schedule; expect INVALID_CONFIG. */
    test_step_announce(7,1);

    /* Test step 7 code */

    rc = -1;
	GET_MODULE_SCHEDULE_ID(invalidsched_name, &altsched_id, &rc);

    /* EXPECTED: */
    if ((INVALID_CONFIG == rc) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 8 
    	Obtain schedule id of the alternative schedule; expect NO_ERROR. */
    test_step_announce(8,1);

    /* Test step 8 code */
    rc = -1;
    GET_MODULE_SCHEDULE_ID(altsched_name, &altsched_id, &rc);

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

    /* Test Step 9 
    	Obtain schedule status; expect NO_ERROR. */
    test_step_announce(9,1);

    /* Test step 9 code */

    /* cleanup the relevant fields */
    curr_sched_status.CURRENT_SCHEDULE 	= -1;
    curr_sched_status.NEXT_SCHEDULE 	= -1;
    rc = -1;

    GET_MODULE_SCHEDULE_STATUS(&curr_sched_status, &rc);

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

    /* Test Step 10 
    	Verify: current schedule id != alternative schedule id. */
    test_step_announce(10,1);

    /* Test step 10 code */

    /* EXPECTED: */
    if ((altsched_id != curr_sched_status.CURRENT_SCHEDULE) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 11 
    	Verify: next schedule id == current (there is no sched pending). */
    test_step_announce(11,1);

    /* Test step 11 code */

    /* EXPECTED: */
    if ((curr_sched_status.CURRENT_SCHEDULE == curr_sched_status.NEXT_SCHEDULE) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
    rtems_task_wake_after(mtf_ticks);

    /* Test Step 18 
    	(On the same MTF of the schedule change call) Obtain schedule
	status. Expect NO_ERROR. */
    test_step_announce(18,1);

    /* Test step 18 code */
    curr_sched_status.CURRENT_SCHEDULE 				= -1;
    curr_sched_status.NEXT_SCHEDULE 				= -1;
    curr_sched_status.TIME_OF_LAST_SCHEDULE_SWITCH	= -1;
    rc = -1;

    GET_MODULE_SCHEDULE_STATUS(&curr_sched_status, &rc);

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

    /* Test Step 19 
    	(On the same MTF of the schedule change call) Verify: current
	schedule id != alternative schedule id. */
    test_step_announce(19,1);

    /* Test step 19 code */

    /* EXPECTED: */
    if ((altsched_id != curr_sched_status.CURRENT_SCHEDULE) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 20 
    	(On the same MTF of the schedule change call) Verify: next schedule
	id == alternate sched id (sched is pending). */
    test_step_announce(20,1);

    /* Test step 20 code */

    /* EXPECTED: */
    if ((altsched_id == curr_sched_status.NEXT_SCHEDULE ) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 21 
    	(On the same MTF of the schedule change call) Verify: last sched
	switch == 0 (no sched switch has occurred). */
    test_step_announce(21,1);

    /* Test step 21 code */

    /* EXPECTED: */
    if ((0 == curr_sched_status.TIME_OF_LAST_SCHEDULE_SWITCH) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    
    /* Test End */
    test_return();
    return 0;
}


