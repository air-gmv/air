/**
 *  @file 
 *  @brief Mode independent part of the GR1553 driver.
 *
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT
 *	
 *  Modifications: 
 *    - Back port to RTEMS 4.8 (no drvmgr)
 *    - Removed Interrupts
 *
 *  @author Cl�udio Silva
 *	
 */
 
/*  
 * GR1553B driver, used by BC, RT and/or BM driver
 *
 *  COPYRIGHT (c) 2010.
 *  Aeroflex Gaisler.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  2010-03-15, Daniel Hellstrom <daniel@gaisler.com>
 *    Created
 *
 *
 * OVERVIEW
 * ========
 * This driver controls the GR1553B device regardless of interfaces supported
 * (BC, RT and/or BM). The device can be located at an on-chip AMBA or an
 * AMBA-over-PCI bus. This driver provides an interface for the BC, RT and BM
 * drivers to use. Since the different interfaces are accessed over the same
 * register interface on the same core, the other drivers must share a GR1553B
 * device. Any combination of interface functionality is supported, but the RT 
 * and BC functionality can nnot be used simultaneously due to hardware
 * limitation.
 *
 */

#ifndef __GR1553B_H__
#define __GR1553B_H__
 
#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <stdint.h>
#include <iop.h>
#include <IOPmilstd_config.h>


/**
 * @brief The GR1553B register mapping
 */
struct gr1553b_regs {

	/* Common Registers */
	volatile uint32_t irq; 		/**< 0x00 IRQ register */
	volatile uint32_t imask;	/**< 0x04 IRQ enable mask */
	
	int unused0[(0x10-0x08)/4];		/* Padding */
	
	volatile uint32_t hwcfg;	/**< 0x10 HW config register */

	int unused1[(0x40-0x14)/4];		/* Padding */

	/* BC Registers */
	volatile uint32_t bc_stat;	/**< 0x40 BC status */
	volatile uint32_t bc_ctrl;	/**< 0x44 BC Action register */
	volatile uint32_t bc_bd;	/**< 0x48 BC transfer list pointer */
	volatile uint32_t bc_abd;	/**< 0x4c BC async list pointer */
	volatile uint32_t bc_timer;	/**< 0x50 BC timer register */
	volatile uint32_t bc_wake;	/**< 0x54 BC wakeup control register */
	volatile uint32_t bc_irqptr;/**< 0x58 BC transfer IRQ pointer */
	volatile uint32_t bc_busmsk;/**< 0x5C BC per-RT bus mask register */

	int unused2[(0x68-0x60)/4];		/* Padding */

	volatile uint32_t bc_slot;	/**< 0x48 BC Current BD pointer */
	volatile uint32_t bc_aslot;	/**< 0x4c BC Current async BD pointer */

	int unused3[(0x80-0x70)/4];		/* Padding */

	/* RT Registers */
	volatile uint32_t rt_stat;	/**< 0x80 RT status */
	volatile uint32_t rt_cfg;	/**< 0x84 RT config register */
	volatile uint32_t rt_stat2;	/**< 0x88 RT bus status bits */
	volatile uint32_t rt_statw;	/**< 0x8c RT status words */
	volatile uint32_t rt_sync;	/**< 0x90 RT bus synchronize */
	volatile uint32_t rt_tab;	/**< 0x94 RT subaddress table base */
	volatile uint32_t rt_mcctrl;/**< 0x98 RT valid mode code mask */
	
	int unused4[(0xa4-0x9c)/4];
	
	volatile uint32_t rt_ttag;	/**< 0xa4 RT time tag register */
	
	int unused5;					/* 0xa8 RESERVED */
	
	volatile uint32_t rt_evsz;	/**< 0xac RT event log end pointer */
	volatile uint32_t rt_evlog;	/**< 0xb0 RT event log position */
	volatile uint32_t rt_evirq;	/**< 0xb4 RT event log IRQ position */

	int unused6[(0xc0-0xb8)/4];		/* Padding */

	/* BM Registers */
	volatile uint32_t bm_stat;	/**< 0xc0 BM status */
	volatile uint32_t bm_ctrl;	/**< 0xc4 BM control register */
	volatile uint32_t bm_adr;	/**< 0xc8 BM address filter */
	volatile uint32_t bm_subadr;/**< 0xcc BM subaddress filter */
	volatile uint32_t bm_mc;	/**< 0xd0 BM mode code filter */
	volatile uint32_t bm_start;	/**< 0xd4 BM log start address */
	volatile uint32_t bm_end;	/**< 0xd8 BM log size/alignment mask */
	volatile uint32_t bm_pos;	/**< 0xdc BM log position */
	volatile uint32_t bm_ttag;	/**< 0xe0 BM time tag register */
};

