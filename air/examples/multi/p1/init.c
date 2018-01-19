/*
 * ============================================================================
 * This file was generated by the GMV's Configurator Tool and is part of an
 * AIR - ARINC 653 Interface in RTEMS - application configuration.
 * File Purpose  : Partition Initialization
 * ============================================================================
 */

#include <air.h>
#include <rtems.h>
#include <imaspex.h>

/**
 * @brief Partition entry point
 */
extern int receiver() __attribute__ ((weak));
/**
 * @brief Partition health-monitor callback
 */
extern void partition_HM_callback(
        air_state_e state_id,
        air_error_e error_id) __attribute__ ((weak));
/**
 * @brief Health-Monitor ISR handler
 */
static void hm_isr_handler(void) {

    /* get HM event */
    air_hm_event_t hm_event;
    air_syscall_get_hm_event(&hm_event);

    /* call partition HM callback */
    if (partition_HM_callback != NULL) {
        partition_HM_callback(hm_event.state_id, hm_event.error_id);
    }
}

/**
 * @brief Main RTEMS task
 */
rtems_task Init(rtems_task_argument ignored) {

    /* register HM ISR handler */
    rtems_isr_entry isr_ignored;
    rtems_interrupt_catch(
            (rtems_isr_entry)hm_isr_handler,
            AIR_IRQ_HM_EVENT,
            &isr_ignored);

    /* initialize IMASPEX */
    imaspex_init();

    /* call entry point */
    if (receiver != NULL) {
        receiver();
    }

    rtems_task_delete(RTEMS_SELF);
}

#include <rtems_config.h>