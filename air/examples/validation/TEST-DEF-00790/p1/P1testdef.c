
/* ***********************************************************************    */
/* ** TEST-DEF-00790 / 790 ********************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0330                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Define a test where the MMU region is overwritten by a non-supervisor partition. Check that this attempt is unsuccessful, and the event is captured and handled by the HM, and the associated HM action is called. Test if the remaining partitions continue to execute. */

#include <rtems.h>
#include <air.h>
#include <air_test.h>
#include <P1testdef.h>
#include <segregation.h>


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 30;

/* Test auxiliary functions    ********************************************	*/
extern air_u32_t *ln_tables[4];

/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id,air_error_e i_error) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    
	/* signal error ocurrence	*/
    unexp_error = i_error;
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
	int part_point = 1;
    air_u32_t * current_pointer;
    

    /* Test Start ******************************************************    */
    test_enter(790);

    /* Test Steps *******************************************************    */
    /* Test Step 1 
    	Attempt to write in the MMU region. Verify that a trap was
	raised. */
    test_step_announce(1,1);

    /* Set zero random MMU entry */
    /* Context MMU */
	current_pointer = ln_tables[0]; // fazer syscall para extrair
    current_pointer += 100;
    *current_pointer = 0xDEAD;

    /* EXPECTED: */
    if (AIR_SEGMENTATION_ERROR == unexp_error)  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
		unexp_error = 0;
    } else {
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    unexp_error);
		test_exit(TEST_FAILURE,mtf_ticks>>1);
    }

    /* Test End */
    test_return(res);
    return 0;
}


