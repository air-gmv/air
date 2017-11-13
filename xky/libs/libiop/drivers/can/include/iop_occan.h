/*  Gaisler wrapper to OpenCores CAN, driver interface
 *
 *  COPYRIGHT (c) 2007.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Author: Daniel Hellstrom, Gaisler Research AB, www.gaisler.com
 */


#ifndef __OCCAN_H__
#define __OCCAN_H__

#include <ambapp.h>

#include <xky.h>
#include <iop.h>
#include <can_support.h>

#ifdef __cplusplus
extern "C" {
#endif

/* default to byte regs */
#ifndef OCCAN_WORD_REGS
 #define OCCAN_BYTE_REGS
#else
 #undef OCCAN_BYTE_REGS
#endif

#ifndef OCCAN_PREFIX
 #define OCCAN_PREFIX(name) occan##name
#endif

#if !defined(OCCAN_DEVNAME) || !defined(OCCAN_DEVNAME_NO)
 #undef OCCAN_DEVNAME
 #undef OCCAN_DEVNAME_NO
 #define OCCAN_DEVNAME "/dev/occan0"
 #define OCCAN_DEVNAME_NO(devstr,no) ((devstr)[10]='0'+(no))
#endif

#ifndef OCCAN_REG_INT
	#define OCCAN_REG_INT(handler,irq,arg) set_vector(handler,irq+0x10)
  #undef  OCCAN_DEFINE_INTHANDLER
  #define OCCAN_DEFINE_INTHANDLER
#endif

/* Default to 40MHz system clock */
/*#ifndef SYS_FREQ_HZ
 #define SYS_FREQ_HZ 40000000
#endif*/

#define OCCAN_WORD_REG_OFS 0x80
#define OCCAN_NCORE_OFS 0x100
#define DEFAULT_CLKDIV 0x7
#define DEFAULT_EXTENDED_MODE 1
//#define DEFAULT_RX_FIFO_LEN 64
//#define DEFAULT_TX_FIFO_LEN 64

/* PELICAN */
#ifdef OCCAN_BYTE_REGS
typedef struct {
	unsigned char
		mode,
		cmd,
		status,
		intflags,
		inten,
		resv0,
		bustim0,
		bustim1,
		unused0[2],
		resv1,
		arbcode,
		errcode,
		errwarn,
		rx_err_cnt,
		tx_err_cnt,
		rx_fi_xff; /* this is also acceptance code 0 in reset mode */
		union{
			struct {
				unsigned char id[2];
				unsigned char data[8];
				unsigned char next_in_fifo[2];
			} rx_sff;
			struct {
				unsigned char id[4];
				unsigned char data[8];
			} rx_eff;
			struct {
				unsigned char id[2];
				unsigned char data[8];
				unsigned char unused[2];
			} tx_sff;
			struct {
				unsigned char id[4];
				unsigned char data[8];
			} tx_eff;
			struct {
				unsigned char code[3];
				unsigned char mask[4];
			} rst_accept;
		} msg;
		unsigned char rx_msg_cnt;
		unsigned char unused1;
		unsigned char clkdiv;
} pelican_regs;
#else
typedef struct {
	unsigned char
		mode, unused0[3],
		cmd, unused1[3],
		status, unused2[3],
		intflags, unused3[3],
		inten, unused4[3],
		resv0, unused5[3],
		bustim0, unused6[3],
		bustim1, unused7[3],
		unused8[8],
		resv1,unused9[3],
		arbcode,unused10[3],
		errcode,unused11[3],
		errwarn,unused12[3],
		rx_err_cnt,unused13[3],
		tx_err_cnt,unused14[3],
		rx_fi_xff, unused15[3]; /* this is also acceptance code 0 in reset mode */
		/* make sure to use pointers when writing (byte access) to these registers */
		union{
			struct {
				unsigned int id[2];
				unsigned int data[8];
				unsigned int next_in_fifo[2];
			} rx_sff;
			struct {
				unsigned int id[4];
				unsigned int data[8];
			} rx_eff;
			struct {
				unsigned int id[2];
				unsigned int data[8];
			} tx_sff;
			struct {
				unsigned int id[4];
				unsigned int data[8];
			} tx_eff;
			struct {
				unsigned int code[3];
				unsigned int mask[4];
			} rst_accept;
		} msg;
		unsigned char rx_msg_cnt,unused16[3];
		unsigned char unused17[4];
		unsigned char clkdiv,unused18[3];
} pelican_regs;
#endif

#define MAX_TSEG2 7
#define MAX_TSEG1 15

#if 0
typedef struct {
	unsigned char brp;
	unsigned char sjw;
	unsigned char tseg1;
	unsigned char tseg2;
	unsigned char sam;
} occan_speed_regs;
#endif
typedef struct {
	unsigned char btr0;
	unsigned char btr1;
} occan_speed_regs;

typedef struct {
	/* tx/rx stats */
	unsigned int rx_msgs;
	unsigned int tx_msgs;

	/* Error Interrupt counters */
	unsigned int err_warn;
	unsigned int err_dovr;
	unsigned int err_errp;
	unsigned int err_arb;
	unsigned int err_bus;

	/**** BUS ERRORS (err_arb) ****/

	/* ALC 4-0 */
	unsigned int err_arb_bitnum[32]; /* At what bit arbitration is lost */

	/******************************/

	/**** BUS ERRORS (err_bus) ****/

	/* ECC 7-6 */
	unsigned int err_bus_bit; /* Bit error */
	unsigned int err_bus_form; /* Form Error */
	unsigned int err_bus_stuff; /* Stuff Error */
	unsigned int err_bus_other; /* Other Error */

	/* ECC 5 */
	unsigned int err_bus_rx; /* Errors during Reception */
	unsigned int err_bus_tx; /* Errors during Transmission */

	/* ECC 4:0 */
	unsigned int err_bus_segs[32]; /* Segment (Where in frame error occured)
	                                * See OCCAN_SEG_* defines for indexes
	                                */

	/******************************/


	/* total number of interrupts */
	unsigned int ints;

	/* software monitoring hw errors */
	unsigned int tx_buf_error;

  /* Software fifo overrun */
  unsigned int rx_sw_dovr;

} occan_stats;

/* TODO See if necessary */
//typedef struct _occan_txrxdesc {
//	int count;
//	uint32_t * addr;
//} occan_txrxdesc;

typedef struct occan_filter_{
	unsigned char code[4];
	unsigned char mask[4];
	int single_mode;
} occan_filter;

typedef struct {
	/* hardware shortcuts */
	pelican_regs *regs;
	int irq;
	occan_speed_regs timing;
	int channel; /* 0=default, 1=second bus */

	/* driver state */
	rtems_id devsem;
	rtems_id txsem;
	rtems_id rxsem;
	int open;
	int started;
	int rxblk;
	int txblk;
	unsigned int status;
	occan_stats stats;
	int sending;

	/* Config */
	unsigned int speed; /* speed in HZ */
//	int single_mode;
//	unsigned char code[4];
//	unsigned char mask[4];
	occan_filter filter;

	occan_fifo	tx_fifo;
	occan_fifo  rx_fifo;

	/* IOP descriptors */
//	occan_txrxdesc txdesc;
//	occan_txrxdesc rxdesc;
	unsigned int tx_ptr;  /*Buffer pointer*/
	unsigned int rx_ptr;
	/* IOP standard buffers */
	iop_buffer_t *iop_buffers;
	uint8_t *iop_buffers_storage;

	iop_buffer_t **tx_iop_buffer;
	iop_buffer_t **rx_iop_buffer;

} occan_priv;

/* indexes into occan_stats.err_bus_segs[index] */
#define OCCAN_SEG_ID28 0x02 /* ID field bit 28:21 */
#define OCCAN_SEG_ID20 0x06 /* ID field bit 20:18 */
#define OCCAN_SEG_ID17 0x07 /* ID field bit 17:13 */
#define OCCAN_SEG_ID12 0x0f /* ID field bit 12:5 */
#define OCCAN_SEG_ID4 0x0e  /* ID field bit 4:0 */

#define OCCAN_SEG_START 0x03 /* Start of Frame */
#define OCCAN_SEG_SRTR 0x04  /* Bit SRTR */
#define OCCAN_SEG_IDE 0x05   /* Bit IDE */
#define OCCAN_SEG_RTR 0x0c   /* Bit RTR */
#define OCCAN_SEG_RSV0 0x09  /* Reserved bit 0 */
#define OCCAN_SEG_RSV1 0x0d  /* Reserved bit 1 */

#define OCCAN_SEG_DLEN 0x0b    /* Data Length code */
#define OCCAN_SEG_DFIELD 0x0a  /* Data Field */

#define OCCAN_SEG_CRC_SEQ 0x08    /* CRC Sequence */
#define OCCAN_SEG_CRC_DELIM 0x18  /* CRC Delimiter */

#define OCCAN_SEG_ACK_SLOT 0x19   /* Acknowledge slot */
#define OCCAN_SEG_ACK_DELIM 0x1b  /* Acknowledge delimiter */
#define OCCAN_SEG_EOF 0x1a        /* End Of Frame */
#define OCCAN_SEG_INTERMISSION 0x12 /* Intermission */
#define OCCAN_SEG_ACT_ERR 0x11    /* Active error flag */
#define OCCAN_SEG_PASS_ERR 0x16   /* Passive error flag */
#define OCCAN_SEG_DOMINANT 0x13   /* Tolerate dominant bits */
#define OCCAN_SEG_EDELIM 0x17     /* Error delimiter */
#define OCCAN_SEG_OVERLOAD 0x1c   /* overload flag */


#define CANMSG_OPT_RTR 0x40 			/* RTR Frame */
#define CANMSG_OPT_EXTENDED 0x80  /* Exteneded frame */
#define CANMSG_OPT_SSHOT 0x01     /* Single Shot, no retry */

#define OCCAN_IOC_START 1
#define OCCAN_IOC_STOP  2

#define OCCAN_IOC_GET_CONF 3
#define OCCAN_IOC_GET_STATS 4
#define OCCAN_IOC_GET_STATUS 5

#define OCCAN_IOC_SET_SPEED 6
#define OCCAN_IOC_SPEED_AUTO 7
#define OCCAN_IOC_SET_LINK 8
#define OCCAN_IOC_SET_FILTER 9
#define OCCAN_IOC_SET_BLK_MODE 10
#define OCCAN_IOC_SET_BUFLEN 11
#define OCCAN_IOC_SET_BTRS 12

#define OCCAN_STATUS_RESET 0x01
#define OCCAN_STATUS_OVERRUN 0x02
#define OCCAN_STATUS_WARN 0x04
#define OCCAN_STATUS_ERR_PASSIVE 0x08
#define OCCAN_STATUS_ERR_BUSOFF 0x10
#define OCCAN_STATUS_QUEUE_ERROR 0x80

#define OCCAN_BLK_MODE_RX 0x1
#define OCCAN_BLK_MODE_TX 0x2

int occan_register(amba_confarea_type *bus);

// TODO change this to an enum
#define OCCAN_SPEED_500K 500000
#define OCCAN_SPEED_250K 250000
#define OCCAN_SPEED_125K 125000
#define OCCAN_SPEED_75K  75000
#define OCCAN_SPEED_50K  50000
#define OCCAN_SPEED_25K  25000
#define OCCAN_SPEED_10K  10000
/**** Hardware related Interface ****/
static int occan_calc_speedregs(unsigned int clock_hz, unsigned int rate, occan_speed_regs *result);
static int occan_set_speedregs(occan_priv *priv, occan_speed_regs *timing);
static int pelican_speed_auto(occan_priv *priv);
static void pelican_init(occan_priv *priv);
static void pelican_open(occan_priv *priv);
static int pelican_start(occan_priv *priv);
static void pelican_stop(occan_priv *priv);
static int pelican_send(occan_priv *can, CANMsg *msg);
static void pelican_set_accept(occan_priv *priv, unsigned char *acode, unsigned char *amask);
static void occan_interrupt(occan_priv *can);
#ifdef DEBUG_PRINT_REGMAP
static void pelican_regadr_print(pelican_regs *regs);
#endif

/***** Driver related interface *****/
//uint32_t occan_ioctl(iop_can_device_t *device, void *arg);
uint32_t occan_write(iop_device_driver_t *iop_dev, void *arg);
uint32_t occan_read(iop_device_driver_t *iop_dev, void *arg);
uint32_t occan_close(iop_device_driver_t *iop_dev, void *arg);
uint32_t occan_open(iop_device_driver_t *iop_dev, void *arg);
uint32_t occan_initialize(iop_device_driver_t *iop_dev, void *arg);
#ifdef OCCAN_DEFINE_INTHANDLER
static void occan_interrupt_handler(rtems_vector_number v);
#endif
static int can_cores;
static amba_confarea_type *amba_bus;
static unsigned int sys_freq_hz;


/**
 * @brief Paramameter block for read/write.
 *
 * It must include 'offset' instead of using iop's offset since we can have
 * multiple outstanding i/o's on a device.
 */
//typedef struct {
//  char                   *buffer;
//  uint32_t                count;
//  uint32_t                flags;
//  uint32_t                bytes_moved;
//} rtems_libio_rw_args_t;

/**
 * @brief Paramameter block for read/write.
 *
 * It must include 'offset' instead of using iop's offset since we can have
 * multiple outstanding i/o's on a device.
 */
///**
// * @brief Parameter block for open/close.
// */
//typedef struct {
//  uint32_t                flags;
//  uint32_t                mode;
//} rtems_libio_open_close_args_t;
//
///**
// * @brief Parameter block for ioctl.
// */
//typedef struct {
//  uint32_t                command;
//  void                   *buffer;
//  uint32_t                ioctl_return;
//} rtems_libio_ioctl_args_t;

#ifdef __cplusplus
}
#endif

#endif //__OCCAN_H__
