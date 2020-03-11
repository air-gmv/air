/**
 * @file
 * @author pfnf
 * @brief Hello! Is it me you're looking for?
 */

#include <air.h>
#include <stdio.h>

char message[] = "I am here at time: ";


/**
 * @brief Partition entry point
 */
void entry_point() {

	air_u64_t us_per_tick = air_syscall_get_us_per_tick();
	printf("P1:: us_per_tick = %lluus\n", us_per_tick);

	air_i32_t ms_per_tick = (air_u32_t)us_per_tick/1000;
	printf("P1:: ms_per_tick = %ldms\n", ms_per_tick);

    int p = 1;

	for(int i = 0; i < 0x100000; ++i);

	printf("P1:: Sanity check. I am here at partition %d. ms/tick = %ld, *2 %ld, +1 %ld, /2 %ld \n",
			p, ms_per_tick, (ms_per_tick*2), (ms_per_tick+1), (ms_per_tick/2));

	air_u64_t i_tick, i_time, tick, time;

	i_tick = air_syscall_get_elapsed_ticks();
	i_time = (air_u64_t)(i_tick * ms_per_tick);

	printf("P1:: I am here at tick(time): %llu(%llums)\n", i_tick, i_time);

	for(int i = 0; i < 0x100000; ++i);
    tick = air_syscall_get_elapsed_ticks();
    time = (air_u64_t)(tick * ms_per_tick);
	printf("P1:: I am here at tick(time) after d_ticks: %llu(%llums) %llu\n", tick, time, tick - i_tick);

	for(int i = 0; i < 0x200000; ++i);
    tick = air_syscall_get_elapsed_ticks();
    time = (air_u64_t)(tick * ms_per_tick);
	printf("P1:: I am here at tick(time) after d_ticks: %llu(%llums) %llu\n", tick, time, tick - i_tick);

	for(int i = 0; i < 0x400000; ++i);
    tick = air_syscall_get_elapsed_ticks();
    time = (air_u64_t)(tick * ms_per_tick);
	printf("P1:: I am here at tick(time) after d_ticks: %llu(%llums) %llu\n", tick, time, tick - i_tick);

	for(;;){}
}
