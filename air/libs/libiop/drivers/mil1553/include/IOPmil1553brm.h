/** @file
 *  @brief Macros used for brm controller
 *
 *  COPYRIGHT (c) 2011\n
 *  GMV-SKYSOFT\n
 *
 *  @author Cl�udio Silva
 *
 * =========================================================\n
 *  COPYRIGHT (c) 2006.
 *  Gaisler Research
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef __MIL1553BRM_H__
#define __MIL1553BRM_H__

#include <iop.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure that maps the BRM core registers 
 */
struct brm_reg {
    volatile unsigned int ctrl;            /**< 0x00 */
    volatile unsigned int oper;            /**< 0x04 */
    volatile unsigned int cur_cmd;         /**< 0x08 */
    volatile unsigned int imask;           /**< 0x0C */
    volatile unsigned int ipend;           /**< 0x10 */
    volatile unsigned int ipoint;          /**< 0x14 */
    volatile unsigned int bit_reg;         /**< 0x18 */
    volatile unsigned int ttag;            /**< 0x1C */
    volatile unsigned int dpoint;          /**< 0x20 */
    volatile unsigned int sw;              /**< 0x24 */
    volatile unsigned int initcount;       /**< 0x28 */
    volatile unsigned int mcpoint;         /**< 0x2C */
    volatile unsigned int mdpoint;         /**< 0x30 */
    volatile unsigned int mbc;             /**< 0x34 */
    volatile unsigned int mfilta;          /**< 0x38 */
    volatile unsigned int mfiltb;          /**< 0x3C */
    volatile unsigned int rt_cmd_leg[16];  /**< 0x40-0x80 */
    volatile unsigned int enhanced;        /**< 0x84 */
    
    volatile unsigned int dummy[31];
    
    volatile unsigned int w_ctrl;          /**< 0x100 */
    volatile unsigned int w_irqctrl;       /**< 0x104 */
    volatile unsigned int w_ahbaddr;       /**< 0x108 */
};

/**
 * @brief Bus Monitor Message
 */
struct bm_msg {
    unsigned short miw; 		/**< Message Information Word */
    unsigned short cw1;			/**< Command Word 1 */
    unsigned short cw2;			/**< Command Word 2 */
    unsigned short sw1;			/**< Status Word 1 */
    unsigned short sw2;			/**< Status Word 2*/
    unsigned short time;		/**< Time */
    unsigned short data[32];	/**< Data */
};

/**
 * @brief Remote Terminal Message
 */
struct rt_msg {
    unsigned short miw;			/**< Message Information Word */
    unsigned short time;		/**< Time */
    unsigned short data[32];	/**< Data */
    unsigned short desc;		/**< Target Descriptor */
};

/**
 * @brief Configuration strcture for the BRM core
 */
typedef struct config {
	unsigned int mode;				/**< Operating mode: BC, BM or RT*/
	unsigned int bus; 				/**< Active buses: A(10) or B(01) or both(11). RT Only*/
	unsigned int rt_address; 		/**< RT address. RT only*/
	unsigned int msg_timeout; 		/**< Message Timeout. BC and BM*/
	unsigned int broadcast;			/**< Broadcast enabled*/
	
	unsigned int rt_buf_number;		/**< Number of data buffers used in RT mode*/
	unsigned int bc_block_number;	/**< Number of command blocks used in BC mode*/
	unsigned int bm_block_number;	/**< Number of command blocks used in BM mode*/
		
	unsigned int event_id;				/**< Error handler task id*/
	int tx_blocking;				/**< TX blocking behavior*/
	int rx_blocking;				/**< RX blocking behavior*/
	unsigned int ignore_mode_data;	/**< Ignore mode data. Focus available memory on sub-addresses*/
	
	unsigned int clkdiv;			/**< Clock Divisor */
	unsigned int clksel;			/**< Clock Selection */
	unsigned int brm_freq;			/**< BRM frequency selection 0=12MHz, 1=16MHz, 2= 20MHz, 3=24MHz*/
} milstd_config_t;

/**
 *  @brief Bus Controller command block interface
 *
 *  rtaddr[0] and subaddr[0] :  RT address and subaddress (for rt-rt receive addresses)
 *  rtaddr[1] and subaddr[1] :  Only for RT-RT. Transmit addresses.
 *
 *  wc : word count, or mode code if subaddress 0 or 31.
 *
 *  ctrl, bit 0 (TR)      : 1 - transmit, 0 - receive. Ignored for rt-rt
 *        bit 1 (RTRT)    : 1 - rt to rt, 0 - normal
 *        bit 2 (AB)      : 1 - Bus B, 0 - Bus A    
 *        bit 4:3 (Retry) : 1 - 1, 2 - 2, 3 - 3, 0 - 4
 *        bit 5 (END)     : End of list 
 *        bit 15 (BAME)   : Message error. Set by BRM if protocol error is detected
 *
 *  tsw[0] : status word
 *  tsw[1] : Only for rt-rt, status word 2
 *
 *  data : data to be transmitted, or received data
 *
 */
