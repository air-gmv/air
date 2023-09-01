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

int entry_func()
{
    RETURN_CODE_TYPE rc = NO_ERROR;

    /*Announce Sampling partition entry*/
    PARTITION_ID_TYPE self_id;
    GET_PARTITION_ID(&self_id, &rc);
    pprintf("Hello from partition %d (Sampling)\n", self_id);

    /*Creating Source sampling Port*/
    SAMPLING_PORT_NAME_TYPE NAME = "ssampling";
    MESSAGE_SIZE_TYPE SIZE = 1520;
    SYSTEM_TIME_TYPE PERIOD = 1000000000ll;
    SAMPLING_PORT_ID_TYPE SEND_PORT;
    pprintf("Creating Sampling Port.");
    CREATE_SAMPLING_PORT(NAME, SIZE, SOURCE, PERIOD, &SEND_PORT, &rc);

    if (NO_ERROR != rc)
    {
        pprintf("CREATE_SAMPLING_PORT error %d\n", rc);
        pprintf("Sampling partition caused abort.\n");
        air_syscall_shutdown_module();
    }
    pprintf("Finished Creating Sampling Port\n");

    char message[32] = "Test message.\0";

    pprintf("Partition %d sending: \"%s\"\n", self_id, message);
    WRITE_SAMPLING_MESSAGE(SEND_PORT, (MESSAGE_ADDR_TYPE)message, 17, &rc);
    if (NO_ERROR != rc)
    {
        pprintf("WRITE_SAMPLING_MESSAGE error %d\n", rc);
        pprintf("Sampling partition caused abort.");
        air_syscall_shutdown_module();
    }

    pprintf("Sampling Partition done\n");
    return RTEMS_SUCCESSFUL;
}
