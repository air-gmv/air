
/* ***********************************************************************    */
/* ** TEST-DEF-01200 / 1200 ********************************************* */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0430                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Test if the execution of an invalid opcode intruction in a user mode
	(non-privileged) partition is captured by the LEON and handled by the HM, and the
	associated HM action is called. The invalid opcode can be created (for example) by
	branching to the address of a fixed string. Check that the remaining partitions are
	not affected. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P1testdef.h>


/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int unexp_error	   = 0;
int mtf_ticks    	= 30;

/* Test auxiliary functions    ********************************************	*/
static __inline__ void ASM_UNIMP(void) {
	__asm__ __volatile__("unimp");
}

/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id,air_error_e i_error) {
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
	int i = 0;
                                        
    /* Test Steps *******************************************************    */
    /* Test Step 1 
    	Execute a unimp instruction. Verify the HM event was raised and the
	partition continues executing. */
    test_step_announce(1,1);

   i++;
	
    /* Test step 1 code */
    ASM_UNIMP();
	
	i++;

    /* EXPECTED: */
    if ((2 == i) && (PMK_ILL_ERR == unexp_error))  {
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


