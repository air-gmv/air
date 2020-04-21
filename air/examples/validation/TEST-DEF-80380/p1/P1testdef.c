
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
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P1testdef.h>


/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int err_id 		= 0;
int mtf_ticks    	= 40;

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
	by the health monitoring. */
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
	
	unexp_error = 0;
	err_id = 0;
	
	
    /* Test Step 3 
    	Perform a Floating Point operation and check that a PMK_FLOAT_ERR is
	captured by the health monitoring. */
    test_step_announce(3,1);

    /* Test step 3 code */
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


