<%
    import xky
    import utils.templates as makoutils
%>\
${makoutils.cfileHeader("init.c", "Partition Initialization")}

#include <bare.h>
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
        xky_state_e state_id,
        xky_error_e error_id) __attribute__ ((weak));

/**
 * @brief Health-Monitor ISR handler
 */
static void hm_isr_handler(void) {

    /* get HM event */
    xky_hm_event_t hm_event;
    xky_syscall_get_hm_event(&hm_event);


    /* call partition HM callback */
    if (${partition.hm_callback} != NULL) {
        ${partition.hm_callback}(hm_event.state_id, hm_event.error_id);
    }
}
%endif

/**
 * @brief Program entry point
 */
int main(void) {

    %if partition.hm_callback:
    /* register HM ISR handler */
    void *ignored;
    isr_install_handler(XKY_IRQ_HM_EVENT, hm_isr_handler, &ignored);
    %endif
    %if 'imaspex' in partition.libraries:
    /* initialize IMASPEX */
    imaspex_init();

    %endif
    /* call entry point */
    if (${partition.entry_point} != NULL) {
        ${partition.entry_point}();
    }

    return 0;
}
