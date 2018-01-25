/**
 * @file
 * @author pfnf
 * @brief Memory offsets for the PMK structures
 */

#ifndef __ASM_OFFSETS_H__
#define __ASM_OFFSETS_H__

/**
 * @defgroup asm_gp0 CPU Stack Frame Offsets and Size
 * @{
 */
/** @brief air_offsetof(sparc_stack_frame_t, l0)                            */
#define CPU_STACK_FRAME_L0_OFFSET                                            0
/** @brief air_offsetof(sparc_stack_frame_t, l1)                            */
#define CPU_STACK_FRAME_L1_OFFSET                                            4
/** @brief air_offsetof(sparc_stack_frame_t, l2)                            */
#define CPU_STACK_FRAME_L2_OFFSET                                            8
/** @brief air_offsetof(sparc_stack_frame_t, l3)                            */
#define CPU_STACK_FRAME_L3_OFFSET                                           12
/** @brief air_offsetof(sparc_stack_frame_t, l4)                            */
#define CPU_STACK_FRAME_L4_OFFSET                                           16
/** @brief air_offsetof(sparc_stack_frame_t, l5)                            */
#define CPU_STACK_FRAME_L5_OFFSET                                           20
/** @brief air_offsetof(sparc_stack_frame_t, l6)                            */
#define CPU_STACK_FRAME_L6_OFFSET                                           24
/** @brief air_offsetof(sparc_stack_frame_t, l7)                            */
#define CPU_STACK_FRAME_L7_OFFSET                                           28
/** @brief air_offsetof(sparc_stack_frame_t, i0)                            */
#define CPU_STACK_FRAME_I0_OFFSET                                           32
/** @brief air_offsetof(sparc_stack_frame_t, i1)                            */
#define CPU_STACK_FRAME_I1_OFFSET                                           36
/** @brief air_offsetof(sparc_stack_frame_t, i2)                            */
#define CPU_STACK_FRAME_I2_OFFSET                                           40
/** @brief air_offsetof(sparc_stack_frame_t, i3)                            */
#define CPU_STACK_FRAME_I3_OFFSET                                           44
/** @brief air_offsetof(sparc_stack_frame_t, i4)                            */
#define CPU_STACK_FRAME_I4_OFFSET                                           48
/** @brief air_offsetof(sparc_stack_frame_t, i5)                            */
#define CPU_STACK_FRAME_I5_OFFSET                                           52
/** @brief air_offsetof(sparc_stack_frame_t, i6_fp)                         */
#define CPU_STACK_FRAME_I6_FP_OFFSET                                        56
/** @brief air_offsetof(sparc_stack_frame_t, i7)                            */
#define CPU_STACK_FRAME_I7_OFFSET                                           60
/** @brief sizeof(sparc_stack_frame_t)                                      */
#define CPU_MINIMUM_STACK_FRAME_SIZE                                        96
/** @} */

/**
 * @defgroup asm_gp1 FPU Offsets and Size
 * @{
 */
/** @brief air_offsetof(sparc_fpu_context_t, f0_f1)                         */
#define FO_F1_OFFSET                                                         0
/** @brief air_offsetof(sparc_fpu_context_t, f2_f3)                         */
#define F2_F3_OFFSET                                                         8
/** @brief air_offsetof(sparc_fpu_context_t, f4_f5)                         */
#define F4_F5_OFFSET                                                        16
/** @brief air_offsetof(sparc_fpu_context_t, f6_f7)                         */
#define F6_F7_OFFSET                                                        24
/** @brief air_offsetof(sparc_fpu_context_t, f8_f9)                         */
#define F8_F9_OFFSET                                                        32
/** @brief air_offsetof(sparc_fpu_context_t, f10_f11)                       */
#define F1O_F11_OFFSET                                                      40
/** @brief air_offsetof(sparc_fpu_context_t, f12_f13)                       */
#define F12_F13_OFFSET                                                      48
/** @brief air_offsetof(sparc_fpu_context_t, f14_f15)                       */
#define F14_F15_OFFSET                                                      56
/** @brief air_offsetof(sparc_fpu_context_t, f16_f17)                       */
#define F16_F17_OFFSET                                                      64
/** @brief air_offsetof(sparc_fpu_context_t, f18_f19)                       */
#define F18_F19_OFFSET                                                      72
/** @brief air_offsetof(sparc_fpu_context_t, f20_f21)                       */
#define F2O_F21_OFFSET                                                      80
/** @brief air_offsetof(sparc_fpu_context_t, f22_f23)                       */
#define F22_F23_OFFSET                                                      88
/** @brief air_offsetof(sparc_fpu_context_t, f24_f25)                       */
#define F24_F25_OFFSET                                                      96
/** @brief air_offsetof(sparc_fpu_context_t, f26_f27)                       */
#define F26_F27_OFFSET                                                     104
/** @brief air_offsetof(sparc_fpu_context_t, f28_f29)                       */
#define F28_F29_OFFSET                                                     112
/** @brief air_offsetof(sparc_fpu_context_t, f30_f31)                       */
#define F3O_F31_OFFSET                                                     120
/** @brief air_offsetof(sparc_fpu_context_t, fsr)                           */
#define FSR_OFFSET                                                         128
/** @} */

