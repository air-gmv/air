
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
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P0testdef.h>

#include <imaspex.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 100;

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
    test_enter(1372);
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	Read the cache control register. Verify if the data cache and the
	code cache are enabled. */
    test_step_announce(0,1);

    /* Test step 0 code */
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
    	Deactivate code cache. Wait for next MFT */
    test_step_announce(1,1);

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
	
	rtems_task_wake_after(mtf_ticks/2);

    /* Test Step 13 
    	Read the cache control register. Verify if data cache is enabled and
	the code cache is disabled. */
    test_step_announce(13,1);

    /* Test step 13 code */
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

    rtems_task_wake_after(mtf_ticks);
    /* Test End */
    test_exit(res,mtf_ticks);
    return 0;
}

