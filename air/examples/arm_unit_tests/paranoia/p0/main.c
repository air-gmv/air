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
	pprintf("\n\n\nPARANOIA\n\n\n");
	pprintf("P101:: us_per_tick = %ldus\n", us_per_tick);
	air_u32_t ms_per_tick = (air_u32_t)us_per_tick/1000;
	pprintf("P0:: ms_per_tick = %ldms\n", ms_per_tick);
	int a = 0;
	int i;
	++a;

//	air_disable_fpu();
	for(i = 0; i < 0x10000; ++i);

	pprintf("P101:: I am here at partition %d. ms/tick = %d, *2 %d, +1 %d, /2 %d \n",
			a, ms_per_tick, (ms_per_tick*2), (ms_per_tick+1), (ms_per_tick/2));

	air_u64_t i_tick, i_time, tick;

	i_tick = air_syscall_get_elapsed_ticks();
	i_time = (air_u64_t)(i_tick * ms_per_tick);

	tick = (i_tick >> 32);

	pprintf("P101::1 I am here at tick(time): %ld(%ldms)   %ld\n",
			i_tick, i_time, tick);
	i=0;
	/*while(1){
	    pprintf("P1: i= %d\n", i);
	    i++;
	}*/
}
