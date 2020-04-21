
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
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P0testdef.h>

#include <imaspex.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 60;

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
	
    int 					test_err			= -1;
    RETURN_CODE_TYPE 		rc 					= -1;
    SCHEDULE_NAME_TYPE 		altsched_name 		= "alt_sched";
	SCHEDULE_NAME_TYPE 		invalidsched_name 	= "invalid_sched";
    SCHEDULE_ID_TYPE 		altsched_id 		= -1;
    SCHEDULE_ID_TYPE 		invalidsched_id 	= -1;
    SCHEDULE_STATUS_TYPE 	curr_sched_status;

	
    /* Test Start ******************************************************    */
    test_enter(1750);
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	TSAL_INIT; set partition mode to NORMAL; expect NO_ERROR. */
    test_step_announce(0,1);

    /* Test step 0 code */
    test_err = 0;
    rc = TSAL_INIT();

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
    rtems_task_wake_after(mtf_ticks);

    /* Test Step 2 
    	Attempt to obtain a schedule id using a name that doesn't identify
	an existing schedule; expect INVALID_CONFIG. */
    test_step_announce(2,1);

    /* Test step 2 code */

    rc = -1;
	GET_MODULE_SCHEDULE_ID(invalidsched_name, &altsched_id, &rc);

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

    /* Test Step 3 
    	Obtain schedule id of the alternative schedule; expect NO_ERROR. */
    test_step_announce(3,1);

    /* Test step 3 code */
    rc = -1;
    GET_MODULE_SCHEDULE_ID(altsched_name, &altsched_id, &rc);

    /* EXPECTED: */
    if ((NO_ERROR == rc) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 4 
    	Obtain schedule status; expect NO_ERROR. */
    test_step_announce(4,1);

    /* Test step 4 code */
    /* cleanup the relevant fields */
    curr_sched_status.CURRENT_SCHEDULE 				= -1;
    curr_sched_status.NEXT_SCHEDULE 				= -1;
    curr_sched_status.TIME_OF_LAST_SCHEDULE_SWITCH	= -1;
    rc = -1;

    GET_MODULE_SCHEDULE_STATUS(&curr_sched_status, &rc);

    /* EXPECTED: */
    if ((NO_ERROR == rc) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 5 
    	Verify: current schedule id != alternative schedule id. */
    test_step_announce(5,1);

    /* Test step 5 code */

    /* EXPECTED: */
    if ((altsched_id != curr_sched_status.CURRENT_SCHEDULE) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 6 
    	Verify: next schedule id == current (there is no sched pending). */
    test_step_announce(6,1);

    /* Test step 6 code */

    /* EXPECTED: */
    if ((curr_sched_status.CURRENT_SCHEDULE == curr_sched_status.NEXT_SCHEDULE) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
	rtems_task_wake_after(mtf_ticks);

    /* Test Step 12 
    	Attempt to set module schedule using an invalid schedule id; expect
	INVALID_PARAM. */
    test_step_announce(12,1);

    /* Test step 12 code */
    
	rc = -1;
    SET_MODULE_SCHEDULE(invalidsched_id, &rc);

    /* EXPECTED: */
    if ((INVALID_PARAM == rc) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 13 
    	Set module schedule to the alternative schedule; expect NO_ERROR. */
    test_step_announce(13,1);

    /* Test step 13 code */
    
    SET_MODULE_SCHEDULE(altsched_id, &rc);

    /* EXPECTED: */
    if ((NO_ERROR == rc) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    rc);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    rc);
    }

    /* Test Step 14 
    	(On the same MTF of the schedule change call) Obtain schedule
	status. Expect NO_ERROR. */
    test_step_announce(14,1);

    /* Test step 14 code */
    /* cleanup the relevant fields */
    curr_sched_status.CURRENT_SCHEDULE 				= -1;
    curr_sched_status.NEXT_SCHEDULE 				= -1;
    curr_sched_status.TIME_OF_LAST_SCHEDULE_SWITCH 	= -1;
    rc = -1;

    GET_MODULE_SCHEDULE_STATUS(&curr_sched_status, &rc);

    /* EXPECTED: */
    if ((NO_ERROR == rc) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 15 
    	(On the same MTF of the schedule change call) Verify: current
	schedule id != alternative schedule id. */
    test_step_announce(15,1);

    /* Test step 15 code */

    /* EXPECTED: */
    if ((altsched_id != curr_sched_status.CURRENT_SCHEDULE) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 16 
    	(On the same MTF of the schedule change call) Verify: next schedule
	id == alternate sched id (sched is pending). */
    test_step_announce(16,1);

    /* Test step 16 code */

    /* EXPECTED: */
    if ((altsched_id == curr_sched_status.NEXT_SCHEDULE) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 17 
    	(On the same MTF of the schedule change call) Verify: last sched
	switch == 0 (no sched switch has occurred). */
    test_step_announce(17,1);

    /* Test step 17 code */

    /* EXPECTED: */
    if ((0 == curr_sched_status.TIME_OF_LAST_SCHEDULE_SWITCH) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    
    /* Test End */
    test_return();
    return 0;
}


