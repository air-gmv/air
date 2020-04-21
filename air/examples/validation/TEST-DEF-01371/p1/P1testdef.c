
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
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P1testdef.h>

#include <imaspex.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int err_id 		= 0;
int mtf_ticks    	= 100;

/* Test auxiliary functions    ********************************************	*/

/* Test HM callbacks        ***********************************************	*/
void hm_part_callback (pmk_hm_err_id_e i_error,void *i_state) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    pmk_hm_state_id_e state = (pmk_hm_state_id_e) i_state;
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
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 2 
    	Call pmk_sparc_enable_FP and check that a PMK_VIOL_ERR is captured
	by the health monitoring. Wait for next MTF. */
    test_step_announce(2,1);

    /* Test step 2 code */
    rc = pmk_sparc_enable_FP();

    /* EXPECTED: */
    if ((PAL_INVALID_CONFIG == rc) && (1 == unexp_error) && (PMK_VIOL_ERR == err_id))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
	
	rtems_task_wake_after(mtf_ticks/2);
	err_id = 0;
	unexp_error = 0;
	
    /* Test Step 5 
    	Call pmk_sparc_enable_FP and check that a PMK_VIOL_ERR is captured
	by the health monitoring. Wait for next MTF. */
    test_step_announce(5,1);

    /* Test step 5 code */
    rc = pmk_sparc_enable_FP();

    /* EXPECTED: */
    if ((PAL_INVALID_CONFIG == rc) && (1 == unexp_error) && (PMK_VIOL_ERR == err_id))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
	
	rtems_task_wake_after(mtf_ticks/2);
	err_id = 0;
	unexp_error = 0;
	
    /* Test Step 10 
    	Perform a floating point operation and check that a PMK_FLOAT_ERR is
	captured by the health monitoring. */
    test_step_announce(10,1);

    /* Test step 10 code */
    a = 2.25+3.75;

    /* EXPECTED: */
    if ((PMK_FLOAT_ERR == err_id) && (1 == unexp_error))  {
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