/**
 * @brief user configuration options 
 */
typedef struct {
	int operating_mode;				/**< Operative mode for this core: RT, BC or BM*/
	
	/* BC specific */
	uint8_t msg_timeout;			/**< Extra RT status timeout in units of 4us */
	uint8_t retry_mode;				/**< Retry Mode: 0 - same bus, 1 - alternating buses */
	
	/* rt specific options */
	uint8_t rtaddress;				/**< RT Address [0,30] */
	uint32_t modecode;				/**< Mode codes enable/disable */
	uint32_t enabled_subs;			/**< Enabled subaddresses bit map */
	unsigned int databufs_per_sub;  /**< Number of data buffer per subaddress */
	unsigned short time_res;		/**< Time tag resolution in us */
} grb_user_config_t;

typedef uint32_t milstd_data_buf[16];

/**
 * @brief Internal BC driver structure
 */
typedef struct {
	int minor;						/**< minor number of this device */
	unsigned int irq;				/**< IRQ */
	unsigned int open;				/**< Was this device already opened */
	unsigned int started;			/**< device has started processing */
	unsigned int available_modes;	/**< Modes that are available in the HW */
	grb_user_config_t *user_config;	/**< user defined configurations */
	void *mem_start;				/**< pointer to device's memory */
	
	/* bc specific */
	bc_command_t *cl;					/**< pointer to user's list */
	unsigned int cl_size;				/**< user command list size */
	struct gr1553bc_bd_tr *sync; 		/**< current bc command list */
	struct gr1553bc_bd_tr *last_read; 	/**< current bc command list */
	struct gr1553bc_bd_tr *assync; 		/**< current bc command list */
	milstd_data_buf *buf_mem_start;		/**< pointer to device's buffer memory */
	iop_chain_control shortcut[32];			/**< Used as a shortcut to map write requests to their respective command */
	
	/* rt specific */
	struct gr1553rt_sa* sa_table;		/**< Subaddress table used for RT mode */
	struct gr1553rt_bd* bds_start;		/**< Start Address for buffer descriptors */
	struct gr1553rt_bd* last_tx[32];	/**< pointer to last bd transmitted */
	struct gr1553rt_bd* last_rx[32];	/**< pointer to last bd received */
	
	
	struct gr1553b_regs *regs;			/**< Core register mapping */

} grb_priv;

/* Available Modes */
#define FEAT_BC 0x1
#define FEAT_RT 0x2
#define FEAT_BM 0x4
#define FEAT_ALL 0x7

/* Macros to read and write from a register */
#define GR1553B_WRITE_REG(adr, val) *(volatile uint32_t *)(adr) = (val)
#define GR1553B_READ_REG(adr) (*(volatile uint32_t *)(adr))

/* Write keys */
#define GR1553BC_KEY 0x15520000
#define GR1553RT_KEY 0x15530000

/* IRQ Definitions */
#define GR1553BC_IRQLOG_SIZE 64
#define GR1553BC_IRQLOG_CNT (GR1553BC_IRQLOG_SIZE/sizeof(uint32_t))

/* IRQ Flag Register */
#define GR1553B_IRQ_BCEV_BIT	0
#define GR1553B_IRQ_BCD_BIT		1
#define GR1553B_IRQ_BCWK_BIT	2
#define GR1553B_IRQ_RTEV_BIT	8
#define GR1553B_IRQ_RTD_BIT		9
#define GR1553B_IRQ_RTTE_BIT	10
#define GR1553B_IRQ_BMD_BIT		16
#define GR1553B_IRQ_BMTOF_BIT   17

#define GR1553B_IRQ_BCEV	(1<<GR1553B_IRQ_BCEV_BIT)
#define GR1553B_IRQ_BCD		(1<<GR1553B_IRQ_BCD_BIT)
#define GR1553B_IRQ_BCWK	(1<<GR1553B_IRQ_BCWK_BIT)
#define GR1553B_IRQ_RTEV	(1<<GR1553B_IRQ_RTEV_BIT)
#define GR1553B_IRQ_RTD		(1<<GR1553B_IRQ_RTD_BIT)
#define GR1553B_IRQ_RTTE	(1<<GR1553B_IRQ_RTTE_BIT)
#define GR1553B_IRQ_BMD		(1<<GR1553B_IRQ_BMD_BIT)
#define GR1553B_IRQ_BMTOF   (1<<GR1553B_IRQ_BMTOF_BIT)