/**
 * @defgroup asm_gp2 CPU Interrupt Stack Frame
 * @{
 */
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, stack_frame)         */
#define ISF_STACK_FRAME_OFFSET                                               0
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, psr)                 */
#define ISF_PSR_OFFSET                                                      96
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, pc)                  */
#define ISF_PC_OFFSET                                                      100
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, nkpc)                */
#define ISF_NPC_OFFSET                                                     104
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, tpc)                 */
#define ISF_TPC_OFFSET                                                     108
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, psp)                 */
#define ISF_PSP_OFFSET                                                     112
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, g1)                  */
#define ISF_G1_OFFSET                                                      116
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, g2)                  */
#define ISF_G2_OFFSET                                                      120
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, g3)                  */
#define ISF_G3_OFFSET                                                      124
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, g4)                  */
#define ISF_G4_OFFSET                                                      128
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, g5)                  */
#define ISF_G5_OFFSET                                                      132
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, g6)                  */
#define ISF_G6_OFFSET                                                      136
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, g7)                  */
#define ISF_G7_OFFSET                                                      140
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, i0)                  */
#define ISF_I0_OFFSET                                                      144
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, i1)                  */
#define ISF_I1_OFFSET                                                      148
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, i2)                  */
#define ISF_I2_OFFSET                                                      152
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, i3)                  */
#define ISF_I3_OFFSET                                                      156
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, i4)                  */
#define ISF_I4_OFFSET                                                      160
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, i5)                  */
#define ISF_I5_OFFSET                                                      164
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, i6_fp)               */
#define ISF_I6_FP_OFFSET                                                   168
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, i7)                  */
#define ISF_I7_OFFSET                                                      172
/** @brief air_offsetof(sparc_interrupt_stack_frame_t, y)                   */
#define ISF_Y_OFFSET                                                       176
/** @brief sizeof(sparc_interrupt_stack_frame_t)                            */
#define CONTEXT_CONTROL_INTERRUPT_FRAME_SIZE                               184
/** @} */

/**
 * @defgroup asm_gp3 Core Context Offsets
 * @{
 */
/** @brief air_offsetof(sparc_core_context_t, vcpu)                         */
#define CONTEXT_VCPU                                                         0
/** @brief air_offsetof(sparc_core_context_t, trash)                        */
#define CONTEXT_TRASHED                                                     32
/** @brief air_offsetof(sparc_core_context_t, entry_point)                  */
#define PMK_CORE_CTX_ENTRY_POINT                                            36
/** @brief air_offsetof(sparc_core_context_t, stack_pointer)                */
#define CONTEXT_STACK_POINTER                                               40
/** @brief air_offsetof(sparc_core_context_t, isf_stack_pointer)            */
#define CONTEXT_INIT_STACK_POINTER                                          44
/** @brief air_offsetof(sparc_core_context_t, isr_nesting_level)            */
#define CONTEXT_NESTING_LEVEL                                               48
/** @brief air_offsetof(sparc_core_context_t, ipc_event)                    */
#define PMK_CORE_CTX_IPC_EVENT                                              56
/** @brief air_offsetof(sparc_core_context_t, state)                        */
#define CONTEXT_SYSTEM_STATE                                                60
/** @brief air_offsetof(sparc_core_context_t, hm_event)                     */
#define PMK_CORE_CTX_HM_EVENT                                               64
/** @brief air_offsetof(sparc_core_context_t, fpu_context)                  */
#define PMK_CORE_CTX_FPU                                                    52
/** @brief sizeof(sparc_core_context_t)                                     */
#define PMK_CORE_CTX_SIZE                                                   68
/** @} */

