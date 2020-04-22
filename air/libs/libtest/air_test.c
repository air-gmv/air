/*
 * Copyright (C) 2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */

#include <air.h>
#include <air_test.h>

/** 
 * @brief Report the test start *
 */
void test_enter(int id) {    
    test_partition_init(TEST_MEMSHARE);
}

/** 
 * @brief Report the test start for post-processes tests *
 */
void test_enter_postprocssd(int id) {
	pprintf("POST NOT IMPLEMENTED check air_test.c\n",id);
}

/** 
 * @brief Report the status of the call 
 * @param [in] result: TEST_FAILURE | TEST_SUCCESS
 * @param [in] type	: test_result_cause_e | test_result_type_e
 * @param [in] value: (if relevant, otherwise zero)
 */
int test_report(char * file, int line, test_result_e result,test_result_cause_e ttype, int tvalue){
        switch (ttype & 0x3){
            case RESULT_DIFF:
                test_step_report("Result is different from ", tvalue , file, line, result);
                break;
            case RESULT_EQUAL:
                test_step_report("Result is equal to ", tvalue , file, line, result);
                break;
            case RESULT_GT:
                test_step_report("Result is greater than ", tvalue , file, line, result);
                break;
            case RESULT_LT:
                test_step_report("Result is lower than ", tvalue , file, line, result);
                break;
        }

	return (int)result;
}

/** 
 * @brief Enter idle mode
 */
void test_return (void) {
    // do nothing, idle is automatic
}

/** 
 * @brief Report the final test result and exit
 * @param [in] result: TEST_FAILURE | TEST_SUCCESS
 */
void test_exit(test_result_e result, int wait) {
    test_finish(result);
}
