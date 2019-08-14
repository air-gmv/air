/*
 * Copyright (C) 2008-2013  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/AIR/raw/master/air/LICENSE
 */

#include <air.h>
#include <rtems.h>
#ifdef RTEMS48I
    #include <pprintf.h>
#else
    #include <stdio.h>
#endif

void partition_HM_callback(air_state_e state_id,air_error_e error_id)
{
    printf("ENTERING partition_HM_callback\tSTATE:%d\tERROR:%d\n",state_id,error_id);

    /*implement some appropriate handler here*/
}

int receiver() {

    /*Force a division by 0*/
    int x = 2;
    volatile int y = 0;

    x = x/y;

    uint32_t tps = 1000000 / air_syscall_get_us_per_tick();

    for (;;)
    {
      #ifdef RTEMS48I
        pprintf("In partition P1 loop\n");
      #else
        printf("In partition P1 loop\n");
      #endif
        rtems_task_wake_after(tps/2);
      #ifdef RTEMS48I
        pprintf("P1 : Waking up - %d\n", x);
      #else
        printf("P1 : Waking up - %d\n", x);
      #endif
    }

    return AIR_SUCCESSFUL;
}
