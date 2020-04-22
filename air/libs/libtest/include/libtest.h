/*
 * Copyright (C) 2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file libtest.h
 * @author pfnf
 * @brief Implements the test platform specific functions
 */

#ifndef __LIBTEST_H__
#define __LIBTEST_H__

#include <air.h>
#include <stdarg.h>
#include <pprintf.h>

/**
 * @brief Size of the partition internal buffer
 */
#define PPRINTF_BUFFER_SIZE                    (4096)
/**
 * @brief Increment the position of the internal buffer
 */
#define INCREMENT_POSITION(pos)                ((pos + 1) % PPRINTF_BUFFER_SIZE)
/**
 * @brief Align address
 */
#define ADDR_ALIGN(addr, align)     (((air_u32_t)(addr) + ((align) - 1)) & ~((align) - 1))


/**
 * @brief Test step announcement flags
 */
typedef enum {

    NONE                    = 0x00,
    SILENT                  = 0x01,
    FINISH                  = 0x02

} announce_flags;

/**
 * @brief Test step result
 */
typedef enum {
    TEST_FAILURE                 = 0x00,
    TEST_SUCCESS                 = 0x01,
} test_result;


/**
 * @brief Partition test buffer
 */
typedef struct {

    air_u32_t step_id;                    /**< partition current step       */
    air_u32_t iterations;                 /**< number of step repeats       */
    announce_flags flags;                 /**< announcement flags           */

    air_u32_t p_done;                     /**< flags partition exec. over   */
    air_u32_t p_pass;                     /**< flags partition test pass    */

    air_u32_t w_pos;                      /**< current writing position     */
    air_u32_t r_pos;                      /**< current reading position     */
    air_u32_t l_pos;                      /**< last end of line position    */
    air_i8_t buffer[PPRINTF_BUFFER_SIZE]; /**< output buffer                */

} partition_buffer_t;

/**
 * @brief Test control structure
 */
typedef struct {

    volatile air_u32_t test_id;           /**< test started                  */
    volatile air_u32_t step_id;           /**< current test step             */
    partition_buffer_t *buffers;          /**< partition test buffers        */

} test_control_t;

extern void test_step_report(char *, int, char *, int, test_result);

/**
 * @brief Initializes the test partition
 * @param shm_name name of the shared memory for the test report
 * @return number of times that the partition have been reseted
 */
air_u32_t test_partition_init(air_name_ptr_t shm_name);
/**
 * @brief Initializes the control partition
 * @param id current test id
 * @param shm_name name of the shared memory for the test report
 */
void control_partition_init(air_u32_t id, air_name_ptr_t shm_name);
/**
 * @brief Announces a test step
 * @param id id of the current step
 * @param flags step announcement flags
 *
 * @note This function blocks until the global test step allows it to run
 */
air_u32_t test_step_announce(air_u32_t id, announce_flags flags);


/**
 * @brief Finish test partition
 * @param result result of the test partition
 *
 * @note Passing SUCCESS as result after a test step fail will not mark the
 *       test as successful
 */
void test_finish(test_result result);


#endif /* __LIBTEST_H_ */
