/**
 * @file smp.c
 * @author pfnf
 * @brief Bare SMP support
 */

#include <asm.h>
#include <xky.h>
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
volatile xky_u32_t *isr_table[CONFIG_MAX_CORES];
/**
 * @brief Space for the ISR tables
 */
xky_u32_t __isr_space[CONFIG_MAX_CORES * SPARC_TRAP_COUNT] = {0};

extern int main();
extern void bare_start();

void sparc_smp_init() {

    xky_u32_t i;

    /* setup entry points */
    smp_entry_point[0] = (xky_u32_t *)main;
    for (i = 1; i < CONFIG_MAX_CORES; ++i) {
        smp_entry_point[i] = (xky_u32_t *)NULL;
    }

    /* setup ISR handler tables */
    for (i = 0; i < CONFIG_MAX_CORES; ++i) {
        isr_table[i] =
                ((xky_u32_t *)&__isr_space[i * SPARC_TRAP_COUNT]);
    }
}

xky_status_code_e boot_core(xky_u32_t core_id, void *entry_point) {

    /* check if core id is valid */
    if (core_id < 1 || core_id > CONFIG_MAX_CORES) {
        return XKY_INVALID_CONFIG;
    }

    /* set core entry point */
    smp_entry_point[core_id] = (xky_u32_t *)entry_point;

    /* boot core */
    return xky_syscall_boot_core(core_id, bare_start);
}

void wake_after(xky_clocktick_t ticks) {

    if (ticks <= 0) return;

    xky_clocktick_t it = xky_syscall_get_elapsed_ticks();
    while (1) {
        xky_clocktick_t ft = xky_syscall_get_elapsed_ticks();
        if ((ft - it) >= ticks) break;
    }
}
