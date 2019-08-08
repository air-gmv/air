/*
 * Copyright (C) 2013-2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * @file
 * @brief Common start code for SPARC.
 */

/* enforce ASM definition */
#ifndef ASM
#define ASM
#endif

#include <asm.h>

/**
 * @brief PMK Trap table
 */
    GLOBAL(pmk_trap_table)
SYM(pmk_trap_table):

    TRAP_RESET(0x00,pmk_hard_reset)            ! 0x00 - reset trap
    TRAP_IGNORE()                              ! 0x01 - instruction access exception
    TRAP_IGNORE()                              ! 0x02 - illegal instruction
    TRAP_IGNORE()                              ! 0x03 - privileged instruction
    TRAP_IGNORE()                              ! 0x04 - fp disabled
    TRAP_HANDLER(0x05, sparc_window_overflow_handler)   ! 0x05 - window overflow
    TRAP_HANDLER(0x06, sparc_window_underflow_handler)  ! 0x06 - window underflow
    TRAP_IGNORE()                              ! 0x07 - memory address not aligned
    TRAP_IGNORE()                              ! 0x08 - fp exception
    TRAP_IGNORE()                              ! 0x09 - data access exception
    TRAP_IGNORE()                              ! 0x0a - tag overflow
    TRAP_IGNORE()                              ! 0x0b - watchpoint_detected
    TRAP_IGNORE()                              ! 0x0c -
    TRAP_IGNORE()                              ! 0x0d -
    TRAP_IGNORE()                              ! 0x0e -
    TRAP_IGNORE()                              ! 0x0f -
    TRAP_IGNORE()                              ! 0x10 -

    ! Hardware Interrupts
    TRAP_IGNORE()                              ! 0x11 - interrupt_level_1 : AHBSTAT
    TRAP_IGNORE()                              ! 0x12 - interrupt_level_2 : APBUART1
    TRAP_IGNORE()                              ! 0x13 - interrupt_level_3 : APBUART2
    TRAP_IGNORE()                              ! 0x14 - interrupt_level_4 : CAN_OC0
    TRAP_IGNORE()                              ! 0x15 - interrupt_level_5 : CAN_OC1/PCIF
    TRAP_IGNORE()                              ! 0x16 - interrupt_level_6 : GPTIMER0
    TRAP_IGNORE()                              ! 0x17 - interrupt_level_7 : GPTIMER1
    TRAP_IGNORE()                              ! 0x18 - interrupt_level_8 : GPTIMER2
    TRAP_IGNORE()                              ! 0x19 - interrupt_level_9 : GPTIMER3
    TRAP_IGNORE()                              ! 0x1a - interrupt_level_10: GRSPW0
    TRAP_IGNORE()                              ! 0x1b - interrupt_level_11: GRSPW1/GRETH1
    TRAP_IGNORE()                              ! 0x1c - interrupt_level_12: GRSPW2/GRETH0
    TRAP_IGNORE()                              ! 0x1d - interrupt_level_13: B1553RT
    TRAP_IGNORE()                              ! 0x1e - interrupt_level_14: B1553BRM
    TRAP_IGNORE()                              ! 0x1f - interrupt_level_15
    /* Processor faults 1    */
    TRAP_IGNORE()                              ! 0x20 - r_register_access_error
    TRAP_IGNORE()                              ! 0x21 - instruction_access_error
    /* Unassigned traps */
    TRAP_IGNORE()                              ! 0x22
    TRAP_IGNORE()                              ! 0x23
    /* Processor faults 2    */
    TRAP_IGNORE()                              ! 0x24 cp_disabled
    TRAP_IGNORE()                              ! 0x25 unimplemented_FLUSH
    /* Unassigned traps */
    TRAP_IGNORE()                              ! 0x26
    TRAP_IGNORE()                              ! 0x27
    /* Processor faults 3    */
    TRAP_IGNORE()                              ! 0x28 cp_exception
    TRAP_IGNORE()                              ! 0x29 data_access_error
    TRAP_IGNORE()                              ! 0x2a division_by_zero
    TRAP_IGNORE()                              ! 0x2b data_store_error
    TRAP_IGNORE()                              ! 0x2c data_access_MMU_miss
    /* Unassigned traps */
    TRAP_IGNORE()                              ! 0x2d
    TRAP_IGNORE()                              ! 0x2e
    TRAP_IGNORE()                              ! 0x2f
    TRAP_IGNORE()                              ! 0x30
    TRAP_IGNORE()                              ! 0x31
    TRAP_IGNORE()                              ! 0x32
    TRAP_IGNORE()                              ! 0x33
    TRAP_IGNORE()                              ! 0x34
    TRAP_IGNORE()                              ! 0x35
    TRAP_IGNORE()                              ! 0x36
    TRAP_IGNORE()                              ! 0x37
    TRAP_IGNORE()                              ! 0x38
    TRAP_IGNORE()                              ! 0x39
    TRAP_IGNORE()                              ! 0x3a
    TRAP_IGNORE()                              ! 0x3b
    /* Processor faults 4    */
    TRAP_IGNORE()                              ! 0x3c instruction_access_MMU_miss
    /* Unassigned traps */
    TRAP_IGNORE()                              ! 0x3d
    TRAP_IGNORE()                              ! 0x3e
    TRAP_IGNORE()                              ! 0x3f
    TRAP_IGNORE()                              ! 0x40
    TRAP_IGNORE()                              ! 0x41
    TRAP_IGNORE()                              ! 0x42
    TRAP_IGNORE()                              ! 0x43
    TRAP_IGNORE()                              ! 0x44
    TRAP_IGNORE()                              ! 0x45
    TRAP_IGNORE()                              ! 0x46
    TRAP_IGNORE()                              ! 0x47
    TRAP_IGNORE()                              ! 0x48
    TRAP_IGNORE()                              ! 0x49
    TRAP_IGNORE()                              ! 0x4a
    TRAP_IGNORE()                              ! 0x4b
    TRAP_IGNORE()                              ! 0x4c
    TRAP_IGNORE()                              ! 0x4d
    TRAP_IGNORE()                              ! 0x4e
    TRAP_IGNORE()                              ! 0x4f
    TRAP_IGNORE()                              ! 0x50
    TRAP_IGNORE()                              ! 0x51
    TRAP_IGNORE()                              ! 0x52
    TRAP_IGNORE()                              ! 0x53
    TRAP_IGNORE()                              ! 0x54
    TRAP_IGNORE()                              ! 0x55
    TRAP_IGNORE()                              ! 0x56
    TRAP_IGNORE()                              ! 0x57
    TRAP_IGNORE()                              ! 0x58
    TRAP_IGNORE()                              ! 0x59
    TRAP_IGNORE()                              ! 0x5a
    TRAP_IGNORE()                              ! 0x5b
    TRAP_IGNORE()                              ! 0x5c
    TRAP_IGNORE()                              ! 0x5d
    TRAP_IGNORE()                              ! 0x5e
    TRAP_IGNORE()                              ! 0x5f
    TRAP_IGNORE()                              ! 0x60
    TRAP_IGNORE()                              ! 0x61
    TRAP_IGNORE()                              ! 0x62
    TRAP_IGNORE()                              ! 0x63
    TRAP_IGNORE()                              ! 0x64
    TRAP_IGNORE()                              ! 0x65
    TRAP_IGNORE()                              ! 0x66
    TRAP_IGNORE()                              ! 0x67
    TRAP_IGNORE()                              ! 0x68
    TRAP_IGNORE()                              ! 0x69
    TRAP_IGNORE()                              ! 0x6a
    TRAP_IGNORE()                              ! 0x6b
    TRAP_IGNORE()                              ! 0x6c
    TRAP_IGNORE()                              ! 0x6d
    TRAP_IGNORE()                              ! 0x6e
    TRAP_IGNORE()                              ! 0x6f
    TRAP_IGNORE()                              ! 0x70
    TRAP_IGNORE()                              ! 0x71
    TRAP_IGNORE()                              ! 0x72
    TRAP_IGNORE()                              ! 0x73
    TRAP_IGNORE()                              ! 0x74
    TRAP_IGNORE()                              ! 0x75
    TRAP_IGNORE()                              ! 0x76
    TRAP_IGNORE()                              ! 0x77
    TRAP_IGNORE()                              ! 0x78
    TRAP_IGNORE()                              ! 0x79
    TRAP_IGNORE()                              ! 0x7a
    TRAP_IGNORE()                              ! 0x7b
    TRAP_IGNORE()                              ! 0x7c
    TRAP_IGNORE()                              ! 0x7d
    TRAP_IGNORE()                              ! 0x7e
    TRAP_IGNORE()                              ! 0x7f
    TRAP_IGNORE()                              ! 0x80
    TRAP_IGNORE()                              ! 0x81
    TRAP_IGNORE()                              ! 0x82
    TRAP_IGNORE()                              ! 0x83
    TRAP_IGNORE()                              ! 0x84
    TRAP_IGNORE()                              ! 0x85
    TRAP_IGNORE()                              ! 0x86
    TRAP_IGNORE()                              ! 0x87
    TRAP_IGNORE()                              ! 0x88
    TRAP_IGNORE()                              ! 0x89
    TRAP_IGNORE()                              ! 0x8a
    TRAP_IGNORE()                              ! 0x8b
    TRAP_IGNORE()                              ! 0x8c
    TRAP_IGNORE()                              ! 0x8d
    TRAP_IGNORE()                              ! 0x8e
    TRAP_IGNORE()                              ! 0x8f
    TRAP_IGNORE()                              ! 0x90
    TRAP_IGNORE()                              ! 0x91
    TRAP_IGNORE()                              ! 0x92
    TRAP_IGNORE()                              ! 0x93
    TRAP_IGNORE()                              ! 0x94
    TRAP_IGNORE()                              ! 0x95
    TRAP_IGNORE()                              ! 0x96
    TRAP_IGNORE()                              ! 0x97
    TRAP_IGNORE()                              ! 0x98
    TRAP_IGNORE()                              ! 0x99
    TRAP_IGNORE()                              ! 0x9a
    TRAP_IGNORE()                              ! 0x9b
    TRAP_IGNORE()                              ! 0x9c
    TRAP_IGNORE()                              ! 0x9d
    TRAP_IGNORE()                              ! 0x9e
    TRAP_IGNORE()                              ! 0x9f
    TRAP_IGNORE()                              ! 0xa0
    TRAP_IGNORE()                              ! 0xa1
    TRAP_IGNORE()                              ! 0xa2
    TRAP_IGNORE()                              ! 0xa3
    TRAP_IGNORE()                              ! 0xa4
    TRAP_IGNORE()                              ! 0xa5
    TRAP_IGNORE()                              ! 0xa6
    TRAP_IGNORE()                              ! 0xa7
    TRAP_IGNORE()                              ! 0xa8
    TRAP_IGNORE()                              ! 0xa9
    TRAP_IGNORE()                              ! 0xaa
    TRAP_IGNORE()                              ! 0xab
    TRAP_IGNORE()                              ! 0xac
    TRAP_IGNORE()                              ! 0xad
    TRAP_IGNORE()                              ! 0xae
    TRAP_IGNORE()                              ! 0xaf
    TRAP_IGNORE()                              ! 0xb0
    TRAP_IGNORE()                              ! 0xb1
    TRAP_IGNORE()                              ! 0xb2
    TRAP_IGNORE()                              ! 0xb3
    TRAP_IGNORE()                              ! 0xb4
    TRAP_IGNORE()                              ! 0xb5
    TRAP_IGNORE()                              ! 0xb6
    TRAP_IGNORE()                              ! 0xb7
    TRAP_IGNORE()                              ! 0xb8
    TRAP_IGNORE()                              ! 0xb9
    TRAP_IGNORE()                              ! 0xba
    TRAP_IGNORE()                              ! 0xbb
    TRAP_IGNORE()                              ! 0xbc
    TRAP_IGNORE()                              ! 0xbd
    TRAP_IGNORE()                              ! 0xbe
    TRAP_IGNORE()                              ! 0xbf
    TRAP_IGNORE()                              ! 0xc0
    TRAP_IGNORE()                              ! 0xc1
    TRAP_IGNORE()                              ! 0xc2
    TRAP_IGNORE()                              ! 0xc3
    TRAP_IGNORE()                              ! 0xc4
    TRAP_IGNORE()                              ! 0xc5
    TRAP_IGNORE()                              ! 0xc6
    TRAP_IGNORE()                              ! 0xc7
    TRAP_IGNORE()                              ! 0xc8
    TRAP_IGNORE()                              ! 0xc9
    TRAP_IGNORE()                              ! 0xca
    TRAP_IGNORE()                              ! 0xcb
    TRAP_IGNORE()                              ! 0xcc
    TRAP_IGNORE()                              ! 0xcd
    TRAP_IGNORE()                              ! 0xce
    TRAP_IGNORE()                              ! 0xcf
    TRAP_IGNORE()                              ! 0xd0
    TRAP_IGNORE()                              ! 0xd1
    TRAP_IGNORE()                              ! 0xd2
    TRAP_IGNORE()                              ! 0xd3
    TRAP_IGNORE()                              ! 0xd4
    TRAP_IGNORE()                              ! 0xd5
    TRAP_IGNORE()                              ! 0xd6
    TRAP_IGNORE()                              ! 0xd7
    TRAP_IGNORE()                              ! 0xd8
    TRAP_IGNORE()                              ! 0xd9
    TRAP_IGNORE()                              ! 0xda
    TRAP_IGNORE()                              ! 0xdb
    TRAP_IGNORE()                              ! 0xdc
    TRAP_IGNORE()                              ! 0xdd
    TRAP_IGNORE()                              ! 0xde
    TRAP_IGNORE()                              ! 0xdf
    TRAP_IGNORE()                              ! 0xe0
    TRAP_IGNORE()                              ! 0xe1
    TRAP_IGNORE()                              ! 0xe2
    TRAP_IGNORE()                              ! 0xe3
    TRAP_IGNORE()                              ! 0xe4
    TRAP_IGNORE()                              ! 0xe5
    TRAP_IGNORE()                              ! 0xe6
    TRAP_IGNORE()                              ! 0xe7
    TRAP_IGNORE()                              ! 0xe8
    TRAP_IGNORE()                              ! 0xe9
    TRAP_IGNORE()                              ! 0xea
    TRAP_IGNORE()                              ! 0xeb
    TRAP_IGNORE()                              ! 0xec
    TRAP_IGNORE()                              ! 0xed
    TRAP_IGNORE()                              ! 0xee
    TRAP_IGNORE()                              ! 0xef
    TRAP_IGNORE()                              ! 0xf0
    TRAP_IGNORE()                              ! 0xf1
    TRAP_IGNORE()                              ! 0xf2
    TRAP_IGNORE()                              ! 0xf3
    TRAP_IGNORE()                              ! 0xf4
    TRAP_IGNORE()                              ! 0xf5
    TRAP_IGNORE()                              ! 0xf6
    TRAP_IGNORE()                              ! 0xf7
    TRAP_IGNORE()                              ! 0xf8
    TRAP_IGNORE()                              ! 0xf9
    TRAP_IGNORE()                              ! 0xfa
    TRAP_IGNORE()                              ! 0xfb
    TRAP_IGNORE()                              ! 0xfc
    TRAP_IGNORE()                              ! 0xfd
    TRAP_IGNORE()                              ! 0xfe
    TRAP_IGNORE()                              ! 0xff