/* IRQ Enable Register */
#define GR1553B_IRQEN_BCEVE_BIT	0
#define GR1553B_IRQEN_BCDE_BIT	1
#define GR1553B_IRQEN_BCWKE_BIT	2
#define GR1553B_IRQEN_RTEVE_BIT	8
#define GR1553B_IRQEN_RTDE_BIT	9
#define GR1553B_IRQEN_RTTEE_BIT	10
#define GR1553B_IRQEN_BMDE_BIT	16
#define GR1553B_IRQEN_BMTOE_BIT	17

#define GR1553B_IRQEN_BCEVE	(1<<GR1553B_IRQEN_BCEVE_BIT)
#define GR1553B_IRQEN_BCDE	(1<<GR1553B_IRQEN_BCDE_BIT)
#define GR1553B_IRQEN_BCWKE	(1<<GR1553B_IRQEN_BCWKE_BIT)
#define GR1553B_IRQEN_RTEVE	(1<<GR1553B_IRQEN_RTEVE_BIT)
#define GR1553B_IRQEN_RTDE	(1<<GR1553B_IRQEN_RTDE_BIT)
#define GR1553B_IRQEN_RTTEE	(1<<GR1553B_IRQEN_RTTEE_BIT)
#define GR1553B_IRQEN_BMDE	(1<<GR1553B_IRQEN_BMDE_BIT)
#define GR1553B_IRQEN_BMTOE	(1<<GR1553B_IRQEN_BMTOE_BIT)

/* BC Status Register */
#define GR1553B_BC_STAT_SCST_BIT	0
#define GR1553B_BC_STAT_SCADL_BIT	3
#define GR1553B_BC_STAT_ASST_BIT	8
#define GR1553B_BC_STAT_ASADL_BIT	11
#define GR1553B_BC_STAT_BCSUP_BIT	31

#define GR1553B_BC_STAT_SCST		(0x3<<GR1553B_BC_STAT_SCST_BIT)
#define GR1553B_BC_STAT_SCADL		(0x1f<<GR1553B_BC_STAT_SCADL_BIT)
#define GR1553B_BC_STAT_ASST		(0x3<<GR1553B_BC_STAT_ASST_BIT)
#define GR1553B_BC_STAT_ASADL		(0x1f<<GR1553B_BC_STAT_ASADL_BIT)
#define GR1553B_BC_STAT_BCSUP		(1<<GR1553B_BC_STAT_BCSUP_BIT)

/* BC Action Register */
#define GR1553B_BC_ACT_SCSRT_BIT	0
#define GR1553B_BC_ACT_SCSUS_BIT	1
#define GR1553B_BC_ACT_SCSTP_BIT	2
#define GR1553B_BC_ACT_SETT_BIT		3
#define GR1553B_BC_ACT_CLRT_BIT		4
#define GR1553B_BC_ACT_ASSRT_BIT	8
#define GR1553B_BC_ACT_ASSTP_BIT	9
#define GR1553B_BC_ACT_BCKEY_BIT	16

#define GR1553B_BC_ACT_SCSRT		(1<<GR1553B_BC_ACT_SCSRT_BIT)
#define GR1553B_BC_ACT_SCSUS		(1<<GR1553B_BC_ACT_SCSUS_BIT)
#define GR1553B_BC_ACT_SCSTP		(1<<GR1553B_BC_ACT_SCSTP_BIT)
#define GR1553B_BC_ACT_SETT			(1<<GR1553B_BC_ACT_SETT_BIT)
#define GR1553B_BC_ACT_CLRT			(1<<GR1553B_BC_ACT_CLRT_BIT)
#define GR1553B_BC_ACT_ASSRT		(1<<GR1553B_BC_ACT_ASSRT_BIT)
#define GR1553B_BC_ACT_ASSTP		(1<<GR1553B_BC_ACT_ASSTP_BIT)
#define GR1553B_BC_ACT_BCKEY		(0xffff<<GR1553B_BC_ACT_BCKEY_BIT)

