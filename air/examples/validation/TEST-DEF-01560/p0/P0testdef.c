
/* ***********************************************************************    */
/* ** TEST-DEF-01560 / 1560 ********************************************* */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0920                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Define a test with the following configuration: P1: ExWindow=10ms,
	Period=40ms, Data Cache Enabled, Code Cache Enabled P2: ExWindow=10ms, Period=40ms,
	Data Cache Enabled, Code Cache Disabled P3: ExWindow=10ms, Period=40ms, Data
	Cache Disabled, Code Cache Enabled P4: ExWindow=10ms, Period=40ms, Data Cache
	Disabled, Code Cache Disabled Use the Get Cache Control Register system call on each
	partition to determine the code and data cache are as expected. Run the test for at
	least 3 Major Time Frames. */

#include <rtems.h>
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P0testdef.h>


/* Test external definitions **********************************************	*/
#include <pal_system.h>
#include <stdint.h>

/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 40;

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
	
    /* Test Start ******************************************************    */
    test_enter(1560);
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	Read the cache control register. Verify if the Data cache and the
	Code Cache are enabled. Wait for next execution window.. */
    for (repeat=1; repeat <= test_step_rep_list[1]; repeat++ ) {
        test_step_announce(0,repeat);
		
		ccr = pmk_sparc_get_cache_ctrl();
	
        /* EXPECTED: */
		if (((ccr & 0xF) == 0xF) && (0 == unexp_error))  {
            res &= test_step_report(	TEST_SUCCESS,
                                        RESULT_EQUAL | RESULT_TYPE_VALUE,
                                        ret);
        } else {    
            res &= test_step_report(TEST_FAILURE,
                                        RESULT_DIFF | RESULT_TYPE_VALUE,
                                        ret);
            test_exit(TEST_FAILURE,mtf_ticks);
        }
		
		rtems_task_wake_after(mtf_ticks);
    }

    
    /* Test End */
    test_exit(res,mtf_ticks);
    return 0;
}


