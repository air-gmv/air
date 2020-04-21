/* 
 * Copyright (C) 2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file air_test.h
 * @author $(AIR_RUNNER_USER)
 * @brief Implements the test platform compatibility for older version
 */

#ifndef AIR_TEST_COMPATIBILITY_H
#define	AIR_TEST_COMPATIBILITY_H
#define TEST_MEMSHARE "TTEST"

#include <libtest.h>
#include <pprintf.h>

#define test_result_e test_result

typedef enum {
	RESULT_DIFF  	= 0,
	RESULT_EQUAL 	= 1,
	RESULT_GT	 	= 2,
	RESULT_LT		= 3 
} test_result_cause_e;

typedef enum {
	RESULT_TYPE_NONE		= 0x0,
	RESULT_TYPE_VALUE		= 0x4,
	RESULT_TYPE_STRING		= 0x8,
	RESULT_TYPE_POINTER		= 0xc,
	RESULT_TYPE_RETVAL		= 0x10,
	RESULT_TYPE_RTEMSRETVAL	= 0x14
} test_result_type_e;

/** Report the test start */
void test_enter(int id);

/** 
 * @brief Report the test start for post-processes tests *
 */
void test_enter_postprocssd(int id);

/** Report the status of the call 
 * @param [in] result: TEST_FAILURE | TEST_SUCCESS
 * @param [in] type	: test_result_cause_e | test_result_type_e
 * @param [in] value: (if relevant, otherwise zero)
 */
int test_report(char * file, int line, test_result_e result,test_result_cause_e type, int value);


/** 
 * Enter idle mode
 */
void test_return (void);

/** 
 * @brief Report the final test result and exit
 * @param [in] result: TEST_FAILURE | TEST_SUCCESS
 */
void test_exit(test_result_e result, int wait);


#endif	/* AIR_TEST_COMPATIBILITY_H */

