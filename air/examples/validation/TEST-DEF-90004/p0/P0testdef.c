
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
	that sets the stack pointer to 0. Checks if that function blocks or not the
	other partition. */

#include <rtems.h>
#include <pmk_hm.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P0testdef.h>


/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 150;

/* Test auxiliary functions    ********************************************	*/
inline void pmk_null_stack_pointer(void);

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
	int repeat = 0;                
	/* function to test return code     */
    rtems_status_code ret = RTEMS_SUCCESSFUL; 
	/* total test result                */
    int res = TEST_SUCCESS;     
    
    /* Test specific variables  ******************************************	*/
	
    /* Test Start ******************************************************    */
    test_enter(90004);
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	Call the function pmk_null_stack_pointer. */
    test_step_announce(0,1);

    /* Test step 0 code */
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
	
	pmk_null_stack_pointer();
    
    /* Test End */
    test_exit(res,mtf_ticks);
    return 0;
}

inline void pmk_null_stack_pointer(void){
	/* function to test return code     */
    rtems_status_code ret = RTEMS_SUCCESSFUL; 
	/* total test result                */
    int res = TEST_SUCCESS;   
	
    /* Test Step 1 
    	Set stack pointer to 0. */ 
    test_step_announce(1,1);

    /* Test step 1 code */
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
	
	asm volatile( "mov 0, %sp" );
	
	/* Test Step 2 
    	Sleep for 50 clock ticks, so the output is shown in order. */
    test_step_announce(2,1);

    /* Test step 2 code */
	
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
	
	rtems_task_wake_after(50);
}
