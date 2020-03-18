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

    double us_per_tick = air_syscall_get_us_per_tick();
    int a = 0;
    int i;
    ++a;

    pprintf("P0:: I am here at partition %d. ms/tick = %d, %d, %d \n",
    		a, (int)us_per_tick, (int)(us_per_tick*2), (int)(us_per_tick+1));

    air_u64_t i_tick, i_time, t_tick;
    air_u64_t tick = 0;
    air_u64_t time = 0;
    air_u32_t d_tick, d_time;

    i_tick = air_syscall_get_elapsed_ticks();
    i_time = (air_u64_t)(i_tick * us_per_tick);
    pprintf("P0:: I am here at tick(time): %ld(%ldms)\n",
    		i_tick, i_time);

//    air_disable_fpu();
    for(i = 0; i < 0xffffff; ++i);

    tick = air_syscall_get_elapsed_ticks();
    time = (air_u64_t)(tick*us_per_tick);
    d_tick = (air_u32_t)(tick - i_tick);
    d_time = (air_u32_t)(d_tick*us_per_tick);
    pprintf("P0:: I am here at tick(time): %ld(%ldms)  after %d(%dms) passed\n",
    		tick, time, d_tick, d_time);

    for(i = 0; i < 0x1ffffff; ++i);

    t_tick = air_syscall_get_elapsed_ticks();
    time = (air_u64_t)(t_tick*us_per_tick);
    d_tick = (air_u32_t)(t_tick - tick);
    d_time = (air_u32_t)(d_tick*us_per_tick);
    pprintf("P0:: I am here at tick(time): %ld(%ldms)  after %d(%dms) passed\n",
    		t_tick, time, d_tick, d_time);

    for(i = 0; i < 0x4ffffff; ++i);

    tick = air_syscall_get_elapsed_ticks();
    time = (air_u64_t)(tick*us_per_tick);
    d_tick = (air_u32_t)(tick - t_tick);
    d_time = (air_u32_t)(d_tick*us_per_tick);
    pprintf("P0:: I am here at tick(time): %ld(%ldms)  after %d(%dms) passed\n",
    		tick, time, d_tick, d_time);

    for(i = 0; i < 0x6ffffff; ++i);

    t_tick = air_syscall_get_elapsed_ticks();
    time = (air_u64_t)(t_tick*us_per_tick);
    d_tick = (air_u32_t)(t_tick - tick);
    d_time = (air_u32_t)(d_tick*us_per_tick);
    pprintf("P0:: I am here at tick(time): %ld(%ldms)  after %d(%dms) passed\n",
    		t_tick, time, d_tick, d_time);

    for(i = 0; i < 0xfffff; ++i);

    tick = air_syscall_get_elapsed_ticks();
    time = (air_u64_t)(tick*us_per_tick);
    d_tick = (air_u32_t)(tick - t_tick);
    d_time = (air_u32_t)(d_tick*us_per_tick);
    pprintf("P0:: I am here at tick(time): %ld(%ldms)  after %d(%dms) passed\n",
    		tick, time, d_tick, d_time);

    for(i = 0; i < 0x2ffffff; ++i);

    t_tick = air_syscall_get_elapsed_ticks();
    time = (air_u64_t)(t_tick*us_per_tick);
    d_tick = (air_u32_t)(t_tick - tick);
    d_time = (air_u32_t)(d_tick*us_per_tick);
    pprintf("P0:: I am here at tick(time): %ld(%ldms)  after %ld(%ldms) passed\n",
    		t_tick, time, d_tick, d_time);

    for(i = 0; i < 0x2ffffff; ++i);

    tick = air_syscall_get_elapsed_ticks();
    time = (air_u64_t)(tick*us_per_tick);
    d_tick = (air_u32_t)(tick - t_tick);
    d_time = (air_u32_t)(d_tick*us_per_tick);
    pprintf("P0:: I am here at tick(time): %ld(%ldms)  after %ld(%ldms) passed\n",
    		tick, time, d_tick, d_time);

    for(;;);
}
