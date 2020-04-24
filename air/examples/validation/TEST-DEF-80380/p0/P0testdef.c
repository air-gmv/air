
/* ***********************************************************************    */
/* ** TEST-DEF-80380 / 80380 ******************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0340                                                            */
/*    AIR_SEP_0760                                                            */
/*    AIR_SEP_0880                                                            */
/*    AIR_SEP_0890                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Create a unit test for the pmk_sparc_enable_FP system call. Run the same
	code in a second non-authorized partition and check that a Health Monitor event
	is generated. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P0testdef.h>


/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int err_id 		= 0;
int mtf_ticks    	= 40;

/* Test auxiliary functions    ********************************************	*/

/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id,air_error_e i_error) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    
	/* signal error ocurrence	*/
    unexp_error	= 1;
	err_id = i_error;
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
	int rc;
	volatile float a;
	volatile float pi;
	
    /* Test Start ******************************************************    */
    test_enter(80380);
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	Call pmk_sparc_enable_FP and check that no Health Monitor event was
	generated. */
    test_step_announce(0,1);

    /* Test step 0 code */
    rc = pmk_sparc_enable_FP();

    /* EXPECTED: */
    if ((PAL_NO_ERROR == rc) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
	
	unexp_error = 0;
	err_id = 0;
	
    /* Test Step 1 
    	Perform a Floating Point operation and check that no Health Monitor
	event was generated. */
    test_step_announce(1,1);

    /* Test step 1 code */
    a = 2.25+2.75;

    /* EXPECTED: */
    if (0 == unexp_error)  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
	
	rtems_task_wake_after(mtf_ticks);
	
	unexp_error = 0;
	err_id = 0;
	
    /* Test Step 5 
    	Perform a Floating Point operation and check that no Health Monitor
	event was generated. */
    test_step_announce(5,1);

    /* Test step 5 code */
    pi = a*2.5;
	
    /* EXPECTED: */
    if ((12.5 == pi) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    
    /* Test End */
    test_exit(res,mtf_ticks);
    return 0;
}


