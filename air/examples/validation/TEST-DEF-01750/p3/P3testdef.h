/* Test Description ******************************************************	*/
/* An non-authorized partition should not be able to register any interrupt 
	handler. The test should test all possible interruptions. */

#ifndef TEST_GMV_1750
#define TEST_GMV_1750
	
/* Unique Test Identifier 									*/
#define TEST_ID 			 (1750)
/* Number of times the test procedure should be repeated	*/
#define TEST_REPEAT  		  (1)
/* Unique test steps (not counting step repetitions ) 		*/
#define TEST_STEPS	 		  (32)
/* Total number of tests steps executions					*/
#define TEST_ITERATIONS 	(32)

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
	"011",
	"012",
	"013",
	"014",
	"015",
	"016",
	"017",
	"018",
	"019",
	"020",
	"021",
	"022",
	"023",
	"024",
	"025",
	"026",
	"027",
	"028",
	"029",
	"030",
	"031",
};

const int const test_step_rep_list[TEST_STEPS] = {
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

#endif /* TEST_GMV_1750 */


