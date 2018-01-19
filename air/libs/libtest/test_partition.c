/* ============================================================================
 *  Copyright (C) GMVIS Skysoft S.A., 2014
 * ============================================================================
 *  This file is part of the AIR - ARINC 653 Interface in RTEMS - Operating
 *  system.
 *  The license and distribution terms for this file may be found in the file
 *  LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 * ==========================================================================*/
/**
 * @file test_partition.c
 * @author pfnf
 * @brief Functions for the test partition
 */

#include <libtest.h>

/**
 * @brief Partition
 */
air_partition_status_t partition;
/**
 * @brief Test control area
 */
test_control_t *test_control;
/**
 * @brief Current partition buffer area
 */
partition_buffer_t *partition_buffer;

/**
 * @brief Initializes the test partition
 * @param shm_name name of the shared memory for the test report
 * @return number of times that the partition have been reseted
 */
air_u32_t test_partition_init(air_name_ptr_t shm_name) {

    /* clear local pointers */
    test_control = NULL;
    partition_buffer = NULL;
    air_sharedmemory_t sharedmemory;

    /* get partition configuration */
    air_syscall_get_partition_status(-1, &partition);

    /* get shared memory area */
    if (air_syscall_get_sharedmemory(shm_name, &sharedmemory) == AIR_NO_ERROR) {

        test_control = (test_control_t *)sharedmemory.address;

    } else {

        /* shutdown partition */
        air_syscall_set_partition_mode(-1, AIR_MODE_IDLE);
    }

    /* busy wait until the until the test controller initializes */
    while (test_control->test_id == 0);

    /* get the current partition buffer */
    partition_buffer = &test_control->buffers[partition.index];

    /* return number of partition restarts */
    return partition.restart_count;
}

/**
 * @brief Announces a test step
 * @param id id of the current step
 * @param flags step announcement flags
 * @return current test id
 *
 * @note This function blocks until the global test step allows it to run
 */
air_u32_t test_step_announce(air_u32_t id, announce_flags flags) {

    /* wait for the current step */
    while (id > test_control->step_id);

    /* advance the current partition test step */
    if ((flags & SILENT) != 0) {

        test_control->step_id = id + 1;
    }

    /* mark that partition finish its test steps */
    if ((flags & FINISH) != 0) {

        partition_buffer->p_done = 1;
    }

    /* control partition test step iterations  */
    if (partition_buffer->step_id != id) {
        partition_buffer->iterations = 0;
    }

    /* store the current step as the current step */
    partition_buffer->step_id = id;
    partition_buffer->flags = flags;

    return id;
}

/**
 * @brief Test Step report
 * @param cond test step condition
 * @param file file of test step
 * @param line of the test step condition
 * @param res result of the test step
 */
void __test_step_report(char *cond, char *file, int line, test_result res) {

    /*pprintf("|  %04x%03x|%x|%01x:%02x  |\n",
            partition_buffer->step_id,
            partition_buffer->iterations,
            res,
            0,
            0);*/

    /* store the test step result */
    partition_buffer->p_pass &= res;

    /* print error line */
    if (FAILURE == res) {

        pprintf("p%i (%s:%i) : %s failed\n",
                partition.index, file, line, cond);
    }

    /* increment test step if required */
    if (partition_buffer->iterations == 0 &&
        (partition_buffer->flags & SILENT) == 0) {

        test_control->step_id = partition_buffer->step_id + 1;
    }

    /* increment number of iterations */
    ++partition_buffer->iterations;
}

/**
 * @brief Finish test partition
 * @param result result of the test partition
 *
 * @note Passing SUCCESS as result after a test step fail will not mark the
 *       test as successful
 */
void test_finish(test_result result) {

    partition_buffer->p_done = 1;
    partition_buffer->p_pass &= result;
    for (;;);
}
