
/* ***********************************************************************    */
/* ** TEST-DEF-00012 / 12 *********************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0120                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Repeat test TEST-DEF-00009 with more than one execution window for
	partition 0. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P1testdef.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************    */
int hm_was_called = 0;
int mtf_ticks     = 50;

/* Test auxiliary functions    ********************************************	*/

/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id,air_error_e i_error) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    
   
	hm_was_called = i_error;
    
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
	                              
    /* Test Steps *******************************************************    */
	/* since we are measuring execution windows, we should not use the
	 * current one, since it was already partially occupied by the 
	 * partition rtems boot-up, so let's wait until the next one	*/
	rtems_task_wake_after(mtf_ticks >> 1); 	
	
    /* Test Step 1 
    	Call air_syscall_set_schedule for scheedule 1, and check that a 
		PMK_VIOL_ERR fault occured, that was captured by the partition
		callback. */
    test_step_announce(1,1);

    
	/* Test step 1 code */
    air_syscall_set_schedule(1);

    /* EXPECTED: */
    if (hm_was_called == PMK_VIOL_ERR) {
		hm_was_called = 0;
        res &=    test_report(__FILE__, __LINE__,    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &=     test_step_report(TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    hm_was_called);

		test_exit(TEST_FAILURE,mtf_ticks >> 1);
    }

    /* Test End */
    test_return();
    return 0;
}

