
/* ***********************************************************************    */
/* ** ${name} / ${id} ${"*" * (63 - len (name) - len(str(id)))} */
/* ***********************************************************************    */
/* ** REQUIREMENTS                                                            */
/*                                                                            */
% for req in requirements:
/*    ${req.ljust(20)}                                                    */
% endfor
/*                                                                            */
/* ***********************************************************************    */

/* Test Description ******************************************************    */
/* ${description} */

#include <rtems.h>

#include <air.h>
#include <air_test.h>

#include <P${p}testdef.h>

% if personality == "RTEMS5_IMASPEX":
#include <imaspex.h>
% endif

/* Test external definitions **********************************************	*/


/* Test type definitions **************************************************	*/

/* Test global variables **************************************************	*/
int mtf_ticks    	= ${int(mtf * tps)};

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
	
% if p == main_part:
    /* Test Start ******************************************************    */
    test_enter(${id});
% endif
                                        
    /* Test Steps *******************************************************    */
% for t in [t for t in test_steps if t["partition"] == p]:
    /* Test Step ${t["id"]} 
    ${t["description"]} */
% if t["repeat"] > 1:
    for (repeat=1;repeat <= test_step_rep_list[${t["id"]}]; repeat ++) {
        test_step_announce(${t["id"]},repeat);

        /* Test step ${t["id"]} code */
        ret |= RTEMS_SUCCESSFUL;
    
        /* EXPECTED: */
		if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
            res &= test_step_report(	TEST_SUCCESS,
                                        RESULT_EQUAL | RESULT_TYPE_VALUE,
                                        ret);
        } else {    
            res &= test_step_report(TEST_FAILURE,
                                        RESULT_DIFF | RESULT_TYPE_VALUE,
                                        ret);
            test_exit(TEST_FAILURE,mtf_ticks);
        }
    }
% else:
    test_step_announce(${t["id"]},1);

    /* Test step ${t["id"]} code */
    ret |= RTEMS_SUCCESSFUL;

    /* EXPECTED: */
    if ((RTEMS_SUCCESSFUL == ret) && (0 == unexp_error))  {
        res &= test_report(__FILE__, __LINE__,       TEST_SUCCESS,
                                    RESULT_EQUAL | RESULT_TYPE_VALUE,
                                    ret);
    } else {    
        res &= test_report(__FILE__, __LINE__,       TEST_FAILURE,
                                    RESULT_DIFF | RESULT_TYPE_VALUE,
                                    ret);
    }
%endif
${""}
% endfor    
    
    /* Test End */
% if p == main_part:
    test_exit(res,mtf_ticks);
% else:
    test_return();
% endif
    return 0;
}

${""}
