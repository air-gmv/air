
/* ***********************************************************************    */
/* ** TEST-DEF-01561 / 1561 ********************************************* */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0930                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Modify TEST-DEF-01560 so that the first three partitions toggle the cache
	status at each Execution window, in the following manner: P1: Toggle the code
	cache status P2: Toggle the data cache status P3: Toggle the code and data
	status P4: Same as P3 but the system calls are not authorized. All but P4 shall
	succeed in changing the cache status and having those changes preserved by the
	partition context switch. P4 shall raise health monitor events. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P1testdef.h>


/* Test external definitions **********************************************	*/
#include <imaspex.h>
#include <stdint.h>

/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 30;

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
	volatile uint32_t ccr;
	RETURN_CODE_TYPE RC;
    
	imaspex_init();
	
    /* Test Steps *******************************************************    */
	for (repeat=1; repeat <= 3; repeat++ ) {
		/* Test Step 2 
			Read the cache control register. Verify if the Data cache and the
		Code Cache are enabled. */
		test_step_announce(2,repeat);

		ccr = pmk_sparc_get_cache_ctrl();
	
        /* EXPECTED: */
		if (((ccr & 0xF) == 0xF) && (0 == unexp_error))  {
			res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										ret);
		}

		/* Test Step 3 
			Deactivate data cache. Wait for Next MFT */
		test_step_announce(3,repeat);

		/* Test step 1 code */
		DEACTIVATE_CACHE(DATA_CACHE, &RC);

		/* EXPECTED: */
		if ((NO_ERROR == RC) &&  (0 == unexp_error))  {
			res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										ret);
		}
		
		rtems_task_wake_after(mtf_ticks);
		
		/* Test Step 10 
			Read the cache control register. Verify if data cache is enabled and
		the code cache is disabled. */
		test_step_announce(10,repeat);

		ccr = pmk_sparc_get_cache_ctrl();
	
        /* EXPECTED: */
		if (((ccr & 0xF) == 0x3) && (0 == unexp_error))  {
			res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										ret);
		}

		/* Test Step 11 
			Activate code cache. Wait for Next MFT */
		test_step_announce(11,repeat);

		/* Test step 1 code */
		ACTIVATE_CACHE(DATA_CACHE, &RC);

		/* EXPECTED: */
		if ((NO_ERROR == RC) &&  (0 == unexp_error))  {
			res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										ret);
		}
		
		rtems_task_wake_after(mtf_ticks);
	}
    
    /* Test End */
    test_return(res);
    return 0;
}


