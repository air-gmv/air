/**
 * @file asm_offsets.h
 * @author lumm
 * @brief Offset determination for assembly code
 */

#include <air_arch.h>
#include <armv7.h>
#include <workspace.h>

#define DEFINE(s, v)            asm volatile("\n-> " #s " %0 " #v :: "i" (v))
#define OFFSETOF(s, v)          DEFINE(offsetof_##s##_##v, air_offsetof(s, v))
#define SIZEOF(s)               DEFINE(sizeof_##s, sizeof(s))
#define LOG2(s, v)              DEFINE(log2_##s, v)

int main(void) {

    OFFSETOF(pmk_sharedarea_t, core);

    OFFSETOF(pmk_core_ctrl_t, context);
    OFFSETOF(pmk_core_ctrl_t, partition);
    SIZEOF(pmk_core_ctrl_t);

    OFFSETOF(arm_core_context_t, trash);
    OFFSETOF(arm_core_context_t, isf_pointer);
    OFFSETOF(arm_core_context_t, idle_isf_pointer);
    OFFSETOF(arm_core_context_t, isr_nesting_level);

    OFFSETOF(arm_interrupt_stack_frame_t, orig_sp);
    OFFSETOF(arm_interrupt_stack_frame_t, vfp_context);
    SIZEOF(arm_interrupt_stack_frame_t);

    OFFSETOF(pmk_partition_t, context);

    LOG2(pmk_core_ctrl_t_size, 5);

}