/* BC Timer Register */
#define GR1553B_BC_TIMER_SCTM_BIT	0

#define GR1553B_BC_TIMER_SCTM		(0xffffff<<GR1553B_BC_TIMER_SCTM_BIT)

/* BC Wake-up control Register */
#define GR1553B_BC_WAKE_TIME_BIT	0
#define GR1553B_BC_WAKE_WKEN_BIT	31

#define GR1553B_BC_WAKE_TIME		(0xffffff<<GR1553B_BC_WAKE_TIME_BIT)
#define GR1553B_BC_WAKE_WKEN		(1<GR1553B_BC_WAKE_WKEN_BIT)

/* RT status Register */
#define GR1553B_RT_STAT_RUN_BIT		0
#define GR1553B_RT_STAT_SHDB_BIT	1
#define GR1553B_RT_STAT_SHDA_BIT	2
#define GR1553B_RT_STAT_ACT_BIT		3
#define GR1553B_RT_STAT_RTSUP_BIT	31

#define GR1553B_RT_STAT_RUN			(1<<GR1553B_RT_STAT_RUN_BIT)
#define GR1553B_RT_STAT_SHDB		(1<<GR1553B_RT_STAT_SHDB_BIT)
#define GR1553B_RT_STAT_SHDA		(1<<GR1553B_RT_STAT_SHDA_BIT)
#define GR1553B_RT_STAT_ACT			(1<<GR1553B_RT_STAT_ACT_BIT)
#define GR1553B_RT_STAT_RTSUP		(1<<GR1553B_RT_STAT_RTSUP_BIT)


/* RT Config Register */
#define GR1553B_RT_CFG_RTEN_BIT		0
#define GR1553B_RT_CFG_RTADDR_BIT	1
#define GR1553B_RT_CFG_RTKEY_BIT	16

#define GR1553B_RT_CFG_RTEN			(1<<GR1553B_RT_CFG_RTEN_BIT)
#define GR1553B_RT_CFG_RTADDR		(1<<GR1553B_RT_CFG_RTADDR_BIT)
#define GR1553B_RT_CFG_RTKEY		(0xffff<<GR1553B_RT_CFG_RTKEY_BIT)

/* RT Bus Status Register */
#define GR1553B_RT_STAT2_RTEN_BIT	0
#define GR1553B_RT_STAT2_DBCA_BIT	1
#define GR1553B_RT_STAT2_SSF_BIT	2
#define GR1553B_RT_STAT2_BUSY_BIT	3
#define GR1553B_RT_STAT2_SREQ_BIT	4

#define GR1553B_RT_STAT2_RTEN		(1<<GR1553B_RT_STAT2_RTEN_BIT)
#define GR1553B_RT_STAT2_DBCA		(1<<GR1553B_RT_STAT2_DBCA_BIT)
#define GR1553B_RT_STAT2_SSF		(1<<GR1553B_RT_STAT2_SSF_BIT)
#define GR1553B_RT_STAT2_BUSY		(1<<GR1553B_RT_STAT2_BUSY_BIT)
#define GR1553B_RT_STAT2_SREQ		(1<<GR1553B_RT_STAT2_RTEN_BIT)

/* RT Status Words Register */
#define GR1553B_RT_STATW_VECW_BIT	0
#define GR1553B_RT_STATW_BITW_BIT	16

#define GR1553B_RT_STATW_VECW		(0xffff<<GR1553B_RT_STATW_VECW_BIT)
#define GR1553B_RT_STATW_BITW		(0xffff<<GR1553B_RT_STATW_BITW_BIT)

/* RT Sync Register */
#define GR1553B_RT_SYNC_SYD_BIT		0
#define GR1553B_RT_SYNC_SYTM_BIT	16

#define GR1553B_RT_SYNC_SYD			(0xffff<<GR1553B_RT_SYNC_SYD_BIT)
#define GR1553B_RT_SYNC_SYTM		(0xffff<<GR1553B_RT_SYNC_SYTM_BIT)

/* RT Sub adress table Register */
#define GR1553B_RT_TAB_SATB_BIT		0

#define GR1553B_RT_TAB_SATB			(0xffff<<GR1553B_RT_TAB_SATB_BIT)

