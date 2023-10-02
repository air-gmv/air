/*
 * Copyright (C) 2020  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */
/**
 * \file ddr.h
 * \author lumm
 * \brief DDR memory controller access
 */

#ifndef DDR_H_
#define DDR_H_

#include <parameters.h>

typedef struct {
    air_u32_t ddrc_ctrl;
    air_u32_t Two_rank_cfg;
    air_u32_t HPR_reg;
    air_u32_t LPR_reg;
    air_u32_t WR_reg;
    air_u32_t DRAM_param_reg0;
    air_u32_t DRAM_param_reg1;
    air_u32_t DRAM_param_reg2;
    air_u32_t DRAM_param_reg3;
    air_u32_t DRAM_param_reg4;
    air_u32_t DRAM_init_param;
    air_u32_t DRAM_EMR_reg;
    air_u32_t DRAM_EMR_MR_reg;
    air_u32_t DRAM_burst8_rdwr;
    air_u32_t DRAM_disable_DQ;
    air_u32_t DRAM_addr_map_bank;
    air_u32_t DRAM_addr_map_col;
    air_u32_t DRAM_addr_map_row;
    air_u32_t DRAM_ODT_reg;
    air_u32_t phy_dbg_reg;
    air_u32_t phy_cmd_timeout_rddata_cpt;
    air_u32_t mode_sts_reg;
    air_u32_t DLL_calib;
    air_u32_t ODT_delay_hold;
    air_u32_t ctrl_reg1;
    air_u32_t ctrl_reg2;
    air_u32_t ctrl_reg3;
    air_u32_t ctrl_reg4;
    air_u32_t ctrl_reg5;
    air_u32_t ctrl_reg6;
    air_u32_t CHE_REFRESH_TIMER01;
    air_u32_t CHE_T_ZQ;
    air_u32_t CHE_T_ZQ_Short_Interval_Reg;
    air_u32_t deep_pwrdwn_reg;
    air_u32_t reg_2c;
    air_u32_t reg_2d;
    air_u32_t dfi_timing;
    air_u32_t CHE_ECC_CONTROL_REG_OFFSET;
    air_u32_t CHE_CORR_ECC_LOG_REG_OFFSET;
    air_u32_t CHE_CORR_ECC_ADDR_REG_OFFSET;
    air_u32_t CHE_CORR_ECC_DATA_31_0_REG_OFFSET;
    air_u32_t CHE_CORR_ECC_DATA_63_32_REG_OFFSET;
    air_u32_t CHE_CORR_ECC_DATA_71_64_REG_OFFSET;
    air_u32_t CHE_UNCORR_ECC_LOG_REG_OFFSET;
    air_u32_t CHE_UNCORR_ECC_ADDR_REG_OFFSET;
    air_u32_t CHE_UNCORR_ECC_DATA_31_0_REG_OFFSET;
    air_u32_t CHE_UNCORR_ECC_DATA_63_32_REG_OFFSET;
    air_u32_t CHE_UNCORR_ECC_DATA_71_64_REG_OFFSET;
    air_u32_t CHE_ECC_STATS_REG_OFFSET;
    air_u32_t ECC_scrub;
    air_u32_t CHE_ECC_CORR_BIT_MASK_31_0_REG_OFFSET;
    air_u32_t CHE_ECC_CORR_BIT_MASK_63_32_REG_OFFSET;
    air_u32_t phy_rcvr_enable;
    air_u32_t PHY_Config0;
    air_u32_t PHY_Config1;
    air_u32_t PHY_Config2;
    air_u32_t PHY_Config3;
    air_u32_t phy_init_ratio0;
    air_u32_t phy_init_ratio1;
    air_u32_t phy_init_ratio2;
    air_u32_t phy_init_ratio3;
    air_u32_t phy_rd_dqs_cfg0;
    air_u32_t phy_rd_dqs_cfg1;
    air_u32_t phy_rd_dqs_cfg2;
    air_u32_t phy_rd_dqs_cfg3;
    air_u32_t phy_wr_dqs_cfg0;
    air_u32_t phy_wr_dqs_cfg1;
    air_u32_t phy_wr_dqs_cfg2;
    air_u32_t phy_wr_dqs_cfg3;
    air_u32_t phy_we_cfg0;
    air_u32_t phy_we_cfg1;
    air_u32_t phy_we_cfg2;
    air_u32_t phy_we_cfg3;
    air_u32_t wr_data_slv0;
    air_u32_t wr_data_slv1;
    air_u32_t wr_data_slv2;
    air_u32_t wr_data_slv3;
    air_u32_t reg_64;
    air_u32_t reg_65;
    air_u32_t reg69_6a0;
    air_u32_t reg69_6a1;
    air_u32_t reg6c_6d2;
    air_u32_t reg6c_6d3;
    air_u32_t reg6e_710;
    air_u32_t reg6e_711;
    air_u32_t reg6e_712;
    air_u32_t reg6e_713;
    air_u32_t phy_dll_sts0;
    air_u32_t phy_dll_sts1;
    air_u32_t phy_dll_sts2;
    air_u32_t phy_dll_sts3;
    air_u32_t dll_lock_sts;
    air_u32_t phy_ctrl_sts;
    air_u32_t phy_ctrl_sts_reg2;
    air_u32_t axi_id;
    air_u32_t page_mask;
    air_u32_t axi_priority_wr_port0;
    air_u32_t axi_priority_wr_port1;
    air_u32_t axi_priority_wr_port2;
    air_u32_t axi_priority_wr_port3;
    air_u32_t axi_priority_rd_port0;
    air_u32_t axi_priority_rd_port1;
    air_u32_t axi_priority_rd_port2;
    air_u32_t axi_priority_rd_port3;
    air_u32_t excl_access_cfg0;
    air_u32_t excl_access_cfg1;
    air_u32_t excl_access_cfg2;
    air_u32_t excl_access_cfg3;
    air_u32_t mode_reg_read;
    air_u32_t lpddr_ctrl0;
    air_u32_t lpddr_ctrl1;
    air_u32_t lpddr_ctrl2;
    air_u32_t lpddr_ctrl3;
} ddrc_t;

#define DDRC ((ddrc_t *)XPAR_PSU_DDRC_0_BASEADDR)


#define DDR_CLK_CTRL (air_uptr_t *)(XPAR_PSU_FPD_SLCR_SECURE_0_S_AXI_BASEADDR + 0x124)
#define DCI_CLK_CTRL (air_uptr_t *)(XPAR_PSU_FPD_SLCR_SECURE_0_S_AXI_BASEADDR + 0x128)

__FORCE_INLINE static void arm_ddr_self_refresh(void) {

    air_u32_t timeout = 0;

    DDRC->ctrl_reg1 |= (1 << 12);
    DDRC->DRAM_param_reg3 |= (1 << 23);

    while((DDRC->mode_sts_reg & 0x7) != 3) {
        if (++timeout > 1000) return;
    };

    timeout = 0;
    while((DDRC->mode_sts_reg & (0x1f << 16)) ||
        (DDRC->mode_sts_reg & (0x3f << 10)) ||
        (DDRC->mode_sts_reg & (0x3f << 4))) {
        if (++timeout > 1000) return;
    };

    timeout = 0;
    while(++timeout < 1000);

    *DDR_CLK_CTRL &= ~3;
    *DCI_CLK_CTRL &= ~1;
}

#endif /* DDR_H_ */