/**
 * @defgroup asm_gp4 Virtual CPU Offsets
 * @{
 */
/** @brief air_offsetof(sparc_virtual_cpu_t, id)                            */
#define SPARC_VCPU_ID                                                        0
/** @brief air_offsetof(sparc_virtual_cpu_t, tbr)                           */
#define SPARC_VCPU_TBR                                                       4
/** @brief air_offsetof(sparc_virtual_cpu_t, psr)                           */
#define SPARC_VCPU_PSR                                                       8
/** @brief air_offsetof(sparc_virtual_cpu_t, imask)                         */
#define SPARC_VCPU_IMASK                                                    12
/** @brief air_offsetof(sparc_virtual_cpu_t, ipend)                         */
#define SPARC_VCPU_IPEND                                                    16
/** @brief air_offsetof(sparc_virtual_cpu_t, cctrl)                         */
#define SPARC_VCPU_CCTRL                                                    20
/** @brief air_offsetof(sparc_virtual_cpu_t, mmu_ctrl)                      */
#define SPARC_VCPU_MMU_CTRL                                                 24
/** @brief air_offsetof(sparc_virtual_cpu_t, mmu_fsr)                       */
#define SPARC_VCPU_MMU_FSR                                                  28
/** @brief sizeof(sparc_virtual_cpu_t)                                      */
#define SPARC_VCPU_SIZE                                                     32
/** @} */

/**
 * @defgroup asm_gp5 Sharedarea Offsets
 * @{
 */
/** @brief air_offsetof(pmk_sharedarea_t, configured_cores)                 */
#define SHAREDAREA_CORE_CNT                                                  0
/** @brief air_offsetof(pmk_sharedarea_t, core)                             */
#define SHAREDAREA_CORE_CTRL                                                 4
/** @brief air_offsetof(pmk_sharedarea_t, schedule_ctrl)                    */
#define SHAREDAREA_SCHEDULE_CTRL                                             8
/** @brief sizeof(pmk_sharedarea_t)                                         */
#define SHAREDAREA_SIZE                                                     20
/** @} */

/**
 * @defgroup asm_gp6 Core Control Offsets
 * @{
 */
/** @brief air_offsetof(pmk_core_ctrl_t, idx)                               */
#define CORE_CTRL_IDX                                                        0
/** @brief air_offsetof(pmk_core_ctrl_t, context)                           */
#define CORE_CTRL_CONTEXT                                                    4
/** @brief air_offsetof(pmk_core_ctrl_t, partition)                         */
#define CORE_CTRL_PARTITION                                                  8
/** @brief air_offsetof(pmk_core_ctrl_t, partition_switch)                  */
#define CORE_CTRL_PARTITION_SWITCH                                          24
/** @brief air_offsetof(pmk_core_ctrl_t, schedule)                          */
#define CORE_CTRL_SCHEDULE                                                  16
/** @brief air_offsetof(pmk_core_ctrl_t, preempt_point)                     */
#define CORE_CTRL_PREEMPT_POINT                                             20
/** @brief sizeof(pmk_core_ctrl_t)                                          */
#define CORE_CTRL_SIZE                                                      40
/** @} */

/**
 * @defgroup asm_gp7 Schedule Control Offsets
 * @{
 */
