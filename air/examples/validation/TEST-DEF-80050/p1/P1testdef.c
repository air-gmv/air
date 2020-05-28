
/* ***********************************************************************    */
/* ** TEST-DEF-80050 / 80050 ******************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0030                                                            */
/*    AIR_SEP_0370                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Devise a test with two partitions: 1) A regular partition 2) A supervisor
	partition Check that the first partition is unable to change a variable in the
	supervisor area (and attempting to do so raises a Health Monitor event), and the
	second partition is able to change the same variable. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P1testdef.h>

/* Test external definitions **********************************************	*/
#include <pmk_tod.h>

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
	volatile uint32_t *pvar;
	
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 1 
    	Attempt to a write predefined value to the PMK variable
	PMK_TOD_Is_set. Verify a 0x09 Trap was generated. */
    test_step_announce(1,1);
	
	pvar = (volatile uint32_t *)&PMK_TOD_Is_set;
	
	*pvar = 0xDEAD;
	
    /* Test step 1 code */
    ret |= RTEMS_SUCCESSFUL;

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (1 == unexp_error))  {
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