struct bc_msg {
    unsigned char  rtaddr[2];   	/**< RT address */
    unsigned char  subaddr[2];		/**< RT subaddress */
    unsigned short wc;				/**< Word Count */
    unsigned short ctrl;			/**< Control Word */
    unsigned short tsw[2];			/**< Status Word */
    unsigned short data[32];		/**< Data */
};

/** @brief Bus Controller Command Block as interpreted by the HW */
struct bc_desc{
	unsigned short ctrl; 	/**<  Control */
	unsigned short cw1;  	/**<  Command word 1*/
	unsigned short cw2;  	/**<  Command word 1*/
	unsigned short dptr; 	/**<  Data pointer in halfword offset from bcmem */
	unsigned short tsw[2];	/**<  status word 1 & 2 */
	unsigned short ba;     	/**<  branch address */
	unsigned short timer;  	/**<  timer value */
}; 

/**
 * @brief MILSTD message
 */
struct msg {
	unsigned short miw; 		/**< Message information word */ 
	unsigned short time;		/**< Time */
	unsigned short data[32];	/**< Data */
};

/**
 * @brief Log List maintained by the driver with relevant events
 */
struct irq_log_list {
    volatile unsigned short iiw;	/**< Interrupt Information Word */
    volatile unsigned short iaw;    /**< Interrupt Address Word */
};

/**
 * @brief driver internal per device structure
 *
 * This Structure contains the map of the core's memory and the current
 * configuration for this device.
 */
typedef struct { 
    unsigned int memarea_base;
    milstd_config_t * config;
    struct brm_reg *regs; /**< BRM core registers */

    /**
	 *  @struct desc_table
	 *	@brief BRM descriptors used to mantain the circular buffer
	 */
    struct desc_table {

        volatile unsigned short ctrl;	/**< Control Word*/     
        volatile unsigned short top;	/**< Pointer to the beggining of the circular buffer*/
        volatile unsigned short cur;	/**< Pointer to current position*/
        volatile unsigned short bot;	/**< Pointer to the last position in the buffer*/

    } *desc;

	/** Pointer to the beggining of the allocated memory for this device */
    volatile unsigned short *mem;
		
		/**
		 * Structure that represents the device's memory when in BC mode 
		 */
		struct {
			/**<  BC command blocks */
			struct cb{
				unsigned short ctrl; 	/**<  Control */
				unsigned short cw1;  	/**<  Command word 1*/
				unsigned short cw2;  	/**<  Command word 1*/
				unsigned short dptr; 	/**<  Data pointer in halfword offset from bcmem */
				unsigned short tsw[2];	/**<  status word 1 & 2 */
				unsigned short ba;     	/**<  branch address */
				unsigned short timer;  	/**<  timer value */
			} *descs; 
		} *bcmem;

	/** 
	 * @brief Memory structure of a RT being inited, just used
	 * during RT initialization.
	 *
	 * *msgs[32] fit each minimally 8 messages per sub address.
	 */
	struct {
		
		/** RX Sub Address descriptors */
		struct desc_table rxsubs[32];
		
		/** TX Sub Address descriptors */
		struct desc_table txsubs[32];
		
		/** RX mode code descriptors */
		struct desc_table rxmodes[32];
		
		/** TX mode code descriptors */
		struct desc_table txmodes[32];
		
	} *rtmem;

    /** Interrupt log list */
    struct irq_log_list *irq_log;   /** Log List that is maintained by the HW with relevant events*/
	
	/** IRQ of this device*/
    unsigned int irq;				/**< IRQ line*/
	
    unsigned int last_read[128];	/** Last Place That was read*/
    unsigned int written[32];		/** Last memory position that was written*/
	
	unsigned int bm_last_read;    	/**< Last command that was read in BM mode*/
	unsigned int bc_last_read;    	/**< Last command that was read in BM mode*/
	unsigned int bm_block_number; 	/**< Number of command blocks in BM mode*/
	unsigned int bc_block_number;	/**< Number of command blocks in BC mode*/
	
    struct bc_desc *cur_list;  		/**< Current list being processed*/
	void *bc_user_list;				/**< user defined bc command list */
	unsigned int bc_user_list_size; /**< User list size*/
	iop_chain_control shortcut[32];		/**< Used as a shortcut to map write requests to their respective command */

    int tx_blocking, rx_blocking;	/**< Write and read directives blocking configuration*/

    unsigned int dev_sem;			/**< Semaphore ID for the device's semaphore*/
	int minor;						/**< Minor number of this device*/
	int irqno;						/**< IRQ line*/
	unsigned int mode;				/**< Current mode*/
		
	unsigned int event_id; 			/**< event that may be signalled upon errors, needs to be set through ioctl command BRM_SET_EVENTID */
	unsigned int status;			/**< Status of the Operation */
	int bc_list_fail;				/**< Did the command block list completed successfully?*/
	
} brm_priv;


