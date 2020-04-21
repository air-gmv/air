
/* ***********************************************************************    */
/* ** TEST-DEF-80401 / 80401 ******************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0940                                                            */
/*    AIR_SEP_0950                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Create a unit test for the ACTIVATE_CACHE and DEACTIVATE_CACHE service
	calls. */

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
int mtf_ticks    	= 200;

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
	int diff = 0;
    /* Test Steps *******************************************************    */
    /* Test Step 6 
    	Call DEACTIVATE_CACHE for every CACHE_TYPE when cache management is
	not allowed at configuration level. Return Code: INVALID_CONFIG */
    test_step_announce(6,1);

    DEACTIVATE_CACHE(ALL_CACHES, &RC);
	if(INVALID_CONFIG != RC){
		diff = 1;
	}
	 
	DEACTIVATE_CACHE(DATA_CACHE, &RC);
	if(INVALID_CONFIG != RC){
		diff = 1;
	}
	 
	DEACTIVATE_CACHE(INSTRUCTION_CACHE, &RC);
	if(INVALID_CONFIG != RC){
		diff = 1;
	}

    /* EXPECTED: */
    if ((0 == diff) && (1 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 7 
    	Call ACTIVATE_CACHE for every CACHE_TYPE when cache management is
	not allowed at configuration level. Return Code: INVALID_CONFIG */
    test_step_announce(7,1);
	
	diff = 0;
	
    ACTIVATE_CACHE(ALL_CACHES, &RC);
	if(INVALID_CONFIG != RC){
		diff = 1;
	}
	 
	ACTIVATE_CACHE(DATA_CACHE, &RC);
	if(INVALID_CONFIG != RC){
		diff = 1;
	}
	 
	ACTIVATE_CACHE(INSTRUCTION_CACHE, &RC);
	if(INVALID_CONFIG != RC){
		diff = 1;
	}

    /* EXPECTED: */
    if ((0 == diff) && (1 == unexp_error))  {
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


