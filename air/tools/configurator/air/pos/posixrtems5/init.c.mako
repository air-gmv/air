<%
    import air
    import utils.templates as makoutils
%>\
${makoutils.cfileHeader("init.c", "Partition Initialization")}

#include <air.h>
#include <rtems.h>
%if 'imaspex' in partition.libraries:
#include <imaspex.h>
%endif

/**
 * @brief Partition entry point
 */
extern int ${partition.entry_point}() __attribute__ ((weak));
%if partition.hm_callback:
/**
 * @brief Partition health-monitor callback
 */
extern void ${partition.hm_callback}(
        air_state_e state_id,
        air_error_e error_id) __attribute__ ((weak));
%endif
%if partition.iop is not None:
/**
 * @brief IOP initialization
 */
extern rtems_status_code IOPinit();
%endif
/**
 * @brief Health-Monitor ISR handler
 */
static void hm_isr_handler(void) {

    /* get HM event */
    air_hm_event_t hm_event;
    air_syscall_get_hm_event(&hm_event);
%if partition.hm_callback:

    /* call partition HM callback */
    if (${partition.hm_callback} != NULL) {
        ${partition.hm_callback}(hm_event.state_id, hm_event.error_id);
    }
%endif
}

/**
 * @brief Main Init Posix RTEMS task 
 */
void *POSIX_Init(
  void *argument
)
{
    /* register HM ISR handler */
    /* HM is now Disabled
    rtems_isr_entry isr_ignored;
    rtems_interrupt_catch(
            (rtems_isr_entry)hm_isr_handler,
            AIR_IRQ_HM_EVENT,
            &isr_ignored);
    */

    %if 'imaspex' in partition.libraries:
    /* initialize IMASPEX */
    imaspex_init();

    %endif
    /* call entry point */
    if (${partition.entry_point} != NULL) {
        ${partition.entry_point}();
    }

}

#include <rtems_config.h>
