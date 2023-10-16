/*
 * Copyright (C) 2018-2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file slcr.h
 * \author lumm
 * \brief System Level Control Registers Definitions
 *
 * TODO too many registers. defined in place of use
 */

#ifndef SLCR_H_
#define SLCR_H_

#include <armv7.h>
#include <cp15.h>
#include <parameters.h>

/*
* CRF_APB Module: FPD Clock and Reset Control Registers
*/

typedef struct {                        
    air_u32_t err_ctrl;                      
    air_u32_t ir_status;                
    air_u32_t ir_mask;              
    air_u32_t ir_enable;            
    air_u32_t ir_disable;    
    air_u32_t reserved[2];
    air_u32_t crf_wprot;            
    //.. More registers, not needed           
} crf_apb_t;

#define CRF_APB ((crf_apb_t *)XPAR_PSU_CRF_APB_S_AXI_BASEADDR)

__FORCE_INLINE static void arm_crf_apb_ir_disable(void) {
    CRF_APB->ir_disable = 1;
}

__FORCE_INLINE static void arm_crf_apb_ir_enable(void) {
    CRF_APB->ir_enable = 1;
}

__FORCE_INLINE static void arm_crf_apb_get_status(void) {
    return CRF_APB->ir_status;
}

__FORCE_INLINE static void arm_crf_apb_clear_status(void) {
    CRF_APB->ir_status = 1;
}


/**< ************************ SLCR ****************************/

// #define UART_CLK_CTRL_CLKACT0_ENABLE    (1U << 0)
// #define UART_CLK_CTRL_CLKACT1_ENABLE    (1U << 1)
// #define UART_CLK_CTRL_SRCSEL(mask)      (mask << 4)
// #define UART_CLK_CTRL_DIVISOR(val)      (val << 8)

// #define MIO_PIN_SLOW_CMOS_EDGE          0x0
// #define MIO_PIN_FAST_CMOS_EDGE          0x1

// #define MIO_PIN_IO_LVCMOS18             (0x1)
// #define MIO_PIN_IO_LVCMOS25             (0x2)
// #define MIO_PIN_IO_LVCMOS33             (0x3)
// #define MIO_PIN_IO_HSTL                 (0x4)

// #define MIO_PIN_TRI_ENABLE              (1U << 0)
// #define MIO_PIN_L0_SEL(mask)            (mask << 1)
// #define MIO_PIN_L1_SEL(mask)            (mask << 2)
// #define MIO_PIN_L2_SEL(mask)            (mask << 3)
// #define MIO_PIN_L3_SEL(mask)            (mask << 5)
// #define MIO_PIN_SPEED(val)              (val << 8)
// #define MIO_PIN_IO_TYPE(val)            (val << 9)
// #define MIO_PIN_PULLUP(val)             (val << 12)
// #define MIO_PIN_DISABLE_RCVR            (1U << 13)

#define SLCR_UNLOCK_KEY                 0xDF0D
#define SLCR_LOCK_KEY                   0x767B

/* too many registers. defined in place of use */
typedef struct {                        /* 0xf800 0000*/
    air_u32_t scl;                      /* 0x000 */
    air_u32_t slcr_lock;                /* 0x004 */
    air_u32_t slcr_unlock;              /* 0x008 */
    air_u32_t slcr_locksta;             /* 0x00c */
} slcr_t;

#define SLCR ((slcr_t *)XPAR_PSU_FPD_SLCR_SECURE_0_S_AXI_BASEADDR)

__FORCE_INLINE static void arm_slcr_lock(void) {
    SLCR->slcr_lock = SLCR_LOCK_KEY;
}

__FORCE_INLINE static void arm_slcr_unlock(void) {
    SLCR->slcr_unlock = SLCR_UNLOCK_KEY;
}

__FORCE_INLINE static air_u32_t arm_slcr_is_locked(void) {
    return SLCR->slcr_locksta;
}

#define PERI_RST                        0x100

void arm_peripheral_soft_reset(void);
void arm_ps_reset(void);
void arm_pll_bypass_mode(void);
void arm_pll_shutdown(void);
void arm_set_cpu_clock_divisor(air_u32_t divisor);

#endif /* SLCR_H_ */
