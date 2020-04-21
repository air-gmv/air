/* Test Description ******************************************************	*/
/* An non-authorized partition should not be able to register any interrupt 
	handler. The test should test all possible interruptions. */

#ifndef TEST_GMV_${id}
#define TEST_GMV_${id}
	
/* Unique Test Identifier 									*/
#define TEST_ID 			 (${id})
/* Number of times the test procedure should be repeated	*/
#define TEST_REPEAT  		  (${repeat})
/* Unique test steps (not counting step repetitions ) 		*/
#define TEST_STEPS	 		  (${len(test_steps)})
/* Total number of tests steps executions					*/
#define TEST_ITERATIONS 	(${total_steps})

/* Test Steps IDs and Repeats *******************************************	*/

const char* const test_step_id_list[TEST_STEPS] = { ${"\\"}
% for t in test_steps:
	"${str(t["id"]).rjust(3,'0')}",
% endfor
};

const int const test_step_rep_list[TEST_STEPS] = {
% for t in test_steps:
	${t["repeat"]},
% endfor
};

#endif /* TEST_GMV_${id} */

${""}
