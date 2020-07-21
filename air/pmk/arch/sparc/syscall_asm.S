/*
 * Copyright (C) 2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file syscall.S
 * @author pfnf
 * @brief Contais the PMK generic system calls (PMK side)
 */

/* enforce ASM definition */
#ifndef ASM
#define ASM
#endif

#include <air.h>
#include <pmk.h>
#include <asm.h>
#include <bsp.h>
#include <ipc.h>
#include <sparc.h>
#include <partition.h>
#include <health_monitor.h>

/**
 * @brief PMK OS System Calls
 */
    GLOBAL(sparc_os_syscall)
SYM(sparc_os_syscall):

    /* the system call is synchronous */
    mov     %l2, %l1
    add     %l2, 0x04, %l2

    /* check if the system call is valid */
    cmp     %i5, AIR_SYSCALL_COUNT
    bgu     syscall_simple_return
    nop

    /* offset within the system call branch table */
    set     syscall_branch_table, %l4
    umul    %i5, 0x04, %l5
    jmp     %l4 + %l5
    nop

/**
 * @brief System call branch table
 */
syscall_branch_table:

    ba,a    syscall_simple_return            ! get virtual IRQ (NYI)
    ba,a    syscall_simple_return            ! mask virtual IRQ (NYI)
    ba,a    syscall_simple_return            ! unmask virtual IRQ (NYI)
    ba,a    sparc_isr_handler_entry          ! get physical address     (in C)
    ba,a    syscall_get_nb_cores             ! get partition core number
    ba,a    syscall_get_core_id              ! get virtual core id
    ba,a    syscall_copy_tbr                 ! copy virtual core tbr
    ba,a    syscall_boot_core                ! boot virtual core
    ba,a    syscall_get_us_per_tick          ! get us per tick
    ba,a    syscall_get_elapsed_ticks        ! get elapsed ticks
    ba,a    sparc_isr_handler_entry          ! get partition id         (in C)
    ba,a    sparc_isr_handler_entry          ! get partition status     (in C)
    ba,a    sparc_isr_handler_entry          ! set partition mode       (in C)
    ba,a    sparc_isr_handler_entry          ! get schedule id          (in C)
    ba,a    sparc_isr_handler_entry          ! get schedule status      (in C)
    ba,a    sparc_isr_handler_entry          ! set schedule             (in C)
    ba,a    sparc_isr_handler_entry          ! get time of day          (in C)
    ba,a    sparc_isr_handler_entry          ! set time of day          (in C)
    ba,a    sparc_isr_handler_entry          ! get port id              (in C)
    ba,a    sparc_isr_handler_entry          ! get port status          (in C)
    ba,a    sparc_isr_handler_entry          ! create port              (in C)
    ba,a    sparc_isr_handler_entry          ! read port                (in C)
    ba,a    sparc_isr_handler_entry          ! write port               (in C)
    ba,a    sparc_isr_handler_entry          ! get sharedarea           (in C)
    ba,a    sparc_isr_handler_entry          ! restart module           (in C)
    ba,a    syscall_shutdown_module          ! shutdown module          (in C)
    ba,a    syscall_get_event                ! get event
    ba,a    syscall_get_system_state         ! get system state
    ba,a    sparc_isr_handler_entry          ! set system state         (in C)
    ba,a    sparc_isr_handler_entry          ! get HM event             (in C)
    ba,a    sparc_isr_handler_entry          ! raise HM event           (in C)
    ba,a    sparc_isr_handler_entry          ! partition putchar        (in C)
    ba,a    sparc_isr_handler_entry          ! partition print          (in C)
    ba,a    syscall_simple_return            ! enable cache (in user land)
    ba,a    syscall_simple_return            ! disable cache (in user land)
    ba,a    syscall_simple_return            ! freeze cache (in user land)
    ba,a    syscall_simple_return            ! flush cache (in user land)
    ba,a    sparc_isr_handler_entry          ! get amba configuration   (in C)
    ba,a    sparc_isr_handler_entry          ! end partition on window  (in C)

/**
 * @brief Get current number of partition cores
 */
syscall_get_nb_cores:

    set     SYM(air_shared_area), %l3                ! shared area
    ld      [%l3 + SHAREDAREA_CORE_CTRL], %l3        ! core control area

    rd      %asr17, %l4
    srl     %l4, 28, %l4
    and     %l4, 0xff, %l4                           ! current physical core
    umul    %l4, CORE_CTRL_SIZE, %l4                 ! core control offset
    add     %l3, %l4, %l4                            ! current core control area

    ld      [%l4 + CORE_CTRL_PARTITION], %l4         ! current partition

    ba      syscall_simple_return
    ld      [%l4 + PARTITION_CORE_CNT], %i0          ! partition core count

