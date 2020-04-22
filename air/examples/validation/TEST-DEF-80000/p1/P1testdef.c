
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
#include <pmk_hm.h>
#include <pmk_linkcmd_vars.h>
#include <pal.h>
#include <pal_pprintf.h>
#include <pal_test.h>
#include <P1testdef.h>

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
void hm_part_callback (pmk_hm_err_id_e i_error,void *i_state) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    pmk_hm_state_id_e state = (pmk_hm_state_id_e) i_state;
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

	int *areas_p0[] = {	(int*) &air_memmap_part_text0_start ,
						(int*) &air_memmap_part_data0_start	,
						(int*) &air_memmap_part_bss0_start 	};


	int *areas_p2[] = {	(int*) &air_memmap_part_text2_start ,
						(int*) &air_memmap_part_data2_start	,
						(int*) &air_memmap_part_bss2_start 	};


    /* Test Steps *******************************************************    */
    /* Test Step 12 
    	Get a pointer to the text section of partition 0. Try to read the
	firt integer from the retrieved address. */
    test_step_announce(12,1);

    /* Test step 12 code */
    ret |= RTEMS_SUCCESSFUL;
	if (*areas_p0[TEST_TEXT] == 0) {
		dummy += 1;
	}
	res &= test_step_report(    TEST_SUCCESS,
								RESULT_EQUAL | RESULT_TYPE_VALUE,
								ret);

    /* Test Step 13 
    	Check that the hm_part_callback was called with the i_error
	parameter set to PMK_SEG_ERR. */
    test_step_announce(13,1);
	
    /* Test step 13 code */
	if (PMK_SEG_ERR == unexp_error) {
	    ret |= RTEMS_SUCCESSFUL;
		unexp_error = 0;
	} else {
		ret = RTEMS_INTERNAL_ERROR;
	}
 
    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    dummy);
    }

    /* Test Step 14 
    	Get a pointer to the data section of partition 0. Try to read the
	firt integer from the retrieved address. */
        
    test_step_announce(14,1);
    /* Test step 14 code */
    ret |= RTEMS_SUCCESSFUL;
	if (*areas_p0[TEST_DATA] == 0) {
		dummy += 1;
	}
	res &= test_step_report(    TEST_SUCCESS,
								RESULT_EQUAL | RESULT_TYPE_VALUE,
								ret);

    /* Test Step 15 
    	Check that the hm_part_callback was called with the i_error
	parameter set to PMK_SEG_ERR. */
    test_step_announce(15,1);
	
    /* Test step 15 code */
	if (PMK_SEG_ERR == unexp_error) {
	    ret |= RTEMS_SUCCESSFUL;
		unexp_error = 0;
	} else {
		ret = RTEMS_INTERNAL_ERROR;
	}
 
    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    dummy);
    }

    /* Test Step 16 
    	Get a pointer to the bss section of partition 0. Try to read the
	firt integer from the retrieved address. */
    test_step_announce(16,1);

    /* Test step 16 code */
    ret |= RTEMS_SUCCESSFUL;
	if (*areas_p0[TEST_BSS] == 0) {
		dummy += 1;
	}
	res &= test_step_report(    TEST_SUCCESS,
								RESULT_EQUAL | RESULT_TYPE_VALUE,
								ret);

    /* Test Step 17 
    	Check that the hm_part_callback was called with the i_error
	parameter set to PMK_SEG_ERR. */
    test_step_announce(17,1);

    /* Test step 17 code */
	if (PMK_SEG_ERR == unexp_error) {
	    ret |= RTEMS_SUCCESSFUL;
		unexp_error = 0;
	} else {
		ret = RTEMS_INTERNAL_ERROR;
	}
 
    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    dummy);
 	}

    /* Test Step 18 
    	Get a pointer to the text section of partition 2. Try to read the
	firt integer from the retrieved address. */
    test_step_announce(18,1);

    /* Test step 18 code */
    ret |= RTEMS_SUCCESSFUL;
	if (*areas_p2[TEST_TEXT] == 0) {
		dummy += 1;
	}
	res &= test_step_report(    TEST_SUCCESS,
								RESULT_EQUAL | RESULT_TYPE_VALUE,
								ret);

    /* Test Step 19 
    	Check that the hm_part_callback was called with the i_error
	parameter set to PMK_SEG_ERR. */
    test_step_announce(19,1);

    /* Test step 19 code */
	if (PMK_SEG_ERR == unexp_error) {
	    ret = RTEMS_SUCCESSFUL;
		unexp_error = 0;
	} else {
		ret = RTEMS_INTERNAL_ERROR;
	}
 	
    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    dummy);
    }

    /* Test Step 20 
    	Get a pointer to the data section of partition 2. Try to read the
	firt integer from the retrieved address. */
    test_step_announce(20,1);
	
    /* Test step 20 code */
    ret |= RTEMS_SUCCESSFUL;
	if (*areas_p2[TEST_DATA] == 0) {
		dummy += 1;
	}
	res &= test_step_report(    TEST_SUCCESS,
								RESULT_EQUAL | RESULT_TYPE_VALUE,
								ret);

    /* Test Step 21 
    	Check that the hm_part_callback was called with the i_error
	parameter set to PMK_SEG_ERR. */
    test_step_announce(21,1);

    /* Test step 21 code */
	if (PMK_SEG_ERR == unexp_error) {
	    ret |= RTEMS_SUCCESSFUL;
		unexp_error = 0;
	} else {
		ret = RTEMS_INTERNAL_ERROR;
	}
 
    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    dummy);
    }

    /* Test Step 22 
    	Get a pointer to the bss section of partition 2. Try to read the
	firt integer from the retrieved address. */
    test_step_announce(22,1);

    /* Test step 22 code */
    ret |= RTEMS_SUCCESSFUL;
	if (*areas_p2[TEST_BSS] == 0) {
		dummy += 1;
	}
	res &= test_step_report(    TEST_SUCCESS,
								RESULT_EQUAL | RESULT_TYPE_VALUE,
								ret);

    /* Test Step 23 
    	Check that the hm_part_callback was called with the i_error
	parameter set to PMK_SEG_ERR. */
    test_step_announce(23,1);

    /* Test step 23 code */
	if (PMK_SEG_ERR == unexp_error) {
	    ret |= RTEMS_SUCCESSFUL;
		unexp_error = 0;
	} else {
		ret = RTEMS_INTERNAL_ERROR;
	}
 
    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_step_report(    TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_step_report(    TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    dummy);
    }

	rtems_task_wake_after(mtf_ticks*3);
    
    /* Test End */
   	test_exit(res,mtf_ticks);
    return 0;
}

