/*
 * Copyright (C) 2008-2013  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */

#include <air.h>
#include <rtems.h>
#include <pprintf.h>

int producer() {

    long pid;
    air_status_code_e rc;
    air_partition_status_t status;
    int x = 2;
    volatile int y = 0;

    rc = air_syscall_get_partition_id(NULL, &pid);
    if(AIR_NO_ERROR != rc)
    {
        printf("P0main air_syscall_get_partition_id ERROR: %u\n", rc);
    }

    rc = air_syscall_get_partition_status(pid, &status);
    if(AIR_NO_ERROR != rc)
    {
        printf("P0main air_syscall_get_partition_status ERROR: %u\n", rc);
    }

    /*Example on how to act differently if a HM restart action was selected*/
    if(status.start_condition != AIR_START_CONDITION_HM_PARTITION_RESTART)
    {
        /*Force a division by 0*/
        x = x/y;
    }

    uint32_t tps = 1000000 / air_syscall_get_us_per_tick();

    for (int i=0; i<3; i++)
    {
        printf("In partition PO loop\n");
        rtems_task_wake_after(tps/2);
        printf("P0 : Waking up %d\n", x);
    }

    return AIR_SUCCESSFUL;
}
