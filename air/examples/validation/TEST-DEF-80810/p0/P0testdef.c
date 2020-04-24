
/* ***********************************************************************    */
/* ** TEST-DEF-80810 / 80810 ******************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_1020                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Define a system with two non-supervisor partitions. Use the references in
	pmk_linkcmds_vars.h to determine the location of the kernel memory area
	(air_memmap_PMK_SharedAreaStart) and try to write it. Check that a health monitor event is raised. Check
	that the remaining partitions are not affected. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P0testdef.h>


/* Test external definitions **********************************************	*/
#include <pmk_linkcmd_vars.h>
#include <pmk_rtems.h>

/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 30;

/* Pointer to PMK shared area pointer */
pmk_sharedarea_t *p_pmk_shared_area = (pmk_sharedarea_t *) &air_memmap_PMK_SharedAreaStart;

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
	int i = 0;
	
    /* Test Start ******************************************************    */
    test_enter(80810);
                                        
    /* Test Steps *******************************************************    */

	rtems_task_wake_after(mtf_ticks >> 1);

    /* Test Step 1 
    	Attempt to write the kernel memory area (number of ticks since boot). 
		Verify that a trap was raised. */
    test_step_announce(1,1);

	/* try to write a field in the PMK_shared_area*/
    p_pmk_shared_area->clock_ticks = 0;

    /* EXPECTED: */
    if (1 == unexp_error)  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }

	rtems_task_wake_after(mtf_ticks >> 1);
    
    /* Test End */
    test_exit(res,mtf_ticks);
    return 0;
}