/* RT Mode code control Register */
#define GR1553B_RT_MCCTRL_S_BIT		0
#define GR1553B_RT_MCCTRL_SB_BIT	2
#define GR1553B_RT_MCCTRL_SD_BIT	4
#define GR1553B_RT_MCCTRL_SDB_BIT	6
#define GR1553B_RT_MCCTRL_TS_BIT	8
#define GR1553B_RT_MCCTRL_TSB_BIT	10
#define GR1553B_RT_MCCTRL_TVW_BIT	12
#define GR1553B_RT_MCCTRL_TBW_BIT	14
#define GR1553B_RT_MCCTRL_DBC_BIT	16
#define GR1553B_RT_MCCTRL_IST_BIT	18
#define GR1553B_RT_MCCTRL_ISTB_BIT	20
#define GR1553B_RT_MCCTRL_ITF_BIT	22
#define GR1553B_RT_MCCTRL_ITFB_BIT	24
#define GR1553B_RT_MCCTRL_RRT_BIT	26
#define GR1553B_RT_MCCTRL_RRTB_BIT	28

#define GR1553B_RT_MCCTRL_S		(1<<GR1553B_RT_MCCTRL_S_BIT)
#define GR1553B_RT_MCCTRL_SB	(1<<GR1553B_RT_MCCTRL_SB_BIT)
#define GR1553B_RT_MCCTRL_SD	(1<<GR1553B_RT_MCCTRL_SD_BIT)
#define GR1553B_RT_MCCTRL_SDB	(1<<GR1553B_RT_MCCTRL_SDB_BIT)
#define GR1553B_RT_MCCTRL_TS	(1<<GR1553B_RT_MCCTRL_TS_BIT)
#define GR1553B_RT_MCCTRL_TSB	(1<<GR1553B_RT_MCCTRL_TSB_BIT)
#define GR1553B_RT_MCCTRL_TVW	(1<<GR1553B_RT_MCCTRL_TVW_BIT)
#define GR1553B_RT_MCCTRL_TBW	(1<<GR1553B_RT_MCCTRL_TBW_BIT)
#define GR1553B_RT_MCCTRL_DBC	(1<<GR1553B_RT_MCCTRL_DBC_BIT)
#define GR1553B_RT_MCCTRL_IST	(1<<GR1553B_RT_MCCTRL_IST_BIT)
#define GR1553B_RT_MCCTRL_ISTB	(1<<GR1553B_RT_MCCTRL_ISTB_BIT)
#define GR1553B_RT_MCCTRL_ITF	(1<<GR1553B_RT_MCCTRL_ITF_BIT)
#define GR1553B_RT_MCCTRL_ITFB	(1<<GR1553B_RT_MCCTRL_ITFB_BIT)
#define GR1553B_RT_MCCTRL_RRT	(1<<GR1553B_RT_MCCTRL_RRT_BIT)
#define GR1553B_RT_MCCTRL_RRTB	(1<<GR1553B_RT_MCCTRL_RRTB_BIT)

/* RT Time Tag control Register */
#define GR1553B_RT_TTAG_TVAL_BIT	0
#define GR1553B_RT_TTAG_TRES_BIT	16

#define GR1553B_RT_TTAG_TVAL		(0xffff<<GR1553B_RT_TTAG_TVAL_BIT)
#define GR1553B_RT_TTAG_TRES		(0xffff<<GR1553B_RT_TTAG_TRES_BIT)

/* BM Control Register */
#define GR1553B_BM_STAT_BMSUP_BIT	31

#define GR1553B_BM_STAT_BMSUP		(1<<GR1553B_BM_STAT_BMSUP_BIT)

/* BM Control Register */
#define GR1553B_BM_CTRL_BMEN_BIT	0
#define GR1553B_BM_CTRL_MANL_BIT	1
#define GR1553B_BM_CTRL_UDWL_BIT	2
#define GR1553B_BM_CTRL_IMCL_BIT	3

#define GR1553B_BM_CTRL_BMEN	(1<<GR1553B_BM_CTRL_BMEN_BIT)
#define GR1553B_BM_CTRL_MANL	(1<<GR1553B_BM_CTRL_MANL_BIT)
#define GR1553B_BM_CTRL_UDWL	(1<<GR1553B_BM_CTRL_UDWL_BIT)
#define GR1553B_BM_CTRL_IMCL	(1<<GR1553B_BM_CTRL_IMCL_BIT)

