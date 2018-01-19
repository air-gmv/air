/**
 *  @file
 *  @author pfnf
 *  @brief Clock device initialization and handling functions
 */

#include <pmk.h>
#include <bsp.h>
#include <sparc.h>
#include <cpu.h>
#include <ipc.h>
#include <amba.h>
#include <clock.h>
#include <irqmp.h>
#include <configurations.h>

/**
 * @brief AIR shared area (defined in multicore.c)
 */
extern pmk_sharedarea_t air_shared_area;

/**
 * @brief IPC trap handler
 * @param isf Executing stack frame
 * @param core Executing core control structure
 */
void pmk_ipc_handler(void *isf, pmk_core_ctrl_t *core);

/**
 * @brief Timer enable control bit
 * @ingroup bsp_leon_clock
 */
#define TIMER_EN                                (0x00000001u)
/**
 * @brief Timer reload at zero control bit
 * @ingroup bsp_leon_clock
 */
#define TIMER_RL                                (0x00000002u)
/**
 * @brief Timer load counter control bit
 * @ingroup bsp_leon_clock
 */
#define TIMER_LD                                (0x00000004u)
/**
 * @brief Timer enable interrupt control bit
 * @ingroup bsp_leon_clock
 */
#define TIMER_IRQEN                             (0x00000008u)

/**
 * @brief LEON registers per timer
 */
typedef struct {

   volatile air_u32_t value;
   volatile air_u32_t reload;
   volatile air_u32_t conf;
   volatile air_u32_t dummy;

} timer_subtype_t;

/**
 * @brief LEON timer register map
 */
typedef struct {

   volatile air_u32_t scaler_value;
   volatile air_u32_t scaler_reload;
   volatile air_u32_t status;
   volatile air_u32_t dummy;
   timer_subtype_t timer[8];

} timer_regmap_t;

/**
 * @brief Timer control
 */
timer_ctrl_t timer_ctrl;


void bsp_clock_start(void) {

    /* */
    air_u32_t i;

    /* install scheduler service routine */
    sparc_install_vector_trap_handler(timer_ctrl.irq, pmk_partition_scheduler);

    /* setup priorities */
    irqmp_interrupt_set_priority(timer_ctrl.irq, 1);
    irqmp_interrupt_set_priority(BSP_IPC_IRQ, 1);
    irqmp_interrupt_set_priority(BSP_IPC_PCS, 1);

    /* setup cores interrupts */
    irqmp_enable_interrupt(0, timer_ctrl.irq);
    for (i = 0; i < air_shared_area.configured_cores; ++i) {
        irqmp_enable_interrupt(i, BSP_IPC_IRQ);
        irqmp_enable_interrupt(i, BSP_IPC_PCS);
    }

    /* install handler for the IPC */
    sparc_install_vector_trap_handler(BSP_IPC_IRQ, pmk_ipc_handler);

    /* install handler for the IPC */
    sparc_install_vector_trap_handler(BSP_IPC_PCS, pmk_partition_scheduler);

    /* configure and arm timer */
    volatile timer_regmap_t *timer_regs = (volatile timer_regmap_t *)timer_ctrl.regs;
    timer_regs->timer[0].reload = pmk_get_usr_us_per_tick() - 1;
    timer_regs->timer[0].conf = TIMER_EN | TIMER_RL | TIMER_LD | TIMER_IRQEN;
    return;
} 

int clock_init(void) {

    amba_apb_dev_t timer_device;
    volatile timer_regmap_t * timer_regs;

    /* look for the GPTIMER0 */
    if (amba_get_apb_slave(
            &amba_confarea, VENDOR_GAISLER, GAISLER_GPTIMER, 0, &timer_device) > 0){

        /* get configuration registers and time interrupt number */
        timer_regs = (volatile timer_regmap_t *)timer_device.start;
        timer_ctrl.regs = (void *)timer_regs;
        timer_ctrl.irq = ((timer_regs->status & 0xFC) >> 3) + 0x10;
        timer_ctrl.frequency = (timer_regs->scaler_reload + 1) * 1000000;
        return 0;
    }

    return 1;
}