
/* ***********************************************************************    */
/* ** TEST-DEF-80000 / 80000 ******************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0010                                                            */
/*    AIR_SEP_0040                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	The pmk_linkcmds_vars.h holds references to the text, data and bss
	sections of each partition. This test shall try to read data from each of these
	sections from all partitions (other than the test partition itself). Each attempt
	shall raise a Health Monitor event that shall be captured. */

#include <rtems.h>

#include <pmk_linkcmd_vars.h>
#include <air.h>
#include <air_test.h>

#include <P0testdef.h>

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/
#define TEST_TEXT 0
#define TEST_DATA 1
#define TEST_BSS  2

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 30;

/* Test auxiliary functions    ********************************************	*/

/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id,air_error_e i_error) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    
	/* signal error ocurrence	*/
    unexp_error	= i_error;
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
	int dummy;

	int *areas_p1[] = {	(int*) &air_memmap_part_text1_start ,
						(int*) &air_memmap_part_data1_start	,
						(int*) &air_memmap_part_bss1_start 	};


	int *areas_p2[] = {	(int*) &air_memmap_part_text2_start ,
						(int*) &air_memmap_part_data2_start	,
						(int*) &air_memmap_part_bss2_start 	};

	
    /* Test Start ******************************************************    */
    test_enter(80000);
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 0 
    	Get a pointer to the text section of partition 1. Try to read the
	firt integer from the retrieved address. */
    test_step_announce(0,1);

    /* Test step 0 code */
    ret |= RTEMS_SUCCESSFUL;
	if (*areas_p1[TEST_TEXT] == 0) {
		dummy += 1;
	}
	res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
								RESULT_EQUAL | RESULT_TYPE_VALUE,
								ret);

    /* Test Step 1 
    	Check that the hm_part_callback was called with the i_error
	parameter set to PMK_SEG_ERR. */
    test_step_announce(1,1);

    /* Test step 1 code */
	if (PMK_SEG_ERR == unexp_error) {
	    ret |= RTEMS_SUCCESSFUL;
		unexp_error = 0;
	} else {
		ret = RTEMS_INTERNAL_ERROR;
	}

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    dummy);
    }

    /* Test Step 2 
    	Get a pointer to the data section of partition 1. Try to read the
	firt integer from the retrieved address. */
    test_step_announce(2,1);

    /* Test step 2 code */
	if (*areas_p1[TEST_DATA] == 0) {
		dummy += 1;
	}
    ret |= RTEMS_SUCCESSFUL;
	res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
								RESULT_EQUAL | RESULT_TYPE_VALUE,
								ret);

    /* Test Step 3 
    	Check that the hm_part_callback was called with the i_error
	parameter set to PMK_SEG_ERR. */
    test_step_announce(3,1);

    /* Test step 3 code */
	if (PMK_SEG_ERR == unexp_error) {
	    ret |= RTEMS_SUCCESSFUL;
		unexp_error = 0;
	} else {
		ret = RTEMS_INTERNAL_ERROR;
	}

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    dummy);
    }

    /* Test Step 4 
    	Get a pointer to the bss section of partition 1. Try to read the
	firt integer from the retrieved address. */
    test_step_announce(4,1);

    /* Test step 4 code */
	if (*areas_p1[TEST_BSS] == 0) {
		dummy += 1;
	}
    ret |= RTEMS_SUCCESSFUL;
	res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
								RESULT_EQUAL | RESULT_TYPE_VALUE,
								ret);

    /* Test Step 5 
    	Check that the hm_part_callback was called with the i_error
	parameter set to PMK_SEG_ERR. */
    test_step_announce(5,1);

    /* Test step 5 code */
	if (PMK_SEG_ERR == unexp_error) {
	    ret |= RTEMS_SUCCESSFUL;
		unexp_error = 0;
	} else {
		ret = RTEMS_INTERNAL_ERROR;
	}

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    dummy);
    }

    /* Test Step 6 
    	Get a pointer to the text section of partition 2. Try to read the
	firt integer from the retrieved address. */
    test_step_announce(6,1);

    /* Test step 6 code */
	if (*areas_p2[TEST_TEXT] == 0) {
		dummy += 1;
	}
    ret |= RTEMS_SUCCESSFUL;
	res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
								RESULT_EQUAL | RESULT_TYPE_VALUE,
								ret);

    /* Test Step 7 
    	Check that the hm_part_callback was called with the i_error
	parameter set to PMK_SEG_ERR. */
    test_step_announce(7,1);

    /* Test step 7 code */
	if (PMK_SEG_ERR == unexp_error) {
	    ret |= RTEMS_SUCCESSFUL;
		unexp_error = 0;
	} else {
		ret = RTEMS_INTERNAL_ERROR;
	}
 
    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    dummy);
    }

    /* Test Step 8 
    	Get a pointer to the data section of partition 2. Try to read the
	firt integer from the retrieved address. */
    test_step_announce(8,1);

    /* Test step 8 code */
	if (*areas_p2[TEST_DATA] == 0) {
		dummy += 1;
	}
    ret |= RTEMS_SUCCESSFUL;
	res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
								RESULT_EQUAL | RESULT_TYPE_VALUE,
								ret);
								
    /* Test Step 9 
    	Check that the hm_part_callback was called with the i_error
	parameter set to PMK_SEG_ERR. */
    test_step_announce(9,1);

    /* Test step 9 code */
	if (PMK_SEG_ERR == unexp_error) {
	    ret |= RTEMS_SUCCESSFUL;
		unexp_error = 0;
	} else {
		ret = RTEMS_INTERNAL_ERROR;
	}
 
    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    dummy);
    }

    /* Test Step 10 
    	Get a pointer to the bss section of partition 2. Try to read the
	firt integer from the retrieved address. */
    test_step_announce(10,1);

    /* Test step 10 code */
	if (*areas_p2[TEST_BSS] == 0) {
		dummy += 1;
	}
    ret |= RTEMS_SUCCESSFUL;
	res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
								RESULT_EQUAL | RESULT_TYPE_VALUE,
								ret);
								

    /* Test Step 11 
    	Check that the hm_part_callback was called with the i_error
	parameter set to PMK_SEG_ERR. */
    test_step_announce(11,1);

    /* Test step 11 code */
	if (PMK_SEG_ERR == unexp_error) {
	    ret |= RTEMS_SUCCESSFUL;
		unexp_error = 0;
	} else {
		ret = RTEMS_INTERNAL_ERROR;
	}
 
    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    dummy);
    }

	rtems_task_wake_after(mtf_ticks*3);

    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
		test_return(res);
	} else {
    	test_exit(res,mtf_ticks);
	}
    return 0;
}

