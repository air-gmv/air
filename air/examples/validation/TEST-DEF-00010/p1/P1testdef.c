
/* ***********************************************************************    */
/* ** TEST-DEF-00010 / 10 *********************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0120                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Call the SET_MODULE_SCHEDULE function from an authorized partition and
	check that the partitioning schedule is altered starting at the next major time
	frame (MTF) start (with GET_PARTITION_MODE). Check that a call to
	SET_MODULE_SCHEDULE with the current schedule produces no change. 
	Check that the call from a non-authorized partition fails with an 
	INVALID_CONFIG error */

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
int hm_was_called = 0;

/* Test auxiliary functions    ********************************************	*/

/* Test HM callbacks        ***********************************************	*/
void hm_part_callback (pmk_hm_err_id_e i_error,void *i_state) {
   
	hm_was_called = i_error;
    return;
}

/* Test execution *******************************************************    */
int test_main (void) {
    /* Test generic variables  ******************************************	*/
	/* function to test return code     */
    rtems_status_code ret    = RTEMS_SUCCESSFUL; 
	/* total test result                */
    int res     = TEST_SUCCESS;     
	/* wait ticks before test end to print results                    */
    int wait    = 50;			
    
	/* since we are measuring execution windows, we should not use the
	 * current one, since it was already partially occupied by the 
	 * partition rtems boot-up, so let's wait until the next one	*/
	rtems_task_wake_after(wait >> 1); 
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 1 
	   Call SET_MODULE_SCHEDULE for schedule 1, and check that INVALID_CONFIG
	   is returned, for operation not allowed.			*/
    test_step_announce(1,1);
    
    /* Test step 1 code */
	SCHEDULE_ID_TYPE sched_id;
	RETURN_CODE_TYPE rc;

	GET_MODULE_SCHEDULE_ID ("alternative_sched", &sched_id, &rc);
	ret |= rc;

	SET_MODULE_SCHEDULE(sched_id, &rc);
	ret |= rc;

    /* EXPECTED: */
    if (hm_was_called == 0 && rc == INVALID_CONFIG ) {
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

