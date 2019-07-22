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
	int a = 0;
	int i;
	++a;

//	air_disable_fpu();
	for(i = 0; i < 0xfffffff; ++i);
	int zero = 0;
	int fzero = 0;

	pprintf("here\n");
	pprintf("P0:: I am here at partition %d. ms/tick = %ld, *2 %ld, +1 %ld, /2 %ld \n",
			a, us_per_tick, (us_per_tick*2), (us_per_tick+1), (us_per_tick/2));

	air_u64_t i_tick, i_time, tick, time, t_tick;
	air_u64_t d_tick;
	air_u64_t d_time;

	i_tick = air_syscall_get_elapsed_ticks();
	i_time = (air_u64_t)(i_tick * us_per_tick);

	tick = (i_tick >> 32);

	pprintf("P0:: I am here at tick(time): %ld(%ldms)   %ld\n",
			i_tick, i_time, tick);


	for(i = 0; i < 0x2fffffff; ++i);


	tick = air_syscall_get_elapsed_ticks();
	time = (air_u64_t)(tick*us_per_tick);
	d_tick = (tick - i_tick);
	d_time = (air_u64_t)(d_tick*us_per_tick);

	pprintf("P0:: /0 = %x, %x I am here at tick(time): %ld(%ldms)  after %ld(%ldms) passed\n",
			(air_u32_t)zero, (air_u32_t)fzero, tick, time, d_tick, d_time);

	for(;;){}
}
