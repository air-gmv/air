/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file libtest.h
 * @author pfnf
 * @brief Implements the test platform specific functions
 */

#ifndef __LIBTEST_H__
#define __LIBTEST_H__

#include <air.h>
#include <stdarg.h>

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

#ifdef COLORS
#define COLOR_KNRM                             "\x1B[0m"
#define COLOR_KRED                             "\x1B[31m"
#define COLOR_KGRN                             "\x1B[32m"
#define COLOR_KYEL                             "\x1B[33m"
#define COLOR_KBLU                             "\x1B[34m"
#define COLOR_KMAG                             "\x1B[35m"
#define COLOR_KCYN                             "\x1B[36m"
#define COLOR_KWHT                             "\x1B[37m"
#define COLOR_RESET                            "\033[0m"
#else
#define COLOR_KNRM                             ""
#define COLOR_KRED                             ""
#define COLOR_KGRN                             ""
#define COLOR_KYEL                             ""
#define COLOR_KBLU                             ""
#define COLOR_KMAG                             ""
#define COLOR_KCYN                             ""
#define COLOR_KWHT                             ""
#define COLOR_RESET                            ""
#endif

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

    FAILURE                 = 0x00,
    SUCCESS                 = 0x01,

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

extern void __test_step_report(char *, char *, int, test_result);

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
 * @brief Reports the result of a test
 * @param cond test step condition
 */
#define test_step_report(cond) \
    do{\
        if (!(cond)){\
            __test_step_report(#cond, __FILE__, __LINE__, FAILURE);\
        } else {\
            __test_step_report(#cond, __FILE__, __LINE__, SUCCESS);\
        }\
    } while(0)

/**
 * @brief Finish test partition
 * @param result result of the test partition
 *
 * @note Passing SUCCESS as result after a test step fail will not mark the
 *       test as successful
 */
void test_finish(test_result result);



void pprintf(const char *fmt , ...);

#endif /* __LIBTEST_H_ */
