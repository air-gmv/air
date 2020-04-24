
/* ***********************************************************************    */
/* ** TEST-DEF-80400 / 80400 ******************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0340                                                            */
/*    AIR_SEP_0760                                                            */
/*    AIR_SEP_0930                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Create a test for the pmk_sparc_enable_cache and pmk_sparc_disable_cache
	system calls. Run the same code in a second non-authorized partition and check
	that a Health Monitor event is generated. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P1testdef.h>
#include <pal_error.h>
#include <pal_system.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/
typedef enum {
	ALL_CACHES = 0, 		/**< Instruction and Data cache */
	DATA_CACHE = 1, 		/**< Data cache only */
	INSTRUCTION_CACHE = 2, 	/**< Instruction cache only */
} CACHE_TYPE;

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 160;

/* Test auxiliary functions    ********************************************	*/

/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id,air_error_e i_error) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    
	/* signal error ocurrence	*/
    unexp_error++;
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
	unsigned int RC;
	int diff = 0;
	unsigned int ccr;
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 8 
    	Call pmk_sparc_disable_cache for every CACHE_TYPE when cache
	management is not allowed at configuration level. Return Code:
	PAL_INVALID_CONFIG */
    test_step_announce(8,1);

    RC = pmk_sparc_disable_cache(ALL_CACHES);
	if(PAL_INVALID_CONFIG != RC){
		diff = 1;
	}
	 
	RC = pmk_sparc_disable_cache(DATA_CACHE);
	if(PAL_INVALID_CONFIG != RC){
		diff = 1;
	}
	 
	RC = pmk_sparc_disable_cache(INSTRUCTION_CACHE);
	if(PAL_INVALID_CONFIG != RC){
		diff = 1;
	}

    
    /* EXPECTED: */
    if ((0 == diff) && (3 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    /* Test Step 9 
    	Call pmk_sparc_enable_cache for every CACHE_TYPE when cache
	management is not allowed at configuration level. Return Code:
	PAL_INVALID_CONFIG */
    test_step_announce(9,1);
	diff = 0;
	
    /* Test step 9 code */
    RC = pmk_sparc_enable_cache(ALL_CACHES);
	if(PAL_INVALID_CONFIG != RC){
		diff = 1;
	}
	 
	RC = pmk_sparc_enable_cache(DATA_CACHE);
	if(PAL_INVALID_CONFIG != RC){
		diff = 1;
	}
	 
	RC = pmk_sparc_enable_cache(INSTRUCTION_CACHE);
	if(PAL_INVALID_CONFIG != RC){
		diff = 1;
	}

    
    /* EXPECTED: */
    if ((0 == diff) && (6 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

    
    /* Test End */
    test_return();
    return 0;
}


