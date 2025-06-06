/*
 * Copyright (C) 2008-2013  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * $(AIR_GIT_REMOTE_URL)/AIR/AIR/raw/master/air/LICENSE
 */

#include <air.h>
#include <bare.h>
#include <pprintf.h>

void entry_func() {

    air_schedule_status_t status;
    long int sched_a_id, sched_b_id, sched_id;
    air_syscall_get_schedule_id("schedA", &sched_a_id);
    air_syscall_get_schedule_id("schedB", &sched_b_id);
    for (int i = 1; i <12; i++) {

        air_syscall_get_schedule_status(&status);
        pprintf("Currently on schedule %d\n", status.current_schedule_id);
        pprintf("Next Schedule: %d\n", status.next_schedule_id);

        if(i % 3 == 0) {
                if (status.current_schedule_id == sched_a_id) sched_id = sched_b_id;     
                if (status.current_schedule_id == sched_b_id) sched_id = sched_a_id;  
                air_syscall_set_schedule(sched_id);
            pprintf("Changed next schedule to %d, starting on next MTF\n", sched_id);
        }

        ++i;
        wake_after(50);
    }
    air_syscall_shutdown_module();
}

