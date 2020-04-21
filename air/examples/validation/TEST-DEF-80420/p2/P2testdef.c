
/* ***********************************************************************    */
/* ** TEST-DEF-80420 / 80420 ******************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0340                                                            */
/*    AIR_SEP_0760                                                            */
/*    AIR_SEP_0960                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Create an unit test for the CACHE_FLUSH system call. Run the same code in
	a second non-authorized partition and check that a Health Monitor event is
	generated. */

#include <rtems.h>
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P2testdef.h>

#include <imaspex.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 120;

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
	RETURN_CODE_TYPE RC;
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 5 
    	Try to flush the instruction cache. Since instruction the cache is
	enabled at configuration level the return code shall be NO_ERROR. */
    test_step_announce(5,1);

    /* Test step 5 code */
    FLUSH_CACHE(INSTRUCTION_CACHE, &RC);

    /* EXPECTED: */
    if ((NO_ERROR == RC) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 6 
    	Try to flush the data cache. Since the data cache is disabled at
	configuration level the return code shall be INVALID_MODE. */
    test_step_announce(6,1);

    /* Test step 5 code */
    FLUSH_CACHE(DATA_CACHE, &RC);

    /* EXPECTED: */
    if ((INVALID_MODE == RC) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 7 
    	Try to flush both caches. Since the data cache is disabled at
	configuration level the return code shall be INVALID_MODE. */
    test_step_announce(7,1);

    /* Test step 5 code */
    FLUSH_CACHE(ALL_CACHES, &RC);

    /* EXPECTED: */
    if ((INVALID_MODE == RC) && (0 == unexp_error))  {
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