/**
 * @brief Get current virtual core id
 */
syscall_get_core_id:

    set     SYM(air_shared_area), %l3                ! shared area
    ld      [%l3 + SHAREDAREA_CORE_CTRL], %l3        ! core control area

    rd      %asr17, %l4
    srl     %l4, 28, %l4
    and     %l4, 0xff, %l4                           ! current physical core
    umul    %l4, CORE_CTRL_SIZE, %l4                 ! core control offset
    add     %l3, %l4, %l3                            ! current core control area
    ld      [%l3 + CORE_CTRL_CONTEXT], %l3           ! current core context

    ba      syscall_simple_return
    ld      [%l3 + SPARC_VCPU_ID], %i0               ! virtual core id

/**
 * @brief Set a secondary core virtual cpu tbr
 */
syscall_copy_tbr:

    set     SYM(air_shared_area), %l3                ! shared area
    ld      [%l3 + SHAREDAREA_CORE_CTRL], %l3        ! core control area

    rd      %asr17, %l4
    srl     %l4, 28, %l4
    and     %l4, 0xff, %l4                           ! current physical core
    umul    %l4, CORE_CTRL_SIZE, %l4                 ! core control offset
    add     %l3, %l4, %l4                            ! current core control area

    ld      [%l4 + CORE_CTRL_PARTITION], %l4         ! current partition
    ld      [%l4 + PARTITION_CORE_CNT], %l5          ! partition core count

    cmp     %i0, %l5
    bge     syscall_invalid_config_return            ! no? then its an error
    nop

    ld      [%l4 + PARTITION_CORE_MAPPING], %l4      ! virtual to real core
    umul    %i0, 0x04, %i0                           ! dest core memory offset
    ld      [%l4 + %i0], %l4                         ! dest core id

    umul    %l4, CORE_CTRL_SIZE, %l5                 ! dest core control offset
    add     %l3, %l5, %l5                            ! dest core control area

    ld      [%l5 + CORE_CTRL_CONTEXT], %l5           ! dest core context
    set     PMK_IPC_SET_TBR, %l6                     ! IPC to set tbr

    st      %l6, [%l5 + PMK_CORE_CTX_IPC_EVENT]      ! set dest IPC event

    sethi   %hi(IRQ_force_register), %l5
    ld      [%l5 + %lo(IRQ_force_register)], %l5     ! get force IRQ register

    mov     1, %l6
    umul    %l4, 0x04, %l4                           ! force IRQ register offset
    sll     %l6, BSP_IPC_IRQ - 0x10, %l6             ! IRQ of IPC
    ba      syscall_no_error_return
    st      %l6, [%l5 + %l4]                         ! force IRQ on core

/**
 * @brief Boot secondary core
 */
syscall_boot_core:

    set     SYM(air_shared_area), %l3                ! shared area
    ld      [%l3 + SHAREDAREA_CORE_CTRL], %l3        ! core control area

    rd      %asr17, %l4
    srl     %l4, 28, %l4
    and     %l4, 0xff, %l4                           ! current physical core
    umul    %l4, CORE_CTRL_SIZE, %l4                 ! core control offset
    add     %l3, %l4, %l4                            ! current core control area

    ld      [%l4 + CORE_CTRL_PARTITION], %l4         ! current partition
    ld      [%l4 + PARTITION_CORE_CNT], %l5          ! partition core count

    cmp     %i0, %l5
    bge     syscall_invalid_config_return            ! no? then its an error
    nop

    ld      [%l4 + PARTITION_CORE_MAPPING], %l4      ! virtual to real core
    umul    %i0, 0x04, %i0                           ! boot core memory offset
    ld      [%l4 + %i0], %l4                         ! boot core id

    umul    %l4, CORE_CTRL_SIZE, %l5                 ! boot core control offset
    add     %l3, %l5, %l5                            ! boot core control area

    ld      [%l5 + CORE_CTRL_CONTEXT], %l5           ! boot core context
    set     PMK_IPC_BOOT_PARTITION_CORE, %l6         ! IPC to boot core

    st      %i1, [%l5 + PMK_CORE_CTX_ENTRY_POINT]    ! set boot core entry
    st      %l6, [%l5 + PMK_CORE_CTX_IPC_EVENT]      ! set boot IPC event

    sethi   %hi(IRQ_force_register), %l5
    ld      [%l5 + %lo(IRQ_force_register)], %l5     ! get force IRQ register

    mov     1, %l6
    umul    %l4, 0x04, %l4                           ! force IRQ register offset
    sll     %l6, BSP_IPC_IRQ - 0x10, %l6                    ! IRQ of IPC
    ba      syscall_no_error_return
    st      %l6, [%l5 + %l4]                         ! force IRQ on boot core

