/**
 * @file
 * @author pfnf
 * @brief Hello! Is it me you're looking for?
 */

#include <air.h>
#include <pprintf.h>

char message[] = "I am here at time: ";

/**
 * @brief Partition entry point
 */
void entry_point() {

    float ms_per_tick = 1.5;//air_syscall_get_us_per_tick()/1000;
    int a = 0;
    int i;
    ++a;

    pprintf("P0:: I am here at partition %d \n", a);

    unsigned long long i_tick;
    i_tick = (unsigned long long)air_syscall_get_elapsed_ticks();
    unsigned long long tick = i_tick;
    unsigned long long i_time = (unsigned long long)(i_tick * ms_per_tick);
	unsigned long long time = i_time;
	int d_tick;
	int d_time;

    pprintf("P0:: I am here at tick(time): %ld(%ldms)\n",
    		tick, time);

    
    for(i = 0; i < 0xffffff; ++i);

    tick = (unsigned long long)air_syscall_get_elapsed_ticks();
    time = (unsigned long long)(tick*ms_per_tick);
    d_tick = (int)(tick - i_tick);
    d_time = (int)(d_tick*ms_per_tick);
    pprintf("P0:: I am here at tick(time): %ld(%ldms)\tafter %d(%dms) passed\n",
    		tick, time, d_tick, d_time);

    for(i = 0; i < 0x1ffffff; ++i);

    tick = (unsigned long long)air_syscall_get_elapsed_ticks();
    time = (unsigned long long)(tick*ms_per_tick);
    d_tick = (int)(tick - i_tick);
    d_time = (int)(d_tick*ms_per_tick);
    pprintf("P0:: I am here at tick(time): %ld(%ldms)\tafter %d(%dms) passed\n",
    		tick, time, d_tick, d_time);

    for(i = 0; i < 0x4ffffff; ++i);

    tick = (unsigned long long)air_syscall_get_elapsed_ticks();
    time = (unsigned long long)(tick*ms_per_tick);
    d_tick = (int)(tick - i_tick);
    d_time = (int)(d_tick*ms_per_tick);
    pprintf("P0:: I am here at tick(time): %ld(%ldms)\tafter %d(%dms) passed\n",
    		tick, time, d_tick, d_time);

    for(i = 0; i < 0x6ffffff; ++i);

    tick = (unsigned long long)air_syscall_get_elapsed_ticks();
    time = (unsigned long long)(tick*ms_per_tick);
    d_tick = (int)(tick - i_tick);
    d_time = (int)(d_tick*ms_per_tick);
    pprintf("P0:: I am here at tick(time): %ld(%ldms)\tafter %d(%dms) passed\n",
    		tick, time, d_tick, d_time);

    for(i = 0; i < 0xfffff; ++i);

    tick = (unsigned long long)air_syscall_get_elapsed_ticks();
    time = (unsigned long long)(tick*ms_per_tick);
    d_tick = (int)(tick - i_tick);
    d_time = (int)(d_tick*ms_per_tick);
    pprintf("P0:: I am here at tick(time): %ld(%ldms)\tafter %d(%dms) passed\n",
    		tick, time, d_tick, d_time);

    for(i = 0; i < 0x2ffffff; ++i);

    tick = (unsigned long long)air_syscall_get_elapsed_ticks();
    time = (unsigned long long)(tick*ms_per_tick);
    d_tick = (int)(tick - i_tick);
    d_time = (int)(d_tick*ms_per_tick);
    pprintf("P0:: I am here at tick(time): %ld(%ldms)\tafter %ld(%ldms) passed\n",
    		tick, time, d_tick, d_time);

    for(i = 0; i < 0x2ffffff; ++i);

    tick = (unsigned long long)air_syscall_get_elapsed_ticks();
    time = (unsigned long long)(tick*ms_per_tick);
    d_tick = (int)(tick - i_tick);
    d_time = (int)(d_tick*ms_per_tick);
    pprintf("P0:: I am here at tick(time): %ld(%ldms)\tafter %ld(%ldms) passed\n",
    		tick, time, d_tick, d_time);

    for(;;){}
}
