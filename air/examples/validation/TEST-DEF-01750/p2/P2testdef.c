
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

#include <P2testdef.h>

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
	
    int 					test_err				= -1;
    RETURN_CODE_TYPE 		rc 						= -1;
    SCHEDULE_NAME_TYPE 		altsched_name 			= "alt_sched";
    SCHEDULE_ID_TYPE 		altsched_id 			= -1;
    SYSTEM_TIME_TYPE		expected_switch_time_ns	= -1;
    SCHEDULE_STATUS_TYPE 	curr_sched_status;
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 22 
    	TSAL_INIT; set partition mode to NORMAL; expect NO_ERROR. */
    test_step_announce(22,1);

    /* Test step 22 code */
    test_err = 0;
    rc = TSAL_INIT();

    if (rc != NO_ERROR){
    	test_err = 1;
    }

    rc = -1;
    SET_PARTITION_MODE(NORMAL, &rc);

    /* EXPECTED: */
    if ((0 == test_err) && (NO_ERROR == rc) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
    rtems_task_wake_after(mtf_ticks);

    /* Test Step 24 
    	Obtain schedule id of the alternative schedule; expect NO_ERROR. */
    test_step_announce(24,1);

    /* Test step 24 code */

    rc = -1;
    GET_MODULE_SCHEDULE_ID(altsched_name, &altsched_id, &rc);

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

    /* Test Step 25 
    	(Schedule change should be effective) Obtain schedule status. Expect
	NO_ERROR. */
    test_step_announce(25,1);

    /* Test step 25 code */
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

    /* Test Step 26 
    	(Schedule change should be effective) Verify: current schedule id ==
	alternative schedule id. */
    test_step_announce(26,1);

    /* Test step 26 code */

    /* EXPECTED: */
    if ((altsched_id == curr_sched_status.CURRENT_SCHEDULE) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 27 
    	(Schedule change should be effective) Verify: last sched switch ==
	180 ticks * ms_per_tick * 1000 (the abs time of the schedule switch). */
    test_step_announce(27,1);

    /* Test step 27 code */
    expected_switch_time_ns = mtf_ticks * 3 * pmk_get_usr_ms_per_tick() * 1000;

    /* EXPECTED: */
    if ((expected_switch_time_ns == curr_sched_status.TIME_OF_LAST_SCHEDULE_SWITCH) &&
    		(0 == unexp_error))  {
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


