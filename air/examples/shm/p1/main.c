/*
 * Copyright (C) 2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @author bmgo
 * @brief print share memory area data
 */

#include <air.h>

/**
 * @brief Partition entry point
 */
void entry_point()
{

    air_sharedmemory_t sharedmemory;

    /* get shared memory area */
    if (air_syscall_get_sharedmemory("DEBUG", &sharedmemory) == AIR_NO_ERROR)
    {

        /* shared memory not available to run the test... */
    }
    else
    {

        /* shutdown module */
        air_syscall_shutdown_module();
    }

    char *message = (char *)sharedmemory.address;

    for (;;)
    {
        while (!*message)
        {
            ;
        }

        air_syscall_print(message, 1);
        message++;
    }
}
