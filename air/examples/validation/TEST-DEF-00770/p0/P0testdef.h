/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2013
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system testsuite, which is distributed under the terms of the AIR License
 *  Agreement.
 *  The full extent of the AIR License Agreement is described in the contract
 *  celebrated between GMVIS Skysoft and the Client.
 * ==========================================================================*/

/*
 * TEST-DEF-00770
 *
 * Requirements:
 *
 * Test Description:
 *    Define a test configuration with the following actions for the given
 *    states: State: Partition Application, Error: Segmentation Error,
 *    ErrorLevel: Partition State: Health Monitor Processing, Error:
 *    Segmentation Error, ErrorLevel: Module The test application shall generate
 *    a segmentation fault. The partition callback called by the Health Monitor
 *    shall in turn also generate a segmentation fault. Check that this
 *    translates into a system level HM event that is properly detected and
 *    handled.
 */

#ifndef _TEST_GMV_770_
#define _TEST_GMV_770_

/* Unique Test Identifier */
#define TEST_ID              (770)
/* Number of times the test procedure should be repeated */
#define TEST_REPEAT  		 (1)
/* Unique test steps (not counting step repetitions) */
#define TEST_STEPS	 		 (5)
/* Total number of tests steps executions */
#define TEST_ITERATIONS 	 (5)

/* Test Steps IDs and Repeats */
const char* const test_step_id_list[TEST_STEPS] = { \
	"000",
	"001",
	"002",
	"003",
	"004",
};

const int const test_step_rep_list[TEST_STEPS] = {
	1,
	1,
	1,
	1,
	1,
};

#endif /* _TEST_GMV_770_ */
