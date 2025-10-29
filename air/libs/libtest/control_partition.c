/*
 * Copyright (C) 2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file control_partition.c
 * @author pfnf
 * @brief Functions for the control partition
 */
#include <air.h>
#include <libtest.h>

/**
 * @brief Partition count
 */
air_u32_t partition_count;

/**
 * @brief Partition
 */
extern air_partition_status_t partition;
/**
 * @brief Test control area
 */
extern test_control_t *test_control;
/**
 * @brief Current partition buffer area
 */
extern partition_buffer_t *partition_buffer;

/**
 * @brief Flush partition buffer
 * @param p_buffer partition buffer to flush
 */
static air_u32_t flush_partition_buffer(partition_buffer_t *p_buffer)
{

    /* store the values locally */
    air_u32_t l_pos = p_buffer->l_pos;

    if ((p_buffer->p_done) != 0)
        printf("TEST| STEP ID %04x ITERATION %03x| RESULT %x\n", p_buffer->step_id, p_buffer->iterations,
               p_buffer->iterations, p_buffer->p_pass);

    /* write to UART */
    while (p_buffer->r_pos != l_pos)
    {

        air_i8_t ch = p_buffer->buffer[p_buffer->r_pos];

        /* write '\r' character */
        if (ch == '\n')
        {
            air_syscall_putchar('\r');
        }

        /* write 'ch' */
        air_syscall_putchar(ch);

        /* increment the reading position */
        p_buffer->r_pos = INCREMENT_POSITION(p_buffer->r_pos);
    }

    return (p_buffer->r_pos != l_pos);
}

/**
 * @brief Initializes the control partition
 * @param id current test id
 * @param shm_name name of the shared memory for the test report
 */
void control_partition_init(air_u32_t id, air_name_ptr_t shm_name)
{

    air_u32_t i;
    test_control = NULL;
    air_sharedmemory_t sharedmemory;
    /* get partition configuration */
    air_syscall_get_partition_status(-1, &partition);

    /* get shared memory area */
    if (air_syscall_get_sharedmemory(shm_name, &sharedmemory) == AIR_NO_ERROR)
    {

        /* get control structures */
        test_control = (test_control_t *)sharedmemory.address;
        test_control->buffers =
            (partition_buffer_t *)ADDR_ALIGN((air_uptr_t)sharedmemory.address + sizeof(test_control_t), 0x40);

        /* initialize each partition buffer */
        for (i = 0; i < partition.index + 1; ++i)
        {
            partition_buffer_t *p_buffer = &test_control->buffers[i];
            p_buffer->l_pos = 0;
            p_buffer->w_pos = 0;
            p_buffer->r_pos = 0;
            p_buffer->p_done = 0;
            p_buffer->p_pass = 1;
            p_buffer->iterations = 0;
            p_buffer->step_id = ~0;
        }

        /* get current partition buffer */
        partition_buffer = &test_control->buffers[partition.index];

        /* test header */
        pprintf("STARTING TEST [ %06i ]\n", id);
        // flush_partition_buffer(partition_buffer);

        /* allow other partitions to continue */
        test_control->test_id = id;
        test_control->step_id = 0;

        /* shared memory not available to run the test... */
    }
    else
    {

        /* shutdown module */
        air_syscall_shutdown_module();
        for (;;)
        {
            ;
        }
    }
    /* loop to print the text results */
    while (1)
    {

        air_u32_t i;
        volatile air_u32_t all_done = 1, all_pass = 1;

        /* loop through all partition */
        for (i = 0; i < partition.index; ++i)
        {

            /* get partition 'i' buffer */
            partition_buffer_t *p_buffer = &test_control->buffers[i];

            all_done &= p_buffer->p_done;
            all_pass &= p_buffer->p_pass;
        }

        /* check if all test partitions are done */
        if (1 == all_done)
        {

            /* present the test final result */
            if (1 == all_pass)
            {

                pprintf("END OF TEST ALL PASS\n");
            }
            else
            {

                pprintf("END OF TEST FAILURES DETECTED\n");
            }

            /* shutdown module */
            air_syscall_shutdown_module();
            for (;;)
            {
                ;
            }
        }
    }
}
