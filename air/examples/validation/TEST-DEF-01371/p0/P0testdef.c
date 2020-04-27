
/* ***********************************************************************    */
/* ** TEST-DEF-01371 / 1371 ********************************************* */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0140                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Define two schedules: 0) P0, P1, P2 1) P1, P3, P2. Modify TEST-DEF-80380
	in order to determine that the Module Schedule operation has no impact in the
	floating point permissions of the relevant partitions. Verify the Floating point
	context remains across module changes. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P0testdef.h>

#include <imaspex.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 100;

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
	int rc;
	volatile float a;
	volatile float pi;
	
    /* Test Start ******************************************************    */
    test_enter(1371);
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	Call pmk_sparc_enable_FP and check that no health monitor event was
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

    /* Test Step 1 
    	Perform a cumulative floating point operation and check that no
	health monitor event was generated. Wait for next MTF. */
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
	
	rtems_task_wake_after(mtf_ticks/2);
	
    /* Test Step 9 
    	Perform a cumulative floating point operation and check that no
	health monitor event was generated. */
    test_step_announce(9,1);

    /* Test step 9 code */
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

    rtems_task_wake_after(mtf_ticks);
    /* Test End */
    test_exit(res,mtf_ticks);
    return 0;
}


