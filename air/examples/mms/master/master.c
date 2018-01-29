
#include <air.h>
#include <bare.h>
#include <pprintf.h>

void entry_func() {

    air_schedule_status_t status;
    int i = 0, sched_a_id, sched_b_id, sched_id;
    air_syscall_get_schedule_id("schedA", &sched_a_id);
    air_syscall_get_schedule_id("schedB", &sched_b_id);
    while (1) {

	air_syscall_get_schedule_status(&status);
	pprintf("Currently on schedule %d\n", status.current_schedule_id);
	pprintf("Next Schedule: %d\n", status.next_schedule_id);

	if(3 == i) {
            if (status.current_schedule_id == sched_a_id) sched_id = sched_b_id;     
            if (status.current_schedule_id == sched_b_id) sched_id = sched_a_id;  
            air_syscall_set_schedule(sched_id);
	    pprintf("Changed next schedule to %d, starting on next MTF\n", sched_id);
            i = 0;
	}

        ++i;
        wake_after(50);
    }
}

