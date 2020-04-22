/* Test Description ******************************************************	*/
/* An non-authorized partition should not be able to register any interrupt 
	handler. The test should test all possible interruptions. */

#ifndef TEST_GMV_1622
#define TEST_GMV_1622
	
/* Unique Test Identifier 									*/
#define TEST_ID 			 (1622)
/* Number of times the test procedure should be repeated	*/
#define TEST_REPEAT  		  (1)
/* Unique test steps (not counting step repetitions ) 		*/
#define TEST_STEPS	 		  (3)
/* Total number of tests steps executions					*/
#define TEST_ITERATIONS 	(3)

/* Test Steps IDs and Repeats *******************************************	*/

const char* const test_step_id_list[TEST_STEPS] = { \
	"000",
	"001",
	"002",
};

const int const test_step_rep_list[TEST_STEPS] = {
	1,
	1,
	1,
};

#endif /* TEST_GMV_1622 */

