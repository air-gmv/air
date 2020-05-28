
/* ***********************************************************************    */
/* ** TEST-DEF-01372 / 1372 ********************************************* */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0140                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Define two schedules: 0) P0, P1, P2, P3 1) P2, P3, P4, P5 Modify
	TEST-DEF-01561 in order to determine that the Module Schedule operation has no impact in
	the cache status of P2 and P3. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P1testdef.h>

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
	volatile uint32_t ccr;
	RETURN_CODE_TYPE RC;
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 2 
    	Read the cache control register. Verify if the data cache and the
	code cache are enabled. */
    test_step_announce(2,1);

    /* Test step 2 code */
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
    	Deactivate data cache. Wait for next MFT */
    test_step_announce(3,1);

    /* Test step 3 code */
    DEACTIVATE_CACHE(DATA_CACHE, &RC);

		/* EXPECTED: */
	if ((NO_ERROR == RC) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
	
	rtems_task_wake_after(mtf_ticks/2);
	
    /* Test Step 8 
    	Read the cache control register. Verify if data cache is enabled and
	the code cache is disabled. */
    test_step_announce(8,1);

    /* Test step 8 code */
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
	
	rtems_task_wake_after(mtf_ticks);
	
    /* Test Step 14 
    	Read the cache control register. Verify if data cache is enabled and
	the code cache is disabled. */
    test_step_announce(14,1);

    /* Test step 14 code */
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

    
    /* Test End */
    test_return(res);
    return 0;
}


