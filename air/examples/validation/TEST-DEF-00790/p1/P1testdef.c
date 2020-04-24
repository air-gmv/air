
/* ***********************************************************************    */
/* ** TEST-DEF-00790 / 790 ********************************************** */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0330                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Define a test where the MMU tables (L1, L2, L3) are overwritten by a
	non-supervisor partition. Check that this attempt is unsuccessful, and the event is
	captured and handled by the HM, and the associated HM action is called. Test if the
	remaining partitions continue to execute. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P1testdef.h>


/* Test external definitions **********************************************	*/
#include <pmk_segregation.h>
#include <pmk_linkcmd_vars.h>

/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 30;

/* Context Table */
PMK_MMU_ContextTable_t *ContextTable = 
			(PMK_MMU_ContextTable_t *)&air_memmap_MMU_ContextTableStart;

/** Pointer to System Level 1 table which contains L1 tables for all partitions*/
static PMK_MMU_L1Table_t *L1Table	= 
			(PMK_MMU_L1Table_t *)&air_memmap_MMU_Level1TableStart;	
	
/** Pointer to System Level 2 table which contains L2 tables for all partitions*/
static PMK_MMU_Partition_L2Tables_t *L2Table = 
			(PMK_MMU_Partition_L2Tables_t *)&air_memmap_MMU_Level2TablesStart;

/** Pointer to System Level 3 table which contains L3 tables for all partitions*/
static PMK_MMU_Partition_L3Tables_t *L3Table =
			(PMK_MMU_Partition_L3Tables_t *) &air_memmap_MMU_Level3TablesStart;

/* Test auxiliary functions    ********************************************	*/

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
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 1 
    	Attempt to write the Context MMU Table. Verify that a trap was
	raised. */
    test_step_announce(1,1);

    /* delete one random context entry */
	(*ContextTable)[part_point] = 0x0;

    /* EXPECTED: */
    if (PMK_SEG_ERR == unexp_error)  {
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

    /* Test Step 2 
    	Attempt to write a Level1 MMU Table entry. Verify that a trap was
	raised. */
    test_step_announce(2,1);

    /* change one random entry entry */
    (L1Table)[0][10] = 0xDEAD;

    /* EXPECTED: */
    if (PMK_SEG_ERR == unexp_error)  {
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

    /* Test Step 3 
    	Attempt to write a Level2 MMU Table entry. Verify that a trap was
	raised. */
    test_step_announce(3,1);

    /* Test step 3 code */
	L2Table[1][5][10] = 0xDEAD;

    /* EXPECTED: */
    if (PMK_SEG_ERR == unexp_error)  {
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

    /* Test Step 4 
    	Attempt to write a Level3 MMU Table entry. Verify that a trap was
	raised. */
    test_step_announce(4,1);

    /* Test step 4 code */
    L3Table[0][0][20] = 0xDEAD;

    /* EXPECTED: */
    if (PMK_SEG_ERR == unexp_error)  {
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
    test_return();
    return 0;
}


