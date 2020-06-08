
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

#include <air.h>
#include <air_test.h>

#include <P3testdef.h>


/* Test external definitions **********************************************	*/
#include <pal_system.h>
#include <stdint.h>

/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 40;

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
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 3 
    	Read the cache control register. Verify if the Data cache and the
	Code Cache are disabled. Wait for next execution window. */
    for (repeat=1;repeat <= test_step_rep_list[4]; repeat ++) {
        test_step_announce(3,repeat);

        ccr = pmk_sparc_get_cache_ctrl();
		
        /* EXPECTED: */
		if (((ccr & 0xF) == 0x0) && (0 == unexp_error))  {
            res &= test_report(__FILE__, __LINE__,    	TEST_SUCCESS,
                                        RESULT_EQUAL | RESULT_TYPE_VALUE,
                                        ret);
        } else {    
            res &= test_report(__FILE__, __LINE__,    TEST_FAILURE,
                                        RESULT_DIFF | RESULT_TYPE_VALUE,
                                        ret);
            test_exit(TEST_FAILURE,mtf_ticks);
        }
		
		rtems_task_wake_after(mtf_ticks);
    }

    
    /* Test End */
    test_return(res);
    return 0;
}


