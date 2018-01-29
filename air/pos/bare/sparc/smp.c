/**
 * @file smp.c
 * @author pfnf
 * @brief Bare SMP support
 */

#include <asm.h>
#include <air.h>
#include <cpu.h>
#include <sparc.h>

#define CONFIG_MAX_CORES                   (4)
#define CORE_STACK_SIZE                    (30 * 0x60)
/**
 * @brief Core entry point
 */
volatile void *smp_entry_point[CONFIG_MAX_CORES];
/**
 * @brief Core trap ISR tables
 */
volatile air_u32_t *isr_table[CONFIG_MAX_CORES];
/**
 * @brief Space for the ISR tables
 */
air_u32_t __isr_space[CONFIG_MAX_CORES * SPARC_TRAP_COUNT] = {0};

extern int main();
extern void bare_start();

void sparc_smp_init() {

    air_u32_t i;

    /* setup entry points */
    smp_entry_point[0] = (air_u32_t *)main;
    for (i = 1; i < CONFIG_MAX_CORES; ++i) {
        smp_entry_point[i] = (air_u32_t *)NULL;
    }

    /* setup ISR handler tables */
    for (i = 0; i < CONFIG_MAX_CORES; ++i) {
        isr_table[i] =
                ((air_u32_t *)&__isr_space[i * SPARC_TRAP_COUNT]);
    }
}

air_status_code_e boot_core(air_u32_t core_id, void *entry_point) {

    /* check if core id is valid */
    if (core_id < 1 || core_id > CONFIG_MAX_CORES) {
        return AIR_INVALID_CONFIG;
    }

    /* set core entry point */
    smp_entry_point[core_id] = (air_u32_t *)entry_point;

    /* boot core */
    return air_syscall_boot_core(core_id, bare_start);
}

void wake_after(air_clocktick_t ticks) {

    if (ticks <= 0) return;

    air_clocktick_t it = air_syscall_get_elapsed_ticks();
    while (1) {
        air_clocktick_t ft = air_syscall_get_elapsed_ticks();
        if ((ft - it) >= ticks) break;
    }
}
