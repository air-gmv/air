
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
#include <P3testdef.h>

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
	
    int 					test_err				= -1;
    RETURN_CODE_TYPE 		rc 						= -1;
    SCHEDULE_NAME_TYPE 		altsched_name 			= "alt_sched";
    SCHEDULE_ID_TYPE 		altsched_id 			= -1;
    SYSTEM_TIME_TYPE		expected_switch_time_ns = -1;
    SCHEDULE_STATUS_TYPE 	curr_sched_status;
                                        
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 23 
    	TSAL_INIT; set partition mode to NORMAL; expect NO_ERROR. */
    test_step_announce(23,1);

    /* Test step 23 code */
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

    /* Test Step 28 
    	Obtain schedule id of the alternative schedule; expect NO_ERROR. */
    test_step_announce(28,1);

    /* Test step 28 code */

    rc = -1;
    GET_MODULE_SCHEDULE_ID(altsched_name, &altsched_id, &rc);

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

    /* Test Step 29 
    	(Schedule change should be effective) Obtain schedule status. Expect
	NO_ERROR. */
    test_step_announce(29,1);

    /* Test step 29 code */
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

    /* Test Step 30 
    	(Schedule change should be effective) Verify: current schedule id ==
	alternative schedule id. */
    test_step_announce(30,1);

    /* Test step 30 code */

    /* EXPECTED: */
    if ((altsched_id == curr_sched_status.CURRENT_SCHEDULE) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 31 
    	(Schedule change should be effective) Verify: last sched switch ==
	180 ticks * ms_per_tick * 1000 (the abs time of the schedule switch). */
    test_step_announce(31,1);

    /* Test step 31 code */
    expected_switch_time_ns = mtf_ticks * 3 * pmk_get_usr_ms_per_tick() * 1000;

    /* EXPECTED: */
    if ((expected_switch_time_ns == curr_sched_status.TIME_OF_LAST_SCHEDULE_SWITCH) &&
    		(0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    rtems_task_wake_after(mtf_ticks);
    
    /* Test End */
    test_exit(res,mtf_ticks);
    return 0;
}


