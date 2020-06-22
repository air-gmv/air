/*
 * Copyright (C) 2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * https://spass-git-ext.gmv.com/AIR/AIR/raw/master/air/LICENSE
 */
/**
 * @file
 * @author bmgo
 * @brief move data to shared area
 */

#include <air.h>
#include <string.h>
#include <rtems.h>

char message[] = {
    0x48,0x65,0x6c,0x6c,0x6f,0x21,0x20,0x49,0x73,0x20,0x69,0x74,0x20,0x6d,0x65,
    0x20,0x79,0x6f,0x75,0x27,0x72,0x65,0x20,0x6c,0x6f,0x6f,0x6b,0x69,0x6e,0x67,
    0x20,0x66,0x6f,0x72,0x3f,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x2e,0x37,0x37,0x6f,0x30,0x55,0x40,0x57,
    0x45,0x53,0x4e,0x57,0x40,0x6d,0x79,0x6f,0x5a,0x3b,0x2c,0x2e,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0a,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x3b,0x4e,0x40,0x57,0x40,0x40,
    0x40,0x57,0x40,0x57,0x40,0x40,0x40,0x57,0x40,0x57,0x40,0x57,0x40,0x57,0x40,
    0x57,0x40,0x57,0x40,0x40,0x57,0x2c,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x40,0x57,0x40,0x57,0x40,0x40,0x40,0x57,0x40,0x57,0x40,0x40,0x40,0x40,0x40,
    0x57,0x40,0x40,0x40,0x57,0x40,0x57,0x40,0x57,0x40,0x40,0x40,0x57,0x40,0x4e,
    0x6f,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0a,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x41,0x40,0x40,0x40,0x57,0x40,0x57,0x40,0x40,0x40,
    0x57,0x40,0x40,0x40,0x57,0x40,0x40,0x40,0x57,0x40,0x57,0x40,0x57,0x40,0x57,
    0x40,0x57,0x40,0x57,0x40,0x40,0x40,0x40,0x6d,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x6d,0x40,0x57,0x40,
    0x57,0x40,0x57,0x40,0x57,0x40,0x40,0x40,0x57,0x40,0x57,0x40,0x57,0x40,0x57,
    0x40,0x57,0x40,0x57,0x40,0x57,0x40,0x57,0x40,0x57,0x40,0x40,0x40,0x57,0x40,
    0x40,0x40,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0a,0x20,0x20,0x20,
    0x20,0x3b,0x57,0x40,0x40,0x40,0x57,0x40,0x57,0x40,0x57,0x40,0x40,0x40,0x57,
    0x40,0x40,0x40,0x57,0x40,0x57,0x40,0x40,0x40,0x40,0x40,0x57,0x40,0x57,0x40,
    0x57,0x40,0x40,0x40,0x40,0x40,0x57,0x40,0x57,0x40,0x3a,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x0a,0x20,0x20,0x20,0x20,0x42,0x40,0x40,0x40,0x57,0x40,0x40,
    0x40,0x57,0x40,0x40,0x40,0x57,0x40,0x40,0x40,0x57,0x41,0x57,0x40,0x40,0x42,
    0x37,0x57,0x43,0x44,0x40,0x40,0x32,0x20,0x20,0x53,0x57,0x40,0x57,0x40,0x57,
    0x40,0x40,0x40,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0a,0x20,0x20,0x20,0x20,
    0x40,0x40,0x40,0x57,0x40,0x40,0x40,0x57,0x40,0x20,0x20,0x20,0x20,0x20,0x20,
    0x2e,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x2c,0x20,0x20,
    0x20,0x20,0x40,0x40,0x40,0x57,0x40,0x57,0x40,0x47,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x0a,0x20,0x20,0x20,0x2c,0x57,0x40,0x57,0x40,0x40,0x40,0x40,0x40,
    0x57,0x3b,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x55,0x57,0x40,0x57,0x40,0x57,0x40,
    0x40,0x40,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0a,0x20,0x20,0x20,0x20,0x40,
    0x57,0x40,0x57,0x40,0x57,0x40,0x57,0x40,0x54,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x40,0x40,0x40,0x57,0x40,0x57,0x40,0x54,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x0a,0x20,0x20,0x20,0x2c,0x57,0x40,0x40,0x40,0x57,0x40,0x57,0x40,0x57,
    0x2c,0x20,0x32,0x47,0x70,0x3b,0x37,0x37,0x2c,0x20,0x20,0x20,0x20,0x20,0x20,
    0x2e,0x43,0x44,0x47,0x44,0x79,0x69,0x20,0x40,0x40,0x57,0x40,0x57,0x40,0x40,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0a,0x20,0x20,0x20,0x20,0x37,0x57,
    0x40,0x40,0x40,0x57,0x5a,0x4f,0x40,0x20,0x20,0x20,0x54,0x43,0x40,0x57,0x40,
    0x57,0x3a,0x2e,0x20,0x20,0x2e,0x2c,0x79,0x44,0x40,0x57,0x6f,0x4b,0x2e,0x20,
    0x69,0x37,0x63,0x57,0x40,0x40,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x0a,0x20,0x20,0x20,0x20,0x20,0x40,0x57,0x40,0x57,0x40,0x49,0x20,0x43,0x20,
    0x20,0x20,0x20,0x2c,0x37,0x37,0x20,0x2e,0x20,0x3a,0x20,0x20,0x37,0x2e,0x20,
    0x20,0x2e,0x2c,0x20,0x20,0x20,0x20,0x20,0x20,0x40,0x40,0x57,0x40,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x69,
    0x40,0x40,0x57,0x40,0x3b,0x20,0x2e,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x3b,0x2e,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x3a,
    0x4b,0x40,0x57,0x40,0x40,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0a,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x41,0x40,0x40,0x57,0x40,0x32,0x37,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x2e,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x57,0x40,0x57,0x57,0x53,0x69,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x2e,0x57,
    0x40,0x57,0x40,0x57,0x40,0x47,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x2c,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x3a,0x20,0x20,0x20,0x20,0x20,0x2e,0x40,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0a,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x79,0x57,0x40,0x40,0x40,0x57,0x40,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x37,0x40,0x40,0x40,0x40,0x57,0x40,0x4b,0x3a,0x20,
    0x20,0x20,0x20,0x20,0x47,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x41,0x40,0x57,0x40,0x57,0x40,0x20,0x20,0x20,0x20,0x32,0x40,0x57,0x40,0x40,
    0x47,0x4f,0x42,0x40,0x40,0x57,0x40,0x53,0x20,0x20,0x30,0x6f,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0a,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x2e,0x20,0x3a,0x37,
    0x20,0x20,0x37,0x4e,0x3b,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x37,0x79,0x3a,
    0x44,0x69,0x2e,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x2e,0x63,0x2c,0x20,0x20,0x20,0x2e,0x20,0x20,
    0x20,0x20,0x2e,0x2e,0x20,0x20,0x2c,0x30,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0a,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x3b,0x37,0x20,0x20,0x20,0x20,0x2e,0x2e,0x20,0x20,0x20,0x20,0x3b,0x69,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x37,0x3b,0x2c,0x20,0x20,0x20,
    0x20,0x2e,0x3a,0x79,0x37,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0a,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x2e,0x2c,0x3a,0x3b,0x3a,0x3a,0x2c,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x00 };

/**
 * @brief Partition entry point
 */
void entry_point() {
    int ticks_per_sec = 1000000 / air_syscall_get_us_per_tick();
    air_sharedmemory_t sharedmemory;
    air_partition_status_t partition;

    /* get partition configuration */
    air_syscall_get_partition_status(-1, &partition);

    /* get shared memory area */
    if (air_syscall_get_sharedmemory("DEBUG", &sharedmemory) == AIR_NO_ERROR) {
        
        memcpy(sharedmemory.address, (void *)message, sizeof(message));

    /* shared memory not available to run the test... */
    } else {

        /* shutdown module */
        air_syscall_shutdown_module();
        
    }
    rtems_task_wake_after(ticks_per_sec/10);
    air_syscall_shutdown_module();
}