/* BC control bits */
#define BC_TR     0x0001
#define BC_RTRT   0x0002
#define BC_BUSA   0x0004
#define BC_EOL    0x0020
#define BC_BAME   0x8000

/**
 * @name IRQ events
 *
 * Events that the core will report in the IRQ log list
 * @{
 */
#define BRM_MBC_IRQ        1                    /**< Monitor Block Counter irq */
#define BRM_CBA_IRQ        2                    /**< Command Block Accessed irq */
#define BRM_RTF_IRQ        4                    /**< Retry Fail irq */
#define BRM_ILLOP_IRQ      8                    /**< Illogical Opcode irq */
#define BRM_BC_ILLCMD_IRQ  16                   /**< BC Illocigal Command irq */
#define BRM_EOL_IRQ        32                   /**< End Of List irq */
#define BRM_RT_ILLCMD_IRQ  128                  /**< RT Illegal Command irq */
#define BRM_IXEQ0_IRQ      256                  /**< Index Equal Zero irq */
#define BRM_BDRCV_IRQ      512                  /**< Broadcast Command Received irq */
#define BRM_SUBAD_IRQ      1024                 /**< Subaddress Accessed irq */
#define BRM_MERR_IRQ       2048                 /**< Message Error irq */
#define BRM_TAPF_IRQ       8192                 /**< Terminal Address Parity Fail irq */
#define BRM_WRAPF_IRQ      16384                /**< Wrap Fail irq */
#define BRM_DMAF_IRQ       32768                /**< DMA Fail irq */
/**@}*/

/**
 * @name IOCTL
 *
 * Macros that define the IOCTL avaliable commands
 * @{
 */
#define BRM_SET_MODE    0
#define BRM_SET_BUS     1
#define BRM_SET_MSGTO   2
#define BRM_SET_RT_ADDR 3
#define BRM_SET_STD     4
#define BRM_SET_BCE     5
#define BRM_TX_BLOCK    7
#define BRM_RX_BLOCK    8

#define BRM_DO_LIST     10
#define BRM_LIST_DONE   11

#define BRM_CLR_STATUS  12
#define BRM_GET_STATUS  13
#define BRM_SET_EVENTID 14
/**@}*/

#define GET_ERROR_DESCRIPTOR(event_in) (event_in>>16)

/**
 * @name BRM Operative Modes
 *
 * BRM core can operate in 4 different modes
 * @{
 */
#define BRM_MODE_BC 0x0		/**< Bus Controler Mode*/
#define BRM_MODE_RT 0x1		/**< Remote terminal Mode*/
#define BRM_MODE_BM 0x2		/**< Bus Monitor Mode*/
#define BRM_MODE_BM_RT 0x3  /**< both RT and BM modes */
/**@}*/

/**
 * @name Frequency Selection
 *
 * Macros for frequency selection among 3 possible choices
 * @{
 */
#define BRM_FREQ_12MHZ 0
#define BRM_FREQ_16MHZ 1
#define BRM_FREQ_20MHZ 2
#define BRM_FREQ_24MHZ 3
#define BRM_FREQ_MASK 0x3
/**@}*/

#define CLKDIV_MASK 0xf

#define CLKSEL_MASK 0x7

/**
 * @name MilStd1553B driver access points
 *
 * Function set used to access the MilStd driver
 * @{
 */
uint32_t brm_initialize(iop_device_driver_t *iop_dev, void *arg);
uint32_t brm_open(brm_priv *brm);
uint32_t brm_close(brm_priv *brm);
uint32_t brm_read(iop_device_driver_t *iop_dev, void *arg);
uint32_t brm_write(iop_device_driver_t *iop_dev, void *arg);
air_status_code_e brm_control(brm_priv *brm, void *arg);

unsigned int brm_get_operative_mode(brm_priv *brm);
air_status_code_e brm_do_list(brm_priv *brm);
air_status_code_e brm_list_done(brm_priv *brm);
int brm_get_number_bc_blocks(brm_priv *brm);


void brm_bc_start_list(brm_priv *brm);

void brm_bc_stop_list(brm_priv *brm);

air_status_code_e brm_bc_init_user_list(brm_priv *brm);

uint32_t brm_bc_process_completed_list(iop_device_driver_t *iop_dev, void *arg);
												
air_status_code_e brm_bc_insert_new_data(brm_priv *bDev, uint8_t *data,
										 milstd_header_t *hdr, unsigned int size);


/**@}*/
#ifdef __cplusplus
}
#endif

#endif /* __BRM_H__ */
