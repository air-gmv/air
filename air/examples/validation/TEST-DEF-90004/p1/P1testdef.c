
/* ***********************************************************************    */
/* ** TEST-DEF-90004 / 90004 ******************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0120                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Configure a test with 2 partitions, in which one of them call a function
	that sets the frame pointer to 0. Checks if that function blocks or not the
	other partition. */

#include <rtems.h>
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P1testdef.h>


/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 150;

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
	
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 3 
    	Test if the partition is blocked or not. */
    test_step_announce(3,1);

    /* Test step 3 code */	

	ret |= RTEMS_SUCCESSFUL;

	/* EXPECTED: */
	if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
		res &= test_step_report(    TEST_SUCCESS,
									RESULT_EQUAL | RESULT_TYPE_VALUE,
									ret);
	} else {    
		res &= test_step_report(    TEST_FAILURE,
									RESULT_DIFF | RESULT_TYPE_VALUE,
									ret);
	}
    
    /* Test End */
    test_return();
    return 0;
}


