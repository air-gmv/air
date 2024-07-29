/*
 * Copyright (C) 2018-2019  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file global_timer.c
 * \author ansi
 * \brief Generic Timer setup and start. The A53 cpu does not have a global timer, it is called a generic timer.
 * It is very similar to the global timer of the A9 but the register access is different.
 */

#include <timer.h>
#include <bsp.h>
#include <isr.h>
#include <parameters.h>
#include <configurations.h>
#ifdef PMK_DEBUG
#include <printk.h>
#endif

#define IOU_SCNTRS ((iou_scntrs_module *)XPAR_PSU_IOU_SCNTRS_S_AXI_BASEADDR)

void arm_acknowledge_gt(void) {   
    //To acknowledge the timer we can:
    // Disable the timer, but then it must be enabled again
    // Set IMASK in the control register (CNTP)
    // Update the comparator
    
    // arm_disable_global_timer();
    arm_set_tval( pmk_get_usr_us_per_tick() * 200 ); //like in the initialization
    return;
}

void arm_set_tval(air_u32_t val){
    //Set the TVAL in CNTP_TVAL register
    __asm__ volatile ( "mcr p15, 0, %0, c14, c2, 0\n"::"r" (val) );

    // #ifdef PMK_DEBUG_TIMER
    // val = 0;
    // __asm__ volatile ( "mrc p15, 0, %0, c14, c2, 0\n":"=r" (val) );
    // printk("\t CNTP_TVAL register = 0x%x\n", val);
    // #endif
    return;
}

void arm_init_global_timer(void) {

#ifdef PMK_DEBUG_TIMER
    printk("\n :: Generic Timer initialization\n"
            "    IOU_SCNTRS module at 0x%x\n"
            "    \t count_control register at 0x%x = 0x%x\n"
            "    \t counter_status register at 0x%x\n"
            "    \t cout_lower register at 0x%x\n"
            "    \t count_upper register at 0x%x\n"
            "    \t base_freq_id register at 0x%x = 0x%x\n",
            IOU_SCNTRS, &IOU_SCNTRS->cnt_ctrl, IOU_SCNTRS->cnt_ctrl, &IOU_SCNTRS->cnt_status, &IOU_SCNTRS->current_cnt_lower, &IOU_SCNTRS->current_cnt_upper, &IOU_SCNTRS->base_freq_id, IOU_SCNTRS->base_freq_id);
#endif
    /* in range 0-255 */
    air_u32_t us_per_tick = pmk_get_usr_us_per_tick();

    air_u32_t val = IOU_SCNTRS->base_freq_id;

    // First we need to write the freq of the system counter to the CNTFRQ register
    __asm__ volatile ( "mcr p15, 0, %0, c14, c0, 0\n"::"r" (val) );

    #ifdef PMK_DEBUG_TIMER
    val = 0;
    __asm__ volatile ( "mrc p15, 0, %0, c14, c0, 0\n":"=r" (val) );
    printk("\n\t CNTFRQ register = 0x%x\n", val);
    #endif

    // Calculate and set TVAL
    val = 0;

    //The system counter generates a count after N = (XPAR_CPU_CORTEXA53_0_CPU_CLK_FREQ_HZ / 2)/100 MHz 
    //Meaning that it generates a count every N/CPU_FREQ s = 1/200M s = (1/200M) * 1E6 us = 1/200 us
    // 1 count --> 1/200 us
    // x count --> us_per_tick us
    //X = us_per_tick/(1/200) = us_per_tick*200
    val = us_per_tick * 200;

    arm_set_tval(val);

    return;
}

void arm_disable_global_timer(void) {
    //Disable the timer with CNTP_CTL

    //Read what was there
    air_u32_t val = 0;
    __asm__ volatile ( "mrc p15, 0, %0, c14, c2, 1\n":"=r" (val) );

    val &= 0b000;
    __asm__ volatile ( "mcr p15, 0, %0, c14, c2, 1\n"::"r" (val) );

    #ifdef PMK_DEBUG_TIMER
    val = 0;
    __asm__ volatile ( "mrc p15, 0, %0, c14, c2, 1\n":"=r" (val) );
    printk("\t CNTP_CTL register = 0x%x\n\n", val);
    #endif

    arm_instruction_synchronization_barrier();
    return;
}

void arm_start_global_timer(void) {
    //Enable the timer with CNTP_CTL

    //Read what was there
    air_u32_t val = 0;
    __asm__ volatile ( "mrc p15, 0, %0, c14, c2, 1\n":"=r" (val) );

    val |= 0b001;
    __asm__ volatile ( "mcr p15, 0, %0, c14, c2, 1\n"::"r" (val) );

    #ifdef PMK_DEBUG_TIMER
    val = 0;
    __asm__ volatile ( "mrc p15, 0, %0, c14, c2, 1\n":"=r" (val) );
    printk("\t CNTP_CTL register = 0x%x\n\n", val);
    #endif

    arm_instruction_synchronization_barrier();
    return;
}

air_u64_t arm_read_global_timer(void) {

    air_u32_t lower = 0;
    air_u32_t upper = 0;
    
    // Read the CNTPCT register
    __asm__ volatile ("mrrc p15, 0, %0, %1, c14\n":"=r" (lower), "=r" (upper) );

    air_u64_t result = (air_u64_t) upper << 32U | lower;

#ifdef PMK_DEBUG
    printk("\n    :: timer count = 0x%x%08x\n", upper, lower);
#endif
    return result;
}