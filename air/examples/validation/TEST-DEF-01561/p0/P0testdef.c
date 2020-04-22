
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
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P0testdef.h>


/* Test external definitions **********************************************	*/
#include <imaspex.h>
#include <stdint.h>

/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 30;

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
	volatile uint32_t ccr;
	RETURN_CODE_TYPE RC;
	
    /* Test Start ******************************************************    */
    test_enter(1561);
	imaspex_init();
                                        
    /* Test Steps *******************************************************    */
	for (repeat=1; repeat <= 3; repeat++ ) {
		/* Test Step 0 
			Read the cache control register. Verify if the Data cache and the
		Code Cache are enabled. */
		test_step_announce(0,repeat);

		ccr = pmk_sparc_get_cache_ctrl();
	
        /* EXPECTED: */
		if (((ccr & 0xF) == 0xF) && (0 == unexp_error))  {
			res &= test_step_report(    TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_step_report(    TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										ret);
		}

		/* Test Step 1 
			Deactivate code cache. Wait for Next MFT */
		test_step_announce(1,repeat);

		/* Test step 1 code */
		DEACTIVATE_CACHE(INSTRUCTION_CACHE, &RC);

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
		
		rtems_task_wake_after(mtf_ticks);

		/* Test Step 8 
			Read the cache control register. Verify if data cache is enabled and
		the code cache is disabled. */
		test_step_announce(8,repeat);

		ccr = pmk_sparc_get_cache_ctrl();
	
        /* EXPECTED: */
		if (((ccr & 0xF) == 0xC) && (0 == unexp_error))  {
			res &= test_step_report(    TEST_SUCCESS,
										RESULT_EQUAL | RESULT_TYPE_VALUE,
										ret);
		} else {    
			res &= test_step_report(    TEST_FAILURE,
										RESULT_DIFF | RESULT_TYPE_VALUE,
										ret);
		}

		/* Test Step 9 
			Activate code cache. Wait for Next MFT */
		test_step_announce(9,repeat);

		/* Test step 1 code */
		ACTIVATE_CACHE(INSTRUCTION_CACHE, &RC);

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
		
		rtems_task_wake_after(mtf_ticks);
	}
    
    /* Test End */
    test_exit(res,mtf_ticks);
    return 0;
}

