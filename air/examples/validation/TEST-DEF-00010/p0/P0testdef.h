/* Test Description ******************************************************	*/
/* An non-authorized partition should not be able to register any interrupt 
	handler. The test should test all possible interruptions. */

#ifndef TEST_GMV_10
#define TEST_GMV_10
	
/* Unique Test Identifier 									*/
#define TEST_ID 			 (10)
/* Number of times the test procedure should be repeated	*/
#define TEST_REPEAT  		  (1)
/* Unique test steps (not counting step repetitions ) 		*/
#define TEST_STEPS	 		  (11)
/* Total number of tests steps executions					*/
#define TEST_ITERATIONS 	(11)

/* Test Steps IDs and Repeats *******************************************	*/

const char* const test_step_id_list[TEST_STEPS] = { \
	"000",
	"001",
	"002",
	"003",
	"004",
	"005",
	"006",
	"007",
	"008",
	"009",
	"010",
};

const int const test_step_rep_list[TEST_STEPS+1] = {
	0,	/* Test steps are not really 0-base indexed, so... */
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
};

int test_main (void);

#endif /* TEST_GMV_10 */


