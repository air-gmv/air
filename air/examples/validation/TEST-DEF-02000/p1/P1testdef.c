
/* ***********************************************************************    */
/* ** TEST-DEF-02000 / 2000 ********************************************* */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
/*    AIR_SEP_0700                                                            */
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* 	Implements the ARINC-653 Part 3 tests related to the SET_PARTITION_MODE
	API. */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P1testdef.h>


/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int mtf_ticks    	= 40;

/* Test auxiliary functions    ********************************************	*/

/* Test HM callbacks        ***********************************************	*/
void partition_HM_callback(air_state_e state_id,air_error_e i_error) {
    /* i_state is not really a pointer, this signature is required due to the
        pal_callbacks function signature -> convert it into a relevant value */
    
    return;
}

/* Test execution *******************************************************    */
int test_main (void) {
    /* Test generic variables  ******************************************	*/
    /* repetition iteration counter	*/ 
    int repeat     = 0;                
    
    /* function to test return code     */
    rtems_status_code ret    = RTEMS_SUCCESSFUL; 

    /* total test result                */
    int res     = TEST_SUCCESS;     
    
    /* Test specific variables  ******************************************	*/
	
                                        
    /* Test Steps *******************************************************    */
    
    /* Test End */
    test_return(res);
    return 0;
}