/**
 * @brief Clear a section of memory (64 bits at a time)
 * @param %g2 start address of the memory section
 * @param %g3 end address of the memory section
 */
zero_bss:

    mov     %g0, %g1                            ! clear on double at time

zerobss_loop:

    std     %g0, [%g2]                          ! clear memory
    add     %g2, 0x08,%g2                       ! advance memory position
    cmp     %g2, %g3                            ! check if end was reached
    bl      zerobss_loop                        ! no? then continue cleaning
    nop
    retl                                        ! return to caller
    nop

/**
 * @brief PMK hard reset routine
 *
 * This routine initializes the TBR, PSR and WIM
 */
    GLOBAL(pmk_hard_reset)
SYM(pmk_hard_reset):

    /* core initialization */
    set     SYM(pmk_trap_table), %g1            ! Initialize TBR
    mov     %g1, %tbr

    set     SPARC_PSR_INIT, %g1                 ! initial PSR
    set     SPARC_WIM_INIT, %g2                 ! initial WIM

    mov     %g1, %psr                           ! set initial PSR
    mov     %g2, %wim                           ! set initial WIM
    nop; nop; nop                               ! WRPSR and WRWIM delay slots

    /* get current core index */
    rd      %asr17, %l0
    srl     %l0, 28, %l0
    and     %l0, 0xff, %l0                      ! core index

    /* setup initial stack */
    set     SYM(sparc_initial_stack), %sp       ! inital stack
    add     %l0, 0x01, %l1
    set     SPARC_STACK_MINIMUM_SIZE, %l2         ! core stack size
    umul    %l1, %l2, %l1                       ! core stack offset
    add     %sp, %l1, %sp
    andn    %sp, 0x0f, %sp                      ! align stack on 16 bytes
    mov     %sp, %fp                            ! set frame pointer

    /* disable cache */
    flush
    sta     %g0, [%g0]2

    /* disable MMU */
    call    sparc_mmu_disable
    nop

    /* check if this the primary or secondary core */
    tst     %l0
    bnz     pmk_secondary_core_init
    nop                                         ! delat slot

/**
 * @brief Primary core initialization
 */
pmk_primary_core_init:

    /* clear BSS */
    set     SYM(air_kernel_bss_start), %g2
    set     SYM(air_kernel_bss_end), %g3
    call    zero_bss
    nop

    /* call PMK entry point */
    call    pmk_init
    sub     %sp, 0x60, %sp                      ! room for the window contents

/**
 * @brief Secondary core initialization
 */
pmk_secondary_core_init:

	call   pmk_core_initialize
	sub    %sp, 0x60, %sp                       ! room for the window contents