/** @brief air_offsetof(pmk_schedule_ctrl_t, curr_schedule)                 */
#define SCHEDULE_CTRL_CURRENT_SCHEDULE                                       0
/** @brief air_offsetof(pmk_schedule_ctrl_t, next_schedule)                 */
#define SCHEDULE_CTRL_NEXT_SCHEDULE                                          4
/** @brief air_offsetof(pmk_schedule_ctrl_t, mtf_ticks)                     */
#define SCHEDULE_CTRL_MTF_TICKS                                              8
/** @brief air_offsetof(pmk_schedule_ctrl_t, total_ticks)                   */
#define SCHEDULE_CTRL_TOTAL_TICKS                                           16
/** @brief air_offsetof(pmk_schedule_ctrl_t, last_switch_ticks)             */
#define SCHEDULE_CTRL_LAST_SWITCH_TICKS                                     24
/** @brief air_offsetof(pmk_schedule_ctrl_t, mtf_barrier)                   */
#define SCHEDULE_CTRL_MTF_BARRIER                                           32
/** @brief sizeof(pmk_schedule_ctrl_t)                                      */
#define SCHEDULE_CTRL_SIZE                                                  64
/** @} */

/**
 * @defgroup asm_gp8 Partition Control Offsets
 * @{
 */
/** @brief air_offsetof(pmk_partition_t, idx)                               */
#define PARTITION_IDX                                                        0
/** @brief air_offsetof(pmk_partition_t, cores)                             */
#define PARTITION_CORE_CNT                                                   4
/** @brief air_offsetof(pmk_partition_t, elapsed_ticks)                     */
#define PARTITION_ELAPSED_TICKS                                             32
/** @brief air_offsetof(pmk_partition_t, permissions)                       */
#define PARTITION_PERMISSIONS                                               48
/** @brief air_offsetof(pmk_partition_t, core_mapping)                      */
#define PARTITION_CORE_MAPPING                                              44
/** @brief air_offsetof(pmk_partition_t, events)                            */
#define PARTITION_EVENTS                                                    52
/** @brief air_offsetof(pmk_partition_t, mmap)                              */
#define PARTITION_MMAP                                                     104
/** @brief sizeof(pmk_partition_t)                                          */
#define PARTITION_SIZE                                                     168
/** @} */

/**
 * @defgroup asm_gp9 Partition Memory Map Offsets
 * @{
 */
/** @brief air_offsetof(pmk_mmap_t, p_addr)                                 */
#define MMAP_P_ADDR                                                          0
/** @brief air_offsetof(pmk_mmap_t, v_addr)                                 */
#define MMAP_V_ADDR                                                          4
/** @brief air_offsetof(pmk_mmap_t, p_unit)                                 */
#define MMAP_UNIT                                                            8
/** @brief air_offsetof(pmk_mmap_t, size)                                   */
#define MMAP_SIZE                                                           12
/** @} */

/**
 * @defgroup asm_gp10 User Configuration Offsets
 * @{
 */
/** @brief air_offsetof(pmk_configuration_t, magic)                         */
#define USR_CONFIG_MAGIC                                                     0
/** @brief air_offsetof(pmk_configuration_t, cores)                         */
#define USR_CONFIG_CORES                                                     4
/** @brief air_offsetof(pmk_configuration_t, us_per_tick)                   */
#define USR_CONFIG_US_PER_TICK                                               8
/** @brief air_offsetof(pmk_configuration_t, partitions)                    */
#define USR_CONFIG_PARTITIONS                                               16
/** @brief air_offsetof(pmk_configuration_t, schedules)                     */
#define USR_CONFIG_SCHEDULES                                                24
/** @brief air_offsetof(pmk_configuration_t, sharedareas)                   */
#define USR_CONFIG_SHARED_AREA                                              32
/** @brief air_offsetof(pmk_configuration_t, channels)                      */
#define USR_CONFIG_CHANNELS                                                 40
/** @brief sizeof(pmk_configuration_t)                                      */
#define USR_CONFIG_SIZE                                                     64
/** @} */

/**
 * @defgroup asm_gp11 CPU MMU control Offsets
 * @{
 */
/** @brief air_offsetof(sparc_mmu_context_t, context)                       */
#define CPU_MMU_CONTROL_CTX                                                  0
/** @brief air_offsetof(sparc_mmu_context_t, l1_tables)                     */
#define CPU_MMU_CONTROL_L1TABLES                                             4
/** @brief sizeof(sparc_mmu_context_t)                                      */
#define CPU_MMU_CONTROL_SIZE                                                 8
/** @} */


#endif /* __ASM_OFFSETS_H__ */