/**
 * @brief Get microseconds per tick
 */
syscall_get_us_per_tick:

    set     air_configuration, %l3                   ! air configuration area
    ba      syscall_simple_return
    ldd     [%l3 + USR_CONFIG_US_PER_TICK], %i0      ! micro seconds per tick

/**
 * @brief Get partition elapsed ticks
 */
syscall_get_elapsed_ticks:

    set     SYM(air_shared_area), %l3                ! shared area
    ld      [%l3 + SHAREDAREA_CORE_CTRL], %l3        ! core control area

    rd      %asr17, %l4
    srl     %l4, 28, %l4
    and     %l4, 0xff, %l4                           ! current physical core
    umul    %l4, CORE_CTRL_SIZE, %l4                 ! core control offset
    add     %l3, %l4, %l4                            ! current core control area

    ld      [%l4 + CORE_CTRL_PARTITION], %l4         ! current partition
    ba      syscall_simple_return
    ldd     [%l4 + PARTITION_ELAPSED_TICKS], %i0     ! elapsed ticks

/**
 * @brief Get System State
 */
syscall_get_event:

    set     SYM(air_shared_area), %l3                ! shared area
    ld      [%l3 + SHAREDAREA_CORE_CTRL], %l3        ! core control area

    rd      %asr17, %l4
    srl     %l4, 28, %l4
    and     %l4, 0xff, %l4                           ! current physical core
    umul    %l4, CORE_CTRL_SIZE, %l4                 ! core control offset
    add     %l3, %l4, %l4                            ! current core control area

    ld      [%l4 + CORE_CTRL_PARTITION], %l4         ! current partition
    ld      [%l4 + PARTITION_EVENTS], %i0            ! load event
    ba      syscall_simple_return
    st      %g0, [%l4 + PARTITION_EVENTS]            ! clear event register


/**
 * @brief Get System state
 */
syscall_get_system_state:

    set     SYM(air_shared_area), %l3                ! shared area
    ld      [%l3 + SHAREDAREA_CORE_CTRL], %l3        ! core control area

    rd      %asr17, %l4
    srl     %l4, 28, %l4
    and     %l4, 0xff, %l4                           ! current physical core
    umul    %l4, CORE_CTRL_SIZE, %l4                 ! core control offset
    add     %l3, %l4, %l3                            ! current core control area
    ld      [%l3 + CORE_CTRL_CONTEXT], %l3           ! current core context

    ba      syscall_simple_return
    ld      [%l3 + CONTEXT_SYSTEM_STATE], %i0        ! current system state

/**
 * @brief Shutdown module
 */
syscall_shutdown_module:

    set     SYM(air_shared_area), %l3                ! shared area
    ld      [%l3 + SHAREDAREA_CORE_CTRL], %l3        ! core control area

    rd      %asr17, %l4
    srl     %l4, 28, %l4
    and     %l4, 0xff, %l4                           ! current physical core
    umul    %l4, CORE_CTRL_SIZE, %l4                 ! core control offset
    add     %l3, %l4, %l4                            ! current core control area

    ld      [%l4 + CORE_CTRL_PARTITION], %l4         ! current partition
    ld      [%l4 + PARTITION_PERMISSIONS], %l4   ! partition permissions

    btst    AIR_PERMISSION_MODULE_CONTROL, %l4
    bz      syscall_simple_return
    nop
    ta      0x0                                      ! "shutdown" module

/**
 * @brief Return without return value
 */
syscall_simple_return:

    mov     %l0, %psr                                ! restore partition PSR
    nop; nop; nop                                    ! WRPSR delay slots
    jmp     %l1                                      ! set PC
    rett    %l2                                      ! set nPC and exit trap

/**
 * @brief No error return
 */
syscall_no_error_return:

    set     AIR_NO_ERROR, %i0                        ! set no error code
    mov     %l0, %psr                                ! restore partition PSR
    nop; nop; nop                                    ! WRPSR delay slots
    jmp     %l1                                      ! set PC
    rett    %l2                                      ! set nPC and exit trap

/**
 * @brief Invalid configuration return
 */
syscall_invalid_config_return:

    set     AIR_INVALID_CONFIG, %i0                  ! set invalid config code
    mov     %l0, %psr                                ! restore partition PSR
    nop; nop; nop                                    ! WRPSR delay slots
    jmp     %l1                                      ! set PC
    rett    %l2                                      ! set nPC and exit trap


