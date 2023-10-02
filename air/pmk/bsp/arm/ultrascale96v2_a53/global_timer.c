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

air_u32_t arm_determine_trigger_bit(air_u32_t us_per_tick){
    // The counter generates a tick every N clock pulses
    // (From Ultrascle+ TRM, Timers and Counters -> Physical Counter section)
    air_u32_t N = (XPAR_CPU_CORTEXA53_0_CPU_CLK_FREQ_HZ / 2) / 100000000;

    // This means that it generates a tick every N/CPU_CLK_FREQ seconds;
    // So the desired period is (in ticks)
    air_u32_t period = (us_per_tick * 1E6) / (N/XPAR_CPU_CORTEXA53_0_CPU_CLK_FREQ_HZ);
    
    // Find the changing bit that represents the period
    
    //TODO: For now assume that it is the rightmost bit of the binary representation of the period, it is only true for powers of 2
    if (period == 0) {
        return 0;  // No set bits
    }

    air_u32_t position = -1;

    // Right shift the period until the rightmost bit is 1.
    while ((period & 1) == 0) {
        period >>= 1;
        position++;
    }

    return position;
}

void arm_acknowledge_gt(void) {
    //TODO ???
    return;
}


void arm_init_global_timer(void) {

    /* in range 0-255 */
    air_u32_t us_per_tick = pmk_get_usr_us_per_tick();

    //First program the counter frequency
    // According to the registers reference we must "program this register to match the clock frequency of the timestamp generator"
    IOU_SCNTRS->base_freq_id = 0x5F5E100; //XPAR_PSU_CORTEXA53_0_TIMESTAMP_CLK_FREQ (100MHz) in hex
    //Should it be XPAR_PSU_CORTEXA53_0_CPU_CLK_FREQ_HZ instead?

    // Use the CNTKCTL, Counter-timer Kernel Control register to setup the timer
    // to generate an event periodically.
    // Set the EVNTI, bits [7:4] to select wich bit of the counter register triggers the event (select the period ?)

    //TODO: Can i just write the value to the specific bits of the register or do i need to read it first and then write "what i want"|"what was alreadt there"?
    __asm__ volatile ( "mcr p15, 0, %0, c14, c1, 0\n"::"r" (1 << arm_determine_trigger_bit(us_per_tick) ) );

#ifdef PMK_DEBUG
    printk("\n :: Generic Timer initialization\n"
            "    IOU_SCNTRS module at 0x%x\n"
            "    trigger bit = %d\n\n",
            IOU_SCNTRS, arm_determine_trigger_bit(us_per_tick) );
#endif
}

void arm_start_global_timer(void) {
    // Set the EVNTEN, bit [2] of the CNTP_CTL register to enable the event stream?
    __asm__ volatile ( "mcr p15, 0, %0, c14, c2, 1\n"::"r" (1 << 2U ) );

    arm_instruction_synchronization_barrier();
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

// ??
// void arm_acknowledge_gt(void) {
//     GT->irq_status = 0x1;
// }

// air_u32_t arm_determine_gt_prescaler(air_u32_t us_per_tick) {

//     air_u32_t freq = XPAR_CPU_CORTEXA53_0_CPU_CLK_FREQ_HZ / 2000000;
//     air_u32_t prescaler_n, us_per_tick_calc;
//     air_u32_t prescaler = 0;
//     air_u64_t counter;
//     air_u32_t error;
//     air_u32_t best_error = ~(0x0);

// #ifdef PMK_DEBUG_TIMER
//     printk("        :: global timer prescaler calculation\n"
//            "           us_per_tick = %d\n"
//            "           freq = %dMHz\n\n", us_per_tick, freq);
// #endif

//     for (prescaler_n = 256; prescaler_n > 0; prescaler_n--) {

//         counter = (air_u64_t)( ((us_per_tick * freq) / (prescaler_n + 1)) - 1 );

// #ifdef PMK_DEBUG_TIMER
//         air_u32_t c_l = (air_u32_t)(counter & 0xffffffff);
//         air_u32_t c_h = (air_u32_t)(counter >> 32U);
//         printk("        counter = 0x%x %08x\n", c_h, c_l);
// #endif

//         us_per_tick_calc = (air_u32_t)(((counter + 1) * (prescaler_n + 1)) / freq);

//         error = us_per_tick - us_per_tick_calc;
// #ifdef PMK_DEBUG_TIMER
//         printk("        prescaler %ld -> error %ld\n"
//                "        us_per_tick %ld, us_per_tick_calc %ld\n",
//                prescaler_n, error, us_per_tick, us_per_tick_calc);
// #endif
//         if (error == 0) {
//             best_error = error;
//             prescaler = prescaler_n;
//             break;
//         }

//         if (error < best_error) {
//             best_error = error;
//             prescaler = prescaler_n;
//         }
//     }

//     return prescaler;
// }
