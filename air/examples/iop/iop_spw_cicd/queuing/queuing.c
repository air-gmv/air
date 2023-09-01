/*
 * Copyright (C) 2008-2021  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */

#include <rtems.h>

#include <rtems/rtems/clock.h>
#include <rtems/rtems/sem.h>
#include <rtems/rtems/tasks.h>

#include <a653.h>
#include <imaspex.h>

#include <pprintf.h>

#include <air.h>

#include <string.h>

int entry_func()
{

    RETURN_CODE_TYPE rc = NO_ERROR;
    int ticks_per_sec = 1000000 / air_syscall_get_us_per_tick();

    /*Announce Queuing partition entry*/
    PARTITION_ID_TYPE self_id;
    GET_PARTITION_ID(&self_id, &rc);
    pprintf("Hello from partition %d (Queuing)\n", self_id);

    /*Creating Destination queuing Port*/
    QUEUING_PORT_ID_TYPE QUEUE_PORT;
    pprintf("Creating Queueing Port. RET = %d\n", rc);
    CREATE_QUEUING_PORT("dqueuing", 1520, 32, DESTINATION, FIFO, &QUEUE_PORT, &rc);

    if (NO_ERROR != rc)
    {
        pprintf("CREATE_QUEUING_PORT error %d\n", rc);
        pprintf("Queuing partition caused abort.\n");
        air_syscall_shutdown_module();
    }
    pprintf("Finished Creating Queueing Port. RET = %d\n", rc);

    char msg[1024] = "empty\0";
    MESSAGE_SIZE_TYPE len;

    while (1)
    {
        RECEIVE_QUEUING_MESSAGE(QUEUE_PORT, INFINITE_TIME_VALUE, msg, &len, &rc);
        if (rc == NO_ERROR)
        {
            pprintf("Partition %d says hello.\n", self_id);
            pprintf("Received %d bytes\n", len);
            pprintf("Message: %s\n", msg);
            break;
        }
    }

    const char expected[32] = "Test message.\0";

    // Set to something different from zero, so it becomes zero in strncmp.
    int comparison = 1;

    comparison = strncmp(msg, expected, len + 1);

    pprintf("Comparing with expected string: \"%s\"\n", expected);
    pprintf("strncmp = %d\n", comparison);

    if (comparison == 0)
    {
        pprintf("Spacewire test successful, exiting.\n");
    }
    else
    {
        pprintf("Spacewire test failed.\n");
    }

    air_syscall_shutdown_module();
}
