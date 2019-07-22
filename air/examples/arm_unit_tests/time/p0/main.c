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

    float s_per_tick = 1;//air_syscall_get_us_per_tick()/1000000;    air_disable_traps();
    int a = 1;
    ++a;

    pprintf("P0:: I am here at a: %d s\n", a);

    int i_tick;
    i_tick = (air_u32_t)air_syscall_get_elapsed_ticks();
    int tick;
    int time = s_per_tick * i_tick;

    pprintf("P0:: I am here at time: %ds\n", time);
    
    for(int i = 0; i < 0xffffff; ++i);

    tick = air_syscall_get_elapsed_ticks();
    pprintf("P0:: I am here at time: %ds\tafter %ds passed\n", tick+s_per_tick, (tick - i_tick)+s_per_tick);

    for(;;){}
}
