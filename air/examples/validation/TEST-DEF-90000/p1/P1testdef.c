
/* ***********************************************************************    */
/* ** TEST-DEF-90000 / 90000 ******************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0120                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Tests if a for(;;); executed by a timer hangs the system or not. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P1testdef.h>


/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 150;

/* Test auxiliary functions    ********************************************	*/
void timer();
void for_r( rtems_id timer_id, rtems_id id);

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

                                        
    /* Test Steps *******************************************************    */

	/* Test Step 6
		Test if the partition is blocked or not and sleeps */
	test_step_announce(6,1);

	/* Test step 6 code */
	ret |= RTEMS_SUCCESSFUL;

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
    
    rtems_task_wake_after(100);

    /* Test End */
    test_exit(res,mtf_ticks);
    return 0;
}


