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
%if os_configuration.arch == "arm":
#include <rtems/score/armv4.h>
#include <libcpu/arm-cp15.h>
%endif

/**
 * @brief Partition entry point
 */
extern int ${partition.entry_point}() __attribute__ ((weak));

/**
 * @brief Install the HM handler
 */
%if os_configuration.arch == "arm":
void hm_handler_install(void *handler){
    arm_cp15_set_exception_handler(ARM_EXCEPTION_DATA_ABORT, &_ARMV4_Exception_data_abort);
    _ARMV4_Exception_data_abort_set_handler(handler);
    arm_cp15_set_exception_handler(ARM_EXCEPTION_PREF_ABORT, &_ARMV4_Exception_prefetch_abort);
    _ARMV4_Exception_prefetch_abort_set_handler(handler);
    arm_cp15_set_exception_handler(ARM_EXCEPTION_UNDEF, &_ARMV4_Exception_prefetch_abort);
    _ARMV4_Exception_prefetch_abort_set_handler(handler);
}
%else:
void hm_handler_install(void *handler){

    rtems_isr_entry isr_ignored;
    rtems_interrupt_catch(
            (rtems_isr_entry)handler,
            AIR_IRQ_HM_EVENT,
            &isr_ignored);
}
%endif 

%if partition.iop is not None:
/**
 * @brief IOP initialization
 */
extern rtems_status_code IOPinit();
%endif

%if partition.hm_callback:
/**
 * @brief Partition health-monitor callback
 */
extern void ${partition.hm_callback}(
        air_state_e state_id,
        air_error_e error_id) __attribute__ ((weak));
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
 * @brief Main RTEMS task
 */
rtems_task Init(rtems_task_argument ignored) {

    /* register HM ISR handler */
    hm_handler_install(hm_isr_handler);

    %if 'imaspex' in partition.libraries:
    /* initialize IMASPEX */
    imaspex_init();

    %endif
    %if partition.iop is not None:
    /* initialize IOP */
    IOPinit();
    %endif
    /* call entry point */
    if (${partition.entry_point} != NULL) {
        ${partition.entry_point}();
    }

#ifdef COVERAGE_ENABLED
    __gcov_exit();
#endif

    rtems_task_delete(RTEMS_SELF);
}

#include <rtems_config.h>


