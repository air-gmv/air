/**
 * @file
 * @author pfnf
 * @brief Offset determination for assembly code
 * @note This file isn't meant to be compiled into executable code
 */

#include <air.h>
#include <pmk.h>
#include <cpu.h>
#include <sparc.h>
#include <schedule.h>
#include <partition.h>
#include <multicore.h>
#include <configurations.h>


#define DEFINE(s, v)     asm volatile("\n->" #s " %0 " #v : : "i" (v))
#define COMMENT(c)       asm volatile("\n->#" c)

int main(void) {

    COMMENT("CPU Stack Frame Offsets and Size");
    DEFINE(CPU_STACK_FRAME_L0_OFFSET, air_offsetof(sparc_stack_frame_t, l0));
    DEFINE(CPU_STACK_FRAME_L1_OFFSET, air_offsetof(sparc_stack_frame_t, l1));
    DEFINE(CPU_STACK_FRAME_L2_OFFSET, air_offsetof(sparc_stack_frame_t, l2));
    DEFINE(CPU_STACK_FRAME_L3_OFFSET, air_offsetof(sparc_stack_frame_t, l3));
    DEFINE(CPU_STACK_FRAME_L4_OFFSET, air_offsetof(sparc_stack_frame_t, l4));
    DEFINE(CPU_STACK_FRAME_L5_OFFSET, air_offsetof(sparc_stack_frame_t, l5));
    DEFINE(CPU_STACK_FRAME_L6_OFFSET, air_offsetof(sparc_stack_frame_t, l6));
    DEFINE(CPU_STACK_FRAME_L7_OFFSET, air_offsetof(sparc_stack_frame_t, l7));
    DEFINE(CPU_STACK_FRAME_I0_OFFSET, air_offsetof(sparc_stack_frame_t, i0));
    DEFINE(CPU_STACK_FRAME_I1_OFFSET, air_offsetof(sparc_stack_frame_t, i1));
    DEFINE(CPU_STACK_FRAME_I2_OFFSET, air_offsetof(sparc_stack_frame_t, i2));
    DEFINE(CPU_STACK_FRAME_I3_OFFSET, air_offsetof(sparc_stack_frame_t, i3));
    DEFINE(CPU_STACK_FRAME_I4_OFFSET, air_offsetof(sparc_stack_frame_t, i4));
    DEFINE(CPU_STACK_FRAME_I5_OFFSET, air_offsetof(sparc_stack_frame_t, i5));
    DEFINE(CPU_STACK_FRAME_I6_FP_OFFSET, air_offsetof(sparc_stack_frame_t, i6_fp));
    DEFINE(CPU_STACK_FRAME_I7_OFFSET, air_offsetof(sparc_stack_frame_t, i7));
    DEFINE(CPU_MINIMUM_STACK_FRAME_SIZE, sizeof(sparc_stack_frame_t));

    COMMENT("FPU Offsets and Size");
    DEFINE(FO_F1_OFFSET, air_offsetof(sparc_fpu_context_t, f0_f1));
    DEFINE(F2_F3_OFFSET, air_offsetof(sparc_fpu_context_t, f2_f3));
    DEFINE(F4_F5_OFFSET, air_offsetof(sparc_fpu_context_t, f4_f5));
    DEFINE(F6_F7_OFFSET, air_offsetof(sparc_fpu_context_t, f6_f7));
    DEFINE(F8_F9_OFFSET, air_offsetof(sparc_fpu_context_t, f8_f9));
    DEFINE(F1O_F11_OFFSET, air_offsetof(sparc_fpu_context_t, f10_f11));
    DEFINE(F12_F13_OFFSET, air_offsetof(sparc_fpu_context_t, f12_f13));
    DEFINE(F14_F15_OFFSET, air_offsetof(sparc_fpu_context_t, f14_f15));
    DEFINE(F16_F17_OFFSET, air_offsetof(sparc_fpu_context_t, f16_f17));
    DEFINE(F18_F19_OFFSET, air_offsetof(sparc_fpu_context_t, f18_f19));
    DEFINE(F2O_F21_OFFSET, air_offsetof(sparc_fpu_context_t, f20_f21));
    DEFINE(F22_F23_OFFSET, air_offsetof(sparc_fpu_context_t, f22_f23));
    DEFINE(F24_F25_OFFSET, air_offsetof(sparc_fpu_context_t, f24_f25));
    DEFINE(F26_F27_OFFSET, air_offsetof(sparc_fpu_context_t, f26_f27));
    DEFINE(F28_F29_OFFSET, air_offsetof(sparc_fpu_context_t, f28_f29));
    DEFINE(F3O_F31_OFFSET, air_offsetof(sparc_fpu_context_t, f30_f31));
    DEFINE(FSR_OFFSET, air_offsetof(sparc_fpu_context_t, fsr));

    COMMENT("CPU Interrupt Stack Frame");
    DEFINE(ISF_STACK_FRAME_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, stack_frame));
    DEFINE(ISF_PSR_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, psr));
    DEFINE(ISF_PC_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, pc));
    DEFINE(ISF_NPC_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, nkpc));
    DEFINE(ISF_TPC_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, tpc));
    DEFINE(ISF_PSP_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, psp));
    DEFINE(ISF_G1_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, g1));
    DEFINE(ISF_G2_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, g2));
    DEFINE(ISF_G3_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, g3));
    DEFINE(ISF_G4_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, g4));
    DEFINE(ISF_G5_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, g5));
    DEFINE(ISF_G6_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, g6));
    DEFINE(ISF_G7_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, g7));
    DEFINE(ISF_I0_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, i0));
    DEFINE(ISF_I1_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, i1));
    DEFINE(ISF_I2_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, i2));
    DEFINE(ISF_I3_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, i3));
    DEFINE(ISF_I4_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, i4));
    DEFINE(ISF_I5_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, i5));
    DEFINE(ISF_I6_FP_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, i6_fp));
    DEFINE(ISF_I7_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, i7));
    DEFINE(ISF_Y_OFFSET, air_offsetof(sparc_interrupt_stack_frame_t, y));
    DEFINE(CONTEXT_CONTROL_INTERRUPT_FRAME_SIZE, sizeof(sparc_interrupt_stack_frame_t));

    COMMENT("Core Context Offsets");
    DEFINE(CONTEXT_VCPU, air_offsetof(sparc_core_context_t, vcpu));
    DEFINE(CONTEXT_TRASHED, air_offsetof(sparc_core_context_t, trash));
    DEFINE(PMK_CORE_CTX_ENTRY_POINT, air_offsetof(sparc_core_context_t, entry_point));
    DEFINE(CONTEXT_STACK_POINTER, air_offsetof(sparc_core_context_t, stack_pointer));
    DEFINE(CONTEXT_INIT_STACK_POINTER, air_offsetof(sparc_core_context_t, isf_stack_pointer));
    DEFINE(CONTEXT_NESTING_LEVEL, air_offsetof(sparc_core_context_t, isr_nesting_level));
    DEFINE(PMK_CORE_CTX_IPC_EVENT, air_offsetof(sparc_core_context_t, ipc_event));
    DEFINE(CONTEXT_SYSTEM_STATE, air_offsetof(sparc_core_context_t, state));
    DEFINE(PMK_CORE_CTX_HM_EVENT, air_offsetof(sparc_core_context_t, hm_event));
    DEFINE(PMK_CORE_CTX_FPU, air_offsetof(sparc_core_context_t, fpu_context));
    DEFINE(PMK_CORE_CTX_SIZE, sizeof(sparc_core_context_t));

    COMMENT("Virtual CPU Offsets");
    DEFINE(SPARC_VCPU_ID, air_offsetof(sparc_virtual_cpu_t, id));
    DEFINE(SPARC_VCPU_TBR, air_offsetof(sparc_virtual_cpu_t, tbr));
    DEFINE(SPARC_VCPU_PSR, air_offsetof(sparc_virtual_cpu_t, psr));
    DEFINE(SPARC_VCPU_IMASK, air_offsetof(sparc_virtual_cpu_t, imask));
    DEFINE(SPARC_VCPU_IPEND, air_offsetof(sparc_virtual_cpu_t, ipend));
    DEFINE(SPARC_VCPU_CCTRL, air_offsetof(sparc_virtual_cpu_t, cctrl));
    DEFINE(SPARC_VCPU_MMU_CTRL, air_offsetof(sparc_virtual_cpu_t, mmu_ctrl));
    DEFINE(SPARC_VCPU_MMU_FSR, air_offsetof(sparc_virtual_cpu_t, mmu_fsr));
    DEFINE(SPARC_VCPU_SIZE, sizeof(sparc_virtual_cpu_t));

    COMMENT("Sharedarea Offsets");
    DEFINE(SHAREDAREA_CORE_CNT, air_offsetof(pmk_sharedarea_t, configured_cores));
    DEFINE(SHAREDAREA_CORE_CTRL, air_offsetof(pmk_sharedarea_t, core));
    DEFINE(SHAREDAREA_SCHEDULE_CTRL, air_offsetof(pmk_sharedarea_t, schedule_ctrl));
    DEFINE(SHAREDAREA_SIZE, sizeof(pmk_sharedarea_t));

    COMMENT("Core Control Offsets");
    DEFINE(CORE_CTRL_IDX, air_offsetof(pmk_core_ctrl_t, idx));
    DEFINE(CORE_CTRL_CONTEXT, air_offsetof(pmk_core_ctrl_t, context));
    DEFINE(CORE_CTRL_PARTITION, air_offsetof(pmk_core_ctrl_t, partition));
    DEFINE(CORE_CTRL_PARTITION_SWITCH, air_offsetof(pmk_core_ctrl_t, partition_switch));
    DEFINE(CORE_CTRL_SCHEDULE, air_offsetof(pmk_core_ctrl_t, schedule));
    DEFINE(CORE_CTRL_PREEMPT_POINT, air_offsetof(pmk_core_ctrl_t, preempt_point));
    DEFINE(CORE_CTRL_SIZE, sizeof(pmk_core_ctrl_t));

    COMMENT("Schedule Control Offsets");
    DEFINE(SCHEDULE_CTRL_CURRENT_SCHEDULE, air_offsetof(pmk_schedule_ctrl_t, curr_schedule));
    DEFINE(SCHEDULE_CTRL_NEXT_SCHEDULE, air_offsetof(pmk_schedule_ctrl_t, next_schedule));
    DEFINE(SCHEDULE_CTRL_MTF_TICKS, air_offsetof(pmk_schedule_ctrl_t, mtf_ticks));
    DEFINE(SCHEDULE_CTRL_TOTAL_TICKS, air_offsetof(pmk_schedule_ctrl_t, total_ticks));
    DEFINE(SCHEDULE_CTRL_LAST_SWITCH_TICKS, air_offsetof(pmk_schedule_ctrl_t, last_switch_ticks));
    DEFINE(SCHEDULE_CTRL_MTF_BARRIER, air_offsetof(pmk_schedule_ctrl_t, mtf_barrier));
    DEFINE(SCHEDULE_CTRL_SIZE, sizeof(pmk_schedule_ctrl_t));

    COMMENT("Partition Control Offsets");
    DEFINE(PARTITION_IDX, air_offsetof(pmk_partition_t, idx));
    DEFINE(PARTITION_CORE_CNT, air_offsetof(pmk_partition_t, cores));
    DEFINE(PARTITION_ELAPSED_TICKS, air_offsetof(pmk_partition_t, elapsed_ticks));
    DEFINE(PARTITION_PERMISSIONS, air_offsetof(pmk_partition_t, permissions));
    DEFINE(PARTITION_CORE_MAPPING, air_offsetof(pmk_partition_t, core_mapping));
    DEFINE(PARTITION_EVENTS, air_offsetof(pmk_partition_t, events));
    DEFINE(PARTITION_MMAP, air_offsetof(pmk_partition_t, mmap));
    DEFINE(PARTITION_SIZE, sizeof(pmk_partition_t));

    COMMENT("Partition Memory Map Offsets");
    DEFINE(MMAP_P_ADDR, air_offsetof(pmk_mmap_t, p_addr));
    DEFINE(MMAP_V_ADDR, air_offsetof(pmk_mmap_t, v_addr));
    DEFINE(MMAP_UNIT, air_offsetof(pmk_mmap_t, p_unit));
    DEFINE(MMAP_SIZE, air_offsetof(pmk_mmap_t, size));

    COMMENT("User Configuration Offsets");
    DEFINE(USR_CONFIG_MAGIC, air_offsetof(pmk_configuration_t, magic));
    DEFINE(USR_CONFIG_CORES, air_offsetof(pmk_configuration_t, cores));
    DEFINE(USR_CONFIG_US_PER_TICK, air_offsetof(pmk_configuration_t, us_per_tick));
    DEFINE(USR_CONFIG_PARTITIONS, air_offsetof(pmk_configuration_t, partitions));
    DEFINE(USR_CONFIG_SCHEDULES, air_offsetof(pmk_configuration_t, schedules));
    DEFINE(USR_CONFIG_SHARED_AREA, air_offsetof(pmk_configuration_t, sharedareas));
    DEFINE(USR_CONFIG_CHANNELS, air_offsetof(pmk_configuration_t, channels));
    DEFINE(USR_CONFIG_SIZE, sizeof(pmk_configuration_t));

    COMMENT("CPU MMU control Offsets");
    DEFINE(CPU_MMU_CONTROL_CTX, air_offsetof(sparc_mmu_context_t, context));
    DEFINE(CPU_MMU_CONTROL_L1TABLES, air_offsetof(sparc_mmu_context_t, l1_tables));
    DEFINE(CPU_MMU_CONTROL_SIZE, sizeof(sparc_mmu_context_t));

    return 0;
}