/* BM RT Mode code filter Register */
#define GR1553B_BM_MC_S_BIT	0
#define GR1553B_BM_MC_SB_BIT	1
#define GR1553B_BM_MC_SD_BIT	2
#define GR1553B_BM_MC_SDB_BIT	3
#define GR1553B_BM_MC_TS_BIT	4
#define GR1553B_BM_MC_TSB_BIT	5
#define GR1553B_BM_MC_TVW_BIT	6
#define GR1553B_BM_MC_TBW_BIT	7
#define GR1553B_BM_MC_DBC_BIT	8
#define GR1553B_BM_MC_IST_BIT	9
#define GR1553B_BM_MC_ISTB_BIT	10
#define GR1553B_BM_MC_ITF_BIT	11
#define GR1553B_BM_MC_ITFB_BIT	12
#define GR1553B_BM_MC_RRT_BIT	13
#define GR1553B_BM_MC_RRTB_BIT	14
#define GR1553B_BM_MC_TSW_BIT	15
#define GR1553B_BM_MC_TLC_BIT	16
#define GR1553B_BM_MC_STS_BIT	17
#define GR1553B_BM_MC_STSB_BIT	18

#define GR1553B_BM_MC_S		(1<<GR1553B_BM_MC_S_BIT)
#define GR1553B_BM_MC_SB	(1<<GR1553B_BM_MC_SB_BIT)
#define GR1553B_BM_MC_SD	(1<<GR1553B_BM_MC_SD_BIT)
#define GR1553B_BM_MC_SDB	(1<<GR1553B_BM_MC_SDB_BIT)
#define GR1553B_BM_MC_TS	(1<<GR1553B_BM_MC_TS_BIT)
#define GR1553B_BM_MC_TSB	(1<<GR1553B_BM_MC_TSB_BIT)
#define GR1553B_BM_MC_TVW	(1<<GR1553B_BM_MC_TVW_BIT)
#define GR1553B_BM_MC_TBW	(1<<GR1553B_BM_MC_TBW_BIT)
#define GR1553B_BM_MC_DBC	(1<<GR1553B_BM_MC_DBC_BIT)
#define GR1553B_BM_MC_IST	(1<<GR1553B_BM_MC_IST_BIT)
#define GR1553B_BM_MC_ISTB	(1<<GR1553B_BM_MC_ISTB_BIT)
#define GR1553B_BM_MC_ITF	(1<<GR1553B_BM_MC_ITF_BIT)
#define GR1553B_BM_MC_ITFB	(1<<GR1553B_BM_MC_ITFB_BIT)
#define GR1553B_BM_MC_RRT	(1<<GR1553B_BM_MC_RRT_BIT)
#define GR1553B_BM_MC_RRTB	(1<<GR1553B_BM_MC_RRTB_BIT)
#define GR1553B_BM_MC_TSW	(1<<GR1553B_BM_MC_TSW_BIT)
#define GR1553B_BM_MC_TLC	(1<<GR1553B_BM_MC_TLC_BIT)
#define GR1553B_BM_MC_STS	(1<<GR1553B_BM_MC_STS_BIT)
#define GR1553B_BM_MC_STSB	(1<<GR1553B_BM_MC_STSB_BIT)

/* BM RT Mode code filter Register */
#define GR1553B_BM_TTAG_VAL_BIT	0
#define GR1553B_BM_TTAG_RES_BIT	24

#define GR1553B_BM_TTAG_VAL		(0xffffff<<GR1553B_BM_TTAG_VAL_BIT)
#define GR1553B_BM_TTAG_RES		(0xff<<GR1553B_BM_TTAG_RES_BIT)

#define GR1553BC_WRITE_MEM(adr, val) *(volatile uint32_t *)(adr) = (uint32_t)(val)
#define GR1553BC_READ_MEM(adr) (*(volatile uint32_t *)(adr))

#define GR1553BC_WRITE_REG(adr, val) *(volatile uint32_t *)(adr) = (uint32_t)(val)
#define GR1553BC_READ_REG(adr) (*(volatile uint32_t *)(adr))


/* General */

rtems_device_driver grb_initialize(rtems_device_major_number major,
								   rtems_device_minor_number minor,
								   void *arg);
								   
rtems_device_driver grb_open(rtems_device_major_number major,
						     rtems_device_minor_number minor,
						     void *arg);

#ifdef __cplusplus
}
#endif

#endif /* __GR1553B_H__ */
