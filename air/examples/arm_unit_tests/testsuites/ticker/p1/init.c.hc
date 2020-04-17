/*
 * ============================================================================
 * This file was generated by the GMV's Configurator Tool and is part of an
 * AIR - ARINC 653 Interface in RTEMS - application configuration.
 * File Purpose  : Partition Initialization
 * ============================================================================
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
//#include "system.h"
#include <stdio.h>
#include <tmacros.h>
#include <pprintf.h>
#include <rtems/irq-extension.h>

/**
 * @brief Partition entry point
 */
extern int entry_point() __attribute__ ((weak));


/**
 * @brief Health-Monitor ISR handler
 */
static void hm_isr_handler(void) {

    /* get HM event */
    air_hm_event_t hm_event;
    air_syscall_get_hm_event(&hm_event);
}

/**
 * @brief Main RTEMS task
 */
rtems_task Init(rtems_task_argument ignored) {
    pprintf("\nInit init\n");
    /* register HM ISR handler */
    rtems_interrupt_handler_install(
            AIR_IRQ_HM_EVENT,
        "HM",
        RTEMS_INTERRUPT_UNIQUE,
        (rtems_interrupt_handler) hm_isr_handler,
        NULL
      );
    pprintf("entry point\n");
    /* call entry point */
    if (entry_point != NULL) {
        entry_point();
    }
    pprintf("END\n");
    while(1){}
    rtems_task_delete(RTEMS_SELF);
}

//#include <rtems_config.h>

