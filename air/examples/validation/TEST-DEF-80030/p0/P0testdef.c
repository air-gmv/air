
/* ***********************************************************************    */
/* ** TEST-DEF-80030 / 80030 ******************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0020                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Define a constant string (which the compiler shall locate in the Text
	area) in the code. Get its address, and try to rewrite it using pointer
	indirection (to avoid compiler detection). Each attempt shall raise a Health Monitor
	event that shall be captured. */

#include <rtems.h>
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P0testdef.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/
const char const constant[] = "abcdefghij";


/* Test global variables **************************************************	*/
int unexp_error	 = 0;
int mtf_ticks    = 30;

/* Test auxiliary functions    ********************************************	*/

/* Test HM callbacks        ***********************************************	*/
void hm_part_callback (pmk_hm_err_id_e i_error,void *i_state) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    pmk_hm_state_id_e state = (pmk_hm_state_id_e) i_state;
	/* signal error ocurrence	*/
    unexp_error	= i_error;
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
	volatile char *p = NULL;

    /* Test Start ******************************************************    */
    test_enter(80030);
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	Define a char pointer to a constant string. */
    test_step_announce(0,1);

    /* Test step 0 code */
	p = &(constant[0]);
	if (p == NULL) {
		ret = RTEMS_INTERNAL_ERROR;
	} else {
	    ret |= RTEMS_SUCCESSFUL;
	}

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 1 
    	Try to write to the obtained location. */
    test_step_announce(1,1);

    /* Test step 1 code */
    ret |= RTEMS_SUCCESSFUL;
	*p = 'z';
	if (*p != 'a') {
		ret = RTEMS_INTERNAL_ERROR;
	}

    if (RTEMS_SUCCESSFUL == ret)  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 2 
    	Check that the hm_part_callback was called with the i_error
	parameter set to PMK_SEG_ERR. */
    test_step_announce(2,1);

    /* Test step 2 code */
	if (PMK_SEG_ERR == unexp_error) {
	    ret |= RTEMS_SUCCESSFUL;
		unexp_error = 0;
	} else {
		ret = RTEMS_INTERNAL_ERROR;
	}

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
    
    /* Test End */
    test_exit(res,mtf_ticks);
    return 0;
}


