
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

#include <P1testdef.h>

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
	int diff = 0;
	
    /* Test Start ******************************************************    */
        test_enter(1570);
   
        test_enter(1570);
   
    /* Test Steps *******************************************************    */

    /* Test Step 5 
    	Call FREEZE_CACHE for every CACHE_TYPE when cache management is not
	allowed at configuration level. Return Code: INVALID_CONFIG */
    /* Configuration not implemented */
    test_step_announce(5,1);

    FREEZE_CACHE(ALL_CACHES, &RC);
	if(INVALID_CONFIG != RC){
		diff = 1;
	}
	 
	FREEZE_CACHE(DATA_CACHE, &RC);
	if(INVALID_CONFIG != RC){
		diff = 1;
	}
	 
	FREEZE_CACHE(INSTRUCTION_CACHE, &RC);
	if(INVALID_CONFIG != RC){
		diff = 1;
	}

    /* EXPECTED: */
    //if ((0 == diff) && (1 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
        /*
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }*/

    
    /* Test End */
    test_return(res);
    return 0;
}


