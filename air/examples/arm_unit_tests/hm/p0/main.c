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

	air_u64_t us_per_tick = air_syscall_get_us_per_tick();

	pprintf("P0:: us_per_tick = %ldus\n", us_per_tick);
	air_u32_t ms_per_tick = (air_u32_t)us_per_tick/1000;
	pprintf("P0:: ms_per_tick = %ldms\n", ms_per_tick);
	int a = 0;
	int i;
	++a;

//	air_disable_fpu();
	for(i = 0; i < 0x10000; ++i);
	int zero = 0;
	int fzero = 0;

	pprintf("P0:: I am here at partition %d. ms/tick = %d, *2 %d, +1 %d, /2 %d \n",
			a, ms_per_tick, (ms_per_tick*2), (ms_per_tick+1), (ms_per_tick/2));

	air_u64_t i_tick, i_time, tick, time, t_tick;
	air_u64_t d_tick;
	air_u64_t d_time;

	i_tick = air_syscall_get_elapsed_ticks();
	i_time = (air_u64_t)(i_tick * ms_per_tick);

	tick = (i_tick >> 32);

	pprintf("P0::1 I am here at tick(time): %ld(%ldms)   %ld\n",
			i_tick, i_time, tick);


	for(i = 0; i < 0x6000000; ++i);

//	int *p = 0;
//	*p = 0;

	double d1 = 1/2;

	tick = air_syscall_get_elapsed_ticks();
	time = (air_u64_t)(tick*ms_per_tick);
	d_tick = (tick - i_tick);
	d_time = (air_u64_t)(d_tick*ms_per_tick);

	pprintf("P0::2 /0 = %x, %x I am here at tick(time): %ld(%ldms)  after %ld(%ldms) passed\n",
			(air_u32_t)zero, (air_u32_t)fzero, tick, time, d_tick, d_time);

	for(i = 0; i < 0x6000000; ++i);

	t_tick = air_syscall_get_elapsed_ticks();
	time = (air_u64_t)(t_tick*ms_per_tick);
	d_tick = (t_tick - tick);
	d_time = (air_u64_t)(d_tick*ms_per_tick);

	pprintf("P0::3 /0 = %x, %x I am here at tick(time): %ld(%ldms)  after %ld(%ldms) passed\n",
			(air_u32_t)zero, (air_u32_t)fzero, t_tick, time, d_tick, d_time);

	for(;;){}
}
