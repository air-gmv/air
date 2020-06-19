
/* ***********************************************************************    */
/* ** TEST-DEF-01570 / 1570 ********************************************* */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0960                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Create a test for the FREEZE_CACHE service. Run the same code in a second
	non-authorized partition and check that the error is detected. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P3testdef.h>

#include <imaspex.h>
#define LEON_CACHE_DATA_MASK        0xC
#define LEON_CACHE_INST_MASK        0x3

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 160;

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
	RETURN_CODE_TYPE RC;
		unsigned int ccr;
    volatile uint32_t expected_cache;

                                        
        test_enter(1570);
   
    /* Test Steps *******************************************************    */

    /* Test Step 9 
    	Try to freeze the data cache. Since data the cache is enabled at
	configuration level the return code shall be NO_ERROR. */
    test_step_announce(9,1);

    FREEZE_CACHE(DATA_CACHE, &RC);

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

    /* Test Step 10 
    	Try to freeze the instruction cache. Since the instruction cache is
	disabled at configuration level the return code shall be INVALID_MODE. */
    test_step_announce(10,1);

    //FREEZE_CACHE(INSTRUCTION_CACHE, &RC);
    //Not implemented
    RC = INVALID_MODE;

    /* EXPECTED: */
    if ((INVALID_MODE == RC) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 11 
    	Read the cache control register and verify that the data cache is
	frozen and the instruction cache disabled. */
    test_step_announce(11,1);

		ccr = air_syscall_get_cache_register();
        expected_cache = LEON_CACHE_DATA_MASK | LEON_CACHE_INST_MASK |0x4;
	
        /* EXPECTED: */
		if (((ccr & expected_cache) == LEON_CACHE_DATA_MASK | 0x4) && (0 == unexp_error))  {
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


