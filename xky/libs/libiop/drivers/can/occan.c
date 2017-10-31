/*  OC_CAN driver
 *
 *  COPYRIGHT (c) 2007.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Author: Daniel Hellstr��m, Gaisler Research AB, www.gaisler.com
 *
 *  Modified: Guilherme Sanches (gmvs@gmv.com)
 *  Modifications introduced to integrate the driver on AIR
 */

//#include <rtems/libio.h>
#include <string.h>
#include <bsp.h>

#include <leon.h>
#include <ambapp.h>
#include <occan.h>
#include <can_support.h>
#include <occan_msg_queue.h>

/* RTEMS -> ERRNO decoding table

rtems_assoc_t errno_assoc[] = {
    { "OK",                 RTEMS_SUCCESSFUL,                0 },
    { "BUSY",               RTEMS_RESOURCE_IN_USE,           EBUSY },
    { "INVALID NAME",       RTEMS_INVALID_NAME,              EINVAL },
    { "NOT IMPLEMENTED",    RTEMS_NOT_IMPLEMENTED,           ENOSYS },
    { "TIMEOUT",            RTEMS_TIMEOUT,                   ETIMEDOUT },
    { "NO MEMORY",          RTEMS_NO_MEMORY,                 ENOMEM },
    { "NO DEVICE",          RTEMS_UNSATISFIED,               ENODEV },
    { "INVALID NUMBER",     RTEMS_INVALID_NUMBER,            EBADF},
    { "NOT RESOURCE OWNER", RTEMS_NOT_OWNER_OF_RESOURCE,     EPERM},
    { "IO ERROR",           RTEMS_IO_ERROR,                  EIO},
    { 0, 0, 0 },
};
*/

/*
#undef DEBUG
#undef DEBUG_EXTRA
#undef DEBUG_PRINT_REGMAP
*/

/* not implemented yet */
#undef REDUNDANT_CHANNELS

/* Define common debug macros */
//#ifdef DEBUG
//	#define iop_debug(fmt, vargs...) iop_debug(fmt, ## vargs )
//#else
//	#define iop_debug(fmt, vargs...)
//#endif

/* Read byte bypassing */

#ifdef OCCAN_DONT_BYPASS_CACHE
 #define READ_REG(address) (*(volatile unsigned char *)(address))
#else
 /* Bypass cache */
 #define READ_REG(address) _OCCAN_REG_READ((unsigned int)(address))
 static __inline__ unsigned char _OCCAN_REG_READ(unsigned int addr) {
        unsigned char tmp;
        asm(" lduba [%1]1, %0 "
            : "=r"(tmp)
            : "r"(addr)
           );
        return tmp;
	}
#endif

#define WRITE_REG(address,data) (*(volatile unsigned char *)(address) = (data))

/* Mode register bit definitions */
#define PELICAN_MOD_RESET          0x1
#define PELICAN_MOD_LISTEN         0x2
#define PELICAN_MOD_SELFTEST       0x4
#define PELICAN_MOD_ACCEPT         0x8

/* Command register bit definitions */
#define PELICAN_CMD_TXREQ          0x1
#define PELICAN_CMD_ABORT          0x2
#define PELICAN_CMD_RELRXBUF       0x4
#define PELICAN_CMD_CLRDOVR        0x8
#define PELICAN_CMD_SELFRXRQ       0x10

/* Status register bit definitions */
#define PELICAN_STAT_RXBUF         0x1
#define PELICAN_STAT_DOVR          0x2
#define PELICAN_STAT_TXBUF         0x4
#define PELICAN_STAT_TXOK          0x8
#define PELICAN_STAT_RX            0x10
#define PELICAN_STAT_TX            0x20
#define PELICAN_STAT_ERR           0x40
#define PELICAN_STAT_BUS           0x80

/* Interrupt register bit definitions */
#define PELICAN_IF_RX         0x1
#define PELICAN_IF_TX         0x2
#define PELICAN_IF_ERRW       0x4
#define PELICAN_IF_DOVR       0x8
#define PELICAN_IF_ERRP       0x20
#define PELICAN_IF_ARB        0x40
#define PELICAN_IF_BUS        0x80

/* Interrupt Enable register bit definitions */
#define PELICAN_IE_RX         0x1
#define PELICAN_IE_TX         0x2
#define PELICAN_IE_ERRW       0x4
#define PELICAN_IE_DOVR       0x8
#define PELICAN_IE_ERRP       0x20
#define PELICAN_IE_ARB        0x40
#define PELICAN_IE_BUS        0x80

/* Arbitration lost capture register bit definitions */
#define PELICAN_ARB_BITS      0x1f

/*  register bit definitions */
#define PELICAN_ECC_CODE_BIT     0x00
#define PELICAN_ECC_CODE_FORM    0x40
#define PELICAN_ECC_CODE_STUFF   0x80
#define PELICAN_ECC_CODE_OTHER   0xc0
#define PELICAN_ECC_CODE    0xc0

#define PELICAN_ECC_DIR     0x20
#define PELICAN_ECC_SEG     0x1f

/* Clock divider register bit definitions */
#define PELICAN_CDR_DIV  0x7
#define PELICAN_CDR_OFF  0x8
#define PELICAN_CDR_MODE 0x80
#define PELICAN_CDR_MODE_PELICAN 0x80
#define PELICAN_CDR_MODE_BITS 7
#define PELICAN_CDR_MODE_BASICAN 0x00


/*  register bit definitions */
#define OCCAN_BUSTIM_SJW       0xc0
#define OCCAN_BUSTIM_BRP       0x3f
#define OCCAN_BUSTIM_SJW_BIT   6

#define OCCAN_BUSTIM_SAM       0x80
#define OCCAN_BUSTIM_TSEG2     0x70
#define OCCAN_BUSTIM_TSEG2_BIT 4
#define OCCAN_BUSTIM_TSEG1     0x0f

/*  register bit definitions */
/*
#define PELICAN_S_       0x1
#define PELICAN_S_       0x2
#define PELICAN_S_       0x4
#define PELICAN_S_       0x8
#define PELICAN_S_       0x10
#define PELICAN_S_       0x20
#define PELICAN_S_       0x40
#define PELICAN_S_       0x80
*/

static void pelican_init(occan_priv *priv){
	/* Reset core */
	priv->regs->mode = PELICAN_MOD_RESET;

	/* wait for core to reset complete */
	/*usleep(1);*/
	rtems_task_wake_after(1);
}

static void pelican_open(occan_priv *priv){
	unsigned char tmp;
	int ret;

	/* Set defaults */
	priv->speed = OCCAN_SPEED_250K;

	/* set acceptance filters to accept all messages */
	priv->filter->code[0] = 0;
	priv->filter->code[1] = 0;
	priv->filter->code[2] = 0;
	priv->filter->code[3] = 0;
	priv->filter->mask[0] = 0xff;
	priv->filter->mask[1] = 0xff;
	priv->filter->mask[2] = 0xff;
	priv->filter->mask[3] = 0xff;

	/* Set clock divider to extended mode, clkdiv not connected
	 */
	priv->regs->clkdiv = (1<<PELICAN_CDR_MODE_BITS) | (DEFAULT_CLKDIV & PELICAN_CDR_DIV);

	ret = occan_calc_speedregs(sys_freq_hz,priv->speed,&priv->timing);
	if ( ret ){
		/* failed to set speed for this system freq, try with 50K instead */
		priv->speed = OCCAN_SPEED_50K;
		occan_calc_speedregs(sys_freq_hz,priv->speed,&priv->timing);
	}

	/* disable all interrupts */
	priv->regs->inten = 0;

	/* clear pending interrupts by reading */
	tmp = READ_REG(&priv->regs->intflags);
}

static int pelican_start(occan_priv *priv){
	unsigned char tmp;
	/* Start HW communication */

	if ( !priv->tx_msg_queue || !priv->rx_msg_queue)
		return -1;

  /* In case we were started before and stopped we
   * should empty the TX fifo or try to resend those
   * messages. We make it simple...
   */
	// TODO Clean buffers
//  occan_fifo_clr(priv->txfifo);
	occan_fifo_clear(priv->tx_msg_queue);

	/* Clear status bits */
	priv->status = 0;
	priv->sending = 0;

	/* clear pending interrupts */
	tmp = READ_REG(&priv->regs->intflags);

	/* clear error counters */
//	priv->regs->rx_err_cnt = 0;
//	priv->regs->tx_err_cnt = 0;
	WRITE_REG( &priv->regs->rx_err_cnt, 0);
	WRITE_REG( &priv->regs->tx_err_cnt, 0);

#ifdef REDUNDANT_CHANNELS
	if ( (priv->channel == 0) || (priv->channel >= REDUNDANT_CHANNELS) ){
		/* Select the first (default) channel */
		OCCAN_SET_CHANNEL(priv,0);
	}else{
		/* set gpio bit, or something */
		OCCAN_SET_CHANNEL(priv,priv->channel);
	}
#endif
	/* set the speed regs of the CAN core */
	occan_set_speedregs(priv,&priv->timing);

	iop_debug("OCCAN: start: set timing regs btr0: 0x%x, btr1: 0x%x\n\r",
			READ_REG(&priv->regs->bustim0),
			READ_REG(&priv->regs->bustim1));

	/* Set default acceptance filter */
	pelican_set_accept(priv,priv->filter->code,priv->filter->mask);

	/* turn on interrupts */
//	priv->regs->inten = PELICAN_IE_RX | PELICAN_IE_TX | PELICAN_IE_ERRW |
//	                    PELICAN_IE_ERRP | PELICAN_IE_BUS;
	WRITE_REG(priv->regs->inten, PELICAN_IE_RX | PELICAN_IE_TX |
			PELICAN_IE_ERRW | PELICAN_IE_ERRP | PELICAN_IE_BUS);

#ifdef DEBUG
	/* print setup before starting */
	pelican_regs_print(priv->regs);
	occan_stat_print(&priv->stats);
#endif

	/* core already in reset mode,
	 *  Exit reset mode
	 *  Enter Single/Dual mode filtering.
	 */
//	priv->regs->mode =  (priv->single_mode << 3);
	WRITE_REG(priv->regs->mode, (priv->filter->single_mode << 3));

	return 0;
}

static void pelican_stop(occan_priv *priv){
	/* stop HW */

#ifdef DEBUG
	/* print setup before stopping */
	pelican_regs_print(priv->regs);
	occan_stat_print(&priv->stats);
#endif

	/* put core in reset mode */
//	priv->regs->mode = PELICAN_MOD_RESET;
	WRITE_REG(priv->regs->mode, PELICAN_MOD_RESET);

	/* turn off interrupts */
//	priv->regs->inten = 0;
	WRITE_REG(priv->regs->inten, 0);

//	priv->status |= OCCAN_STATUS_RESET;
	WRITE_REG(priv->status, OCCAN_STATUS_RESET);
}


/* Try to send message "msg", if hardware txfifo is
 * full, then -1 is returned.
 *
 * Be sure to have disabled CAN interrupts when
 * entering this function.
 */
static int pelican_send(occan_priv *can, CANMsg *msg){
	unsigned char tmp,status;
	pelican_regs *regs = can->regs;

	/* is there room in send buffer? */
	status = READ_REG(&regs->status);
	if ( !(status & PELICAN_STAT_TXBUF) ){
		/* tx fifo taken, we have to wait */
		return -1;
	}

	tmp = msg->len & 0xf;
	if ( msg->rtr )
		tmp |= 0x40;

	if ( msg->extended ){
		/* Extended Frame */
		regs->rx_fi_xff = 0x80 | tmp;
		WRITE_REG(&regs->msg.tx_eff.id[0],(msg->id >> (5+8+8)) & 0xff);
		WRITE_REG(&regs->msg.tx_eff.id[1],(msg->id >> (5+8)) & 0xff);
		WRITE_REG(&regs->msg.tx_eff.id[2],(msg->id >> (5)) & 0xff);
		WRITE_REG(&regs->msg.tx_eff.id[3],(msg->id << 3) & 0xf8);
		tmp = msg->len;
		while(tmp--){
			WRITE_REG(&regs->msg.tx_eff.data[tmp],msg->data[tmp]);
		}
	}else{
		/* Standard Frame */
		regs->rx_fi_xff = tmp;
		WRITE_REG(&regs->msg.tx_sff.id[0],(msg->id >> 3) & 0xff);
		WRITE_REG(&regs->msg.tx_sff.id[1],(msg->id << 5) & 0xe0);
		tmp = msg->len;
		while(tmp--){
			WRITE_REG(&regs->msg.tx_sff.data[tmp],msg->data[tmp]);
		}
	}

	/* let HW know of new message */
	if ( msg->sshot ){
		regs->cmd = PELICAN_CMD_TXREQ | PELICAN_CMD_ABORT;
	}else{
		/* normal case -- try resend until sent */
		regs->cmd = PELICAN_CMD_TXREQ;
	}

	return 0;
}


static void pelican_set_accept(occan_priv *priv, unsigned char *acode, unsigned char *amask){
	unsigned char *acode0, *acode1, *acode2, *acode3;
	unsigned char *amask0, *amask1, *amask2, *amask3;

	acode0 = &priv->regs->rx_fi_xff;
	acode1 = (unsigned char *)&priv->regs->msg.rst_accept.code[0];
	acode2 = (unsigned char *)&priv->regs->msg.rst_accept.code[1];
	acode3 = (unsigned char *)&priv->regs->msg.rst_accept.code[2];

	amask0 = (unsigned char *)&priv->regs->msg.rst_accept.mask[0];
	amask1 = (unsigned char *)&priv->regs->msg.rst_accept.mask[1];
	amask2 = (unsigned char *)&priv->regs->msg.rst_accept.mask[2];
	amask3 = (unsigned char *)&priv->regs->msg.rst_accept.mask[3];

	/* Set new mask & code */
	*acode0 = acode[0];
	*acode1 = acode[1];
	*acode2 = acode[2];
	*acode3 = acode[3];

	*amask0 = amask[0];
	*amask1 = amask[1];
	*amask2 = amask[2];
	*amask3 = amask[3];
}

#ifdef DEBUG
static void pelican_regs_print(pelican_regs *regs){
	iop_debug("--- PELICAN 0x%lx ---\n\r",(unsigned int)regs);
	iop_debug(" MODE: 0x%02x\n\r",READ_REG(&regs->mode));
	iop_debug(" CMD: 0x%02x\n\r",READ_REG(&regs->cmd));
	iop_debug(" STATUS: 0x%02x\n\r",READ_REG(&regs->status));
	/*iop_debug(" INTFLG: 0x%02x\n\r",READ_REG(&regs->intflags));*/
	iop_debug(" INTEN: 0x%02x\n\r",READ_REG(&regs->inten));
	iop_debug(" BTR0: 0x%02x\n\r",READ_REG(&regs->bustim0));
	iop_debug(" BTR1: 0x%02x\n\r",READ_REG(&regs->bustim1));
	iop_debug(" ARBCODE: 0x%02x\n\r",READ_REG(&regs->arbcode));
	iop_debug(" ERRCODE: 0x%02x\n\r",READ_REG(&regs->errcode));
	iop_debug(" ERRWARN: 0x%02x\n\r",READ_REG(&regs->errwarn));
	iop_debug(" RX_ERR_CNT: 0x%02x\n\r",READ_REG(&regs->rx_err_cnt));
	iop_debug(" TX_ERR_CNT: 0x%02x\n\r",READ_REG(&regs->tx_err_cnt));
	if ( READ_REG(&regs->mode) & PELICAN_MOD_RESET ){
		/* in reset mode it is possible to read acceptance filters */
		iop_debug(" ACR0: 0x%02x (0x%lx)\n\r",READ_REG(&regs->rx_fi_xff),&regs->rx_fi_xff);
		iop_debug(" ACR1: 0x%02x (0x%lx)\n\r",READ_REG(&regs->msg.rst_accept.code[0]),(unsigned int)&regs->msg.rst_accept.code[0]);
		iop_debug(" ACR1: 0x%02x (0x%lx)\n\r",READ_REG(&regs->msg.rst_accept.code[1]),(unsigned int)&regs->msg.rst_accept.code[1]);
		iop_debug(" ACR1: 0x%02x (0x%lx)\n\r",READ_REG(&regs->msg.rst_accept.code[2]),(unsigned int)&regs->msg.rst_accept.code[2]);
		iop_debug(" AMR0: 0x%02x (0x%lx)\n\r",READ_REG(&regs->msg.rst_accept.mask[0]),(unsigned int)&regs->msg.rst_accept.mask[0]);
		iop_debug(" AMR1: 0x%02x (0x%lx)\n\r",READ_REG(&regs->msg.rst_accept.mask[1]),(unsigned int)&regs->msg.rst_accept.mask[1]);
		iop_debug(" AMR2: 0x%02x (0x%lx)\n\r",READ_REG(&regs->msg.rst_accept.mask[2]),(unsigned int)&regs->msg.rst_accept.mask[2]);
		iop_debug(" AMR3: 0x%02x (0x%lx)\n\r",READ_REG(&regs->msg.rst_accept.mask[3]),(unsigned int)&regs->msg.rst_accept.mask[3]);

	}else{
		iop_debug(" RXFI_XFF: 0x%02x\n\r",READ_REG(&regs->rx_fi_xff));
	}
	iop_debug(" RX_MSG_CNT: 0x%02x\n\r",READ_REG(&regs->rx_msg_cnt));
	iop_debug(" CLKDIV: 0x%02x\n\r",READ_REG(&regs->clkdiv));
	iop_debug("-------------------\n\r");
}
#endif

#ifdef DEBUG_PRINT_REGMAP
static void pelican_regadr_print(pelican_regs *regs){
	iop_debug("--- PELICAN 0x%lx ---\n\r",(unsigned int)regs);
	iop_debug(" MODE: 0x%lx\n\r",(unsigned int)&regs->mode);
	iop_debug(" CMD: 0x%lx\n\r",(unsigned int)&regs->cmd);
	iop_debug(" STATUS: 0x%lx\n\r",(unsigned int)&regs->status);
	/*iop_debug(" INTFLG: 0x%lx\n\r",&regs->intflags);*/
	iop_debug(" INTEN: 0x%lx\n\r",(unsigned int)&regs->inten);
	iop_debug(" BTR0: 0x%lx\n\r",(unsigned int)&regs->bustim0);
	iop_debug(" BTR1: 0x%lx\n\r",(unsigned int)&regs->bustim1);
	iop_debug(" ARBCODE: 0x%lx\n\r",(unsigned int)&regs->arbcode);
	iop_debug(" ERRCODE: 0x%lx\n\r",(unsigned int)&regs->errcode);
	iop_debug(" ERRWARN: 0x%lx\n\r",(unsigned int)&regs->errwarn);
	iop_debug(" RX_ERR_CNT: 0x%lx\n\r",(unsigned int)&regs->rx_err_cnt);
	iop_debug(" TX_ERR_CNT: 0x%lx\n\r",(unsigned int)&regs->tx_err_cnt);

	/* in reset mode it is possible to read acceptance filters */
	iop_debug(" RXFI_XFF: 0x%lx\n\r",(unsigned int)&regs->rx_fi_xff);

	/* reset registers */
	iop_debug(" ACR0: 0x%lx\n\r",(unsigned int)&regs->rx_fi_xff);
	iop_debug(" ACR1: 0x%lx\n\r",(unsigned int)&regs->msg.rst_accept.code[0]);
	iop_debug(" ACR2: 0x%lx\n\r",(unsigned int)&regs->msg.rst_accept.code[1]);
	iop_debug(" ACR3: 0x%lx\n\r",(unsigned int)&regs->msg.rst_accept.code[2]);
	iop_debug(" AMR0: 0x%lx\n\r",(unsigned int)&regs->msg.rst_accept.mask[0]);
	iop_debug(" AMR1: 0x%lx\n\r",(unsigned int)&regs->msg.rst_accept.mask[1]);
	iop_debug(" AMR2: 0x%lx\n\r",(unsigned int)&regs->msg.rst_accept.mask[2]);
	iop_debug(" AMR3: 0x%lx\n\r",(unsigned int)&regs->msg.rst_accept.mask[3]);

	/* TX Extended */
	iop_debug(" EFFTX_ID[0]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.id[0]);
	iop_debug(" EFFTX_ID[1]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.id[1]);
	iop_debug(" EFFTX_ID[2]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.id[2]);
	iop_debug(" EFFTX_ID[3]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.id[3]);

	iop_debug(" EFFTX_DATA[0]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.data[0]);
	iop_debug(" EFFTX_DATA[1]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.data[1]);
	iop_debug(" EFFTX_DATA[2]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.data[2]);
	iop_debug(" EFFTX_DATA[3]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.data[3]);
	iop_debug(" EFFTX_DATA[4]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.data[4]);
	iop_debug(" EFFTX_DATA[5]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.data[5]);
	iop_debug(" EFFTX_DATA[6]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.data[6]);
	iop_debug(" EFFTX_DATA[7]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_eff.data[7]);

	/* RX Extended */
	iop_debug(" EFFRX_ID[0]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.id[0]);
	iop_debug(" EFFRX_ID[1]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.id[1]);
	iop_debug(" EFFRX_ID[2]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.id[2]);
	iop_debug(" EFFRX_ID[3]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.id[3]);

	iop_debug(" EFFRX_DATA[0]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.data[0]);
	iop_debug(" EFFRX_DATA[1]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.data[1]);
	iop_debug(" EFFRX_DATA[2]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.data[2]);
	iop_debug(" EFFRX_DATA[3]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.data[3]);
	iop_debug(" EFFRX_DATA[4]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.data[4]);
	iop_debug(" EFFRX_DATA[5]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.data[5]);
	iop_debug(" EFFRX_DATA[6]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.data[6]);
	iop_debug(" EFFRX_DATA[7]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_eff.data[7]);


	/* RX Extended */
	iop_debug(" SFFRX_ID[0]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.id[0]);
	iop_debug(" SFFRX_ID[1]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.id[1]);

	iop_debug(" SFFRX_DATA[0]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.data[0]);
	iop_debug(" SFFRX_DATA[1]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.data[1]);
	iop_debug(" SFFRX_DATA[2]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.data[2]);
	iop_debug(" SFFRX_DATA[3]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.data[3]);
	iop_debug(" SFFRX_DATA[4]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.data[4]);
	iop_debug(" SFFRX_DATA[5]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.data[5]);
	iop_debug(" SFFRX_DATA[6]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.data[6]);
	iop_debug(" SFFRX_DATA[7]: 0x%lx\n\r",(unsigned int)&regs->msg.rx_sff.data[7]);

	/* TX Extended */
	iop_debug(" SFFTX_ID[0]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.id[0]);
	iop_debug(" SFFTX_ID[1]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.id[1]);

	iop_debug(" SFFTX_DATA[0]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.data[0]);
	iop_debug(" SFFTX_DATA[1]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.data[1]);
	iop_debug(" SFFTX_DATA[2]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.data[2]);
	iop_debug(" SFFTX_DATA[3]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.data[3]);
	iop_debug(" SFFTX_DATA[4]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.data[4]);
	iop_debug(" SFFTX_DATA[5]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.data[5]);
	iop_debug(" SFFTX_DATA[6]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.data[6]);
	iop_debug(" SFFTX_DATA[7]: 0x%lx\n\r",(unsigned int)&regs->msg.tx_sff.data[7]);

	iop_debug(" RX_MSG_CNT: 0x%lx\n\r",(unsigned int)&regs->rx_msg_cnt);
	iop_debug(" CLKDIV: 0x%lx\n\r",(unsigned int)&regs->clkdiv);
	iop_debug("-------------------\n\r");
}
#endif

#ifdef DEBUG
static void occan_stat_print(occan_stats *stats){
	iop_debug("----Stats----\n\r");
	iop_debug("rx_msgs: %d\n\r",stats->rx_msgs);
	iop_debug("tx_msgs: %d\n\r",stats->tx_msgs);
	iop_debug("err_warn: %d\n\r",stats->err_warn);
	iop_debug("err_dovr: %d\n\r",stats->err_dovr);
	iop_debug("err_errp: %d\n\r",stats->err_errp);
	iop_debug("err_arb: %d\n\r",stats->err_arb);
	iop_debug("err_bus: %d\n\r",stats->err_bus);
	iop_debug("Int cnt: %d\n\r",stats->ints);
	iop_debug("tx_buf_err: %d\n\r",stats->tx_buf_error);
	iop_debug("-------------\n\r");
}
#endif

/* This function calculates BTR0 and BTR1 values for a given bitrate.
 *
 * Set communication parameters.
 * \param clock_hz OC_CAN Core frequency in Hz.
 * \param rate Requested baud rate in bits/second.
 * \param result Pointer to where resulting BTRs will be stored.
 * \return zero if successful to calculate a baud rate.
 */
static int occan_calc_speedregs(unsigned int clock_hz, unsigned int rate, occan_speed_regs *result){
	int best_error = 1000000000;
	int error;
	int best_tseg=0, best_brp=0, best_rate=0, brp=0;
	int tseg=0, tseg1=0, tseg2=0;
	int sjw = 0;
	int clock = clock_hz / 2;
	int sampl_pt = 90;

	if ( (rate<5000) || (rate>1000000) ){
		/* invalid speed mode */
		return -1;
	}

	/* find best match, return -2 if no good reg
	 * combination is available for this frequency */

	/* some heuristic specials */
	if (rate > ((1000000 + 500000) / 2))
		sampl_pt = 75;

	if (rate < ((12500 + 10000) / 2))
		sampl_pt = 75;

	if (rate < ((100000 + 125000) / 2))
		sjw = 1;

	/* tseg even = round down, odd = round up */
	for (tseg = (0 + 0 + 2) * 2;
	     tseg <= (MAX_TSEG2 + MAX_TSEG1 + 2) * 2 + 1;
	     tseg++)
	{
		brp = clock / ((1 + tseg / 2) * rate) + tseg % 2;
		if ((brp == 0) || (brp > 64))
			continue;

		error = rate - clock / (brp * (1 + tseg / 2));
		if (error < 0)
		{
			error = -error;
		}

		if (error <= best_error)
		{
			best_error = error;
			best_tseg = tseg/2;
			best_brp = brp-1;
			best_rate = clock/(brp*(1+tseg/2));
		}
	}

	if (best_error && (rate / best_error < 10))
	{
		iop_debug("OCCAN: bitrate %d is not possible with %d Hz clock\n\r",rate, clock);
		return -2;
	}else if ( !result )
		return 0; /* nothing to store result in, but a valid bitrate can be calculated */

	tseg2 = best_tseg - (sampl_pt * (best_tseg + 1)) / 100;

	if (tseg2 < 0)
	{
		tseg2 = 0;
	}

	if (tseg2 > MAX_TSEG2)
	{
		tseg2 = MAX_TSEG2;
	}

	tseg1 = best_tseg - tseg2 - 2;

	if (tseg1 > MAX_TSEG1)
	{
		tseg1 = MAX_TSEG1;
		tseg2 = best_tseg - tseg1 - 2;
	}

	result->btr0 = (sjw<<OCCAN_BUSTIM_SJW_BIT) | (best_brp&OCCAN_BUSTIM_BRP);
	result->btr1 = (0<<7) | (tseg2<<OCCAN_BUSTIM_TSEG2_BIT) | tseg1;

	return 0;
}

static int occan_set_speedregs(occan_priv *priv, occan_speed_regs *timing){
	if ( !timing || !priv || !priv->regs)
		return -1;

	priv->regs->bustim0 = timing->btr0;
	priv->regs->bustim1 = timing->btr1;
	return 0;
}

#if 0
static unsigned int pelican_speed_auto_steplist [] = {
	OCCAN_SPEED_500K,
	OCCAN_SPEED_250K,
	OCCAN_SPEED_125K,
	OCCAN_SPEED_75K,
	OCCAN_SPEED_50K,
	OCCAN_SPEED_25K,
	OCCAN_SPEED_10K,
	0
};
#endif

static int pelican_speed_auto(occan_priv *priv){
	return -1;

#if 0
	int i=0;
	occan_speed_regs timing;
	unsigned int speed;
	unsigned char tmp;
	while ( (speed=pelican_speed_auto_steplist[i]) > 0){

		/* Reset core */
		priv->regs->mode = PELICAN_MOD_RESET;

		/* tell int handler about the auto speed detection test */


		/* wait for a moment (10ms) */
		/*usleep(10000);*/

		/* No acceptance filter */
		pelican_set_accept(priv);

		/* calc timing params for this */
		if ( occan_calc_speedregs(sys_freq_hz,speed,&timing) ){
			/* failed to get good timings for this frequency
			 * test with next
			 */
			 continue;
		}

		timing.sam = 0;

		/* set timing params for this speed */
		occan_set_speedregs(priv,&timing);

		/* Empty previous messages in hardware RX fifo */
		/*
		while( READ_REG(&priv->regs->) ){

		}
		*/

		/* Clear pending interrupts */
		tmp = READ_REG(&priv->regs->intflags);

		/* enable RX & ERR interrupt */
		priv->regs->inten =

		/* Get out of reset state */
		priv->regs->mode = PELICAN_MOD_LISTEN;

		/* wait for frames or errors */
		while(1){
			/* sleep 10ms */

		}

	}
#endif
}


uint32_t occan_initialize(iop_device_driver_t *iop_dev, void *arg){
	int dev_cnt;
	int subcore_cnt,devi,subi,subcores;
	amba_ahb_device ambadev;
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	occan_priv *can = device->dev.driver;
	char fs_name[20];
	rtems_status_code status;

//	strcpy(fs_name,OCCAN_DEVNAME);

//	/* find device on amba bus */
//	dev_cnt = amba_get_number_ahbslv_devices(amba_bus,VENDOR_GAISLER,GAISLER_OCCAN);
//	if ( dev_cnt < 1 ){
//		/* Failed to find any CAN cores! */
//		iop_debug("OCCAN: Failed to find any CAN cores\n\r");
//		return -1;
//	}

	/* Detect System Frequency from initialized timer */
#ifndef SYS_FREQ_HZ
#if defined(LEON3)
	/* LEON3: find timer address via AMBA Plug&Play info */
	{
		amba_apb_device gptimer;
		LEON3_Timer_Regs_Map *tregs;

		if ( amba_find_apbslv(&amba_conf,VENDOR_GAISLER,GAISLER_GPTIMER,&gptimer) == 1 ){
			tregs = (LEON3_Timer_Regs_Map *)gptimer.start;
			sys_freq_hz = (tregs->scaler_reload+1)*1000*1000;
			iop_debug("OCCAN: detected %dHZ system frequency\n\r",sys_freq_hz);
		}else{
			sys_freq_hz = 40000000; /* Default to 40MHz */
			iop_debug("OCCAN: Failed to detect system frequency\n\r");
		}

	}
#elif defined(LEON2)
	/* LEON2: use hardcoded address to get to timer */
	{
		LEON_Register_Map *regs = (LEON_Register_Map *)0x80000000;
		sys_freq_hz = (regs->Scaler_Reload+1)*1000*1000;
	}
#else
  #error CPU not supported for OC_CAN driver
#endif
#else
	/* Use hardcoded frequency */
	sys_freq_hz = SYS_FREQ_HZ;
#endif

	iop_debug("OCCAN: Detected %dHz system frequency\n\r",sys_freq_hz);

	/* OCCAN speciality:
	 *  Mulitple cores are supported through the same amba AHB interface.
	 *  The number of "sub cores" can be detected by decoding the AMBA
	 *  Plug&Play version information. verion = ncores. A maximum of 8
	 *  sub cores are supported, each separeated with 0x100 inbetween.
	 *
	 *  Now, lets detect sub cores.
	 */

	for(subcore_cnt=devi=0; devi<dev_cnt; devi++){
		amba_find_next_ahbslv(amba_bus,VENDOR_GAISLER,GAISLER_OCCAN,&ambadev,devi);
		subcore_cnt += (ambadev.ver & 0x7)+1;
	}

	iop_debug("OCCAN: Found %d devs, totally %d sub cores\n\r",dev_cnt,subcore_cnt);

	for(devi=0; devi<dev_cnt; devi++){

		/* Get AHB device info */
		amba_find_next_ahbslv(amba_bus,VENDOR_GAISLER,GAISLER_OCCAN,&ambadev,devi);
		subcores = (ambadev.ver & 0x7)+1;
		iop_debug("OCCAN: on dev %d found %d sub cores\n\r",devi,subcores);

		/* loop all subcores, at least 1 */
		for(subi=0; subi<subcores; subi++){
//			can = &cans[minor];
			can = (occan_priv *) device->dev.driver;

#ifdef OCCAN_BYTE_REGS
			/* regs is byte regs */
			can->regs = (void *)(ambadev.start[0] + OCCAN_NCORE_OFS*subi);
#else
			/* regs is word regs, accessed 0x100 from base address */
			can->regs = (void *)(ambadev.start[0] + OCCAN_NCORE_OFS*subi+ OCCAN_WORD_REG_OFS);
#endif

			/* remember IRQ number */
			can->irq = ambadev.irq+subi;

			/* bind filesystem name to device */
			OCCAN_DEVNAME_NO(fs_name, device->dev_name);
			iop_debug("OCCAN: Registering %s @ 0x%lx irq %d\n\r",fs_name,(unsigned int)can->regs,can->irq);
			status = rtems_io_register_name(fs_name);
			if (RTEMS_SUCCESSFUL != status )
				rtems_fatal_error_occurred(status);

			/* initialize software */
			can->open = 0;
			can->rx_iop_buffer[can->rx_ptr] = NULL;
			can->tx_iop_buffer[can->tx_ptr] = NULL;
			status = rtems_semaphore_create(
                        rtems_build_name('C', 'd', 'v', device->dev_name),
                        1,
                        RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY | \
                        RTEMS_NO_PRIORITY_CEILING,
                        0,
                        &can->devsem);
			if ( status != RTEMS_SUCCESSFUL ){
				iop_debug("OCCAN: Failed to create dev semaphore for   (%d)\n\r", status);
				return RTEMS_UNSATISFIED;
			}
			status = rtems_semaphore_create(
                        rtems_build_name('C', 't', 'x', device->dev_name),
                        0,
                        RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY | \
                        RTEMS_NO_PRIORITY_CEILING,
                        0,
                        &can->txsem);
			if ( status != RTEMS_SUCCESSFUL ){
				iop_debug("OCCAN: Failed to create tx semaphore for minor , (%d)\n\r",status);
				return RTEMS_UNSATISFIED;
			}
			status = rtems_semaphore_create(
                        rtems_build_name('C', 'r', 'x', device->dev_name),
                        0,
                        RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY | \
                        RTEMS_NO_PRIORITY_CEILING,
                        0,
                        &can->rxsem);
			if ( status != RTEMS_SUCCESSFUL ){
				iop_debug("OCCAN: Failed to create rx semaphore for minor , (%d)\n\r",status);
				return RTEMS_UNSATISFIED;
			}

			/* hardware init/reset */
			pelican_init(can);

			/* Setup interrupt handler for each channel */
//    	OCCAN_REG_INT(OCCAN_PREFIX(_interrupt_handler), can->irq, can);

#ifdef DEBUG_PRINT_REGMAP
			pelican_regadr_print(can->regs);
#endif
		}
	}

	return RTEMS_SUCCESSFUL;
}

uint32_t occan_open(iop_device_driver_t *iop_dev, void *arg){
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	occan_priv *can = device->dev.driver;

	iop_debug("OCCAN: Opening \n\r");

	/* already opened? */
	rtems_semaphore_obtain(can->devsem,RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	if ( can->open ){
		rtems_semaphore_release(can->devsem);
		return RTEMS_RESOURCE_IN_USE; /* EBUSY */
	}
	can->open = 1;
	rtems_semaphore_release(can->devsem);

	/* allocate fifos */
	/* No longer needed allocation of the fifos occurs in a higher level */
//	can->rxfifo = occan_fifo_create(DEFAULT_RX_FIFO_LEN);
//	if( !occan_fifo_clear(can->rx_msg_queue) && !occan_fifo_clear(can->tx_msg_queue)){
//		return RTEMS_NO_MEMORY;
//	}

	/* check buffers allocation */
	if ( !can->rx_msg_queue || !can->rx_msg_queue){
		can->open = 0;
		iop_debug("OCCAN fifos not allocated on the upper level.\n");
		return RTEMS_NO_MEMORY; /* ENOMEM */
	}

//	can->txfifo = occan_fifo_create(DEFAULT_TX_FIFO_LEN);
	/* Checking buffers allocation */
//	if ( !can->tx_iop_buffer && !(*(can->tx_iop_buffer))){
//		occan_fifo_free(can->rxfifo);
//		can->rxfifo= NULL;
//		can->open = 0;
//		return RTEMS_NO_MEMORY; /* ENOMEM */
//	}

	/* Buffers allocation are good */
	iop_debug("OCCAN: Opening success\n\r");

	can->started = 0;
	can->channel = 0; /* Default to first can link */
	can->txblk = 1; /* Default to Blocking mode */
	can->rxblk = 1; /* Default to Blocking mode */
	can->filter->single_mode = 1; /* single mode acceptance filter */

	/* reset stat counters */
	memset(&can->stats,0,sizeof(occan_stats));

	/* HW must be in reset mode here (close and initializes resets core...)
	 *
	 * 1. set default modes/speeds
	 */
	pelican_open(can);

	return RTEMS_SUCCESSFUL;
}

uint32_t occan_close(iop_device_driver_t *iop_dev, void *arg){
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	occan_priv *can = device->dev.driver;

	iop_debug("OCCAN: Closing\n\r");

	/* stop if running */
	if ( can->started )
		pelican_stop(can);

	/* Enter Reset Mode */
//	can->regs->mode = PELICAN_MOD_RESET;
	WRITE_REG(can->regs->mode, PELICAN_MOD_RESET);

	occan_fifo_clear(can->rx_msg_queue);
	occan_fifo_clear(can->tx_msg_queue);

	return RTEMS_SUCCESSFUL;
}

uint32_t occan_read(iop_device_driver_t *iop_dev,  void *arg){

	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	occan_priv *can = (occan_priv *) device->dev.driver;
	CANMsg *dstmsg, *srcmsg;
	rtems_interrupt_level oldLevel;
	int left;

	/* get IOP buffer */
	iop_wrapper_t *wrapper = (iop_wrapper_t *) arg;
	iop_buffer_t *iop_buffer = wrapper->buffer;

	if ( !can->started ){
		iop_debug("OCCAN: cannot read when not started\n\r");
		return RTEMS_RESOURCE_IN_USE; /* -EBUSY*/
	}

	/* does at least one message fit */
	left = device->count;
	if ( left < sizeof(CANMsg) ){
		iop_debug("OCCAN: buffer must be at least %d, our is %d\n\r",sizeof(CANMsg),left);
		return  RTEMS_INVALID_NAME; /* -EINVAL */
	}

	/* Check if buffer was well allocated on the upper level */
	if(iop_buffer->v_addr == NULL) {
		return RTEMS_INVALID_NAME;
	}

	/* get pointer to start where to put CAN messages */
	dstmsg = (CANMsg *)wrapper->buffer->v_addr;

	if ( !dstmsg ){
		iop_debug("OCCAN: input buffer is NULL\n\r");
		return  RTEMS_INVALID_NAME; /* -EINVAL */
	}

	while (left >= sizeof(CANMsg) ){

		/* turn off interrupts */
		rtems_interrupt_disable(oldLevel);

		/* A bus off interrupt may have occured after checking can->started */
		if ( can->status & (OCCAN_STATUS_ERR_BUSOFF|OCCAN_STATUS_RESET) ){
			rtems_interrupt_enable(oldLevel);
			iop_debug("OCCAN: read is cancelled due to a BUS OFF error\n\r");
			device->bytes_moved = device->count-left;
			return RTEMS_IO_ERROR; /* EIO */
		}

		srcmsg = occan_fifo_get(can->rx_msg_queue);
		if ( !srcmsg ){
			/* no more messages in reception fifo.
			 * Wait for incoming packets only if in
			 * blocking mode AND no messages been
			 * read before.
			 */
			if ( !can->rxblk || (left != device->count) ){
				/* turn on interrupts again */
				rtems_interrupt_enable(oldLevel);
				break;
			}

			/* turn on interrupts again */
			rtems_interrupt_enable(oldLevel);

			iop_debug("OCCAN: Waiting for RX int\n\r");

			/* wait for incomming messages */
			rtems_semaphore_obtain(can->rxsem,RTEMS_WAIT,RTEMS_NO_TIMEOUT);

			/* did we get woken up by a BUS OFF error? */
			if ( can->status & (OCCAN_STATUS_ERR_BUSOFF|OCCAN_STATUS_RESET) ){
				iop_debug("OCCAN: Blocking read got woken up by BUS OFF error\n\r");
				/* At this point it should not matter how many messages we handled */
				device->bytes_moved = device->count-left;
				return RTEMS_IO_ERROR; /* EIO */
			}

			/* no errors detected, it must be a message */
			continue;
		}

		/* got message, copy it to userspace buffer */
		*dstmsg = *srcmsg;

		/* turn on interrupts again */
		rtems_interrupt_enable(oldLevel);

		/* increase pointers */
		left -= sizeof(CANMsg);
		dstmsg++;
	}

	/* save number of read bytes. */
	device->bytes_moved = device->count-left;
	if ( device->bytes_moved == 0 ){
		iop_debug("OCCAN: read would block, returning\n\r");
		return RTEMS_TIMEOUT; /* ETIMEDOUT should be EAGAIN/EWOULDBLOCK */
	}
	return RTEMS_SUCCESSFUL;
}

uint32_t occan_write(iop_device_driver_t *iop_dev, void *arg){

	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	occan_priv *can = (occan_priv *) device->dev.driver;
	iop_wrapper_t *wrapper = (iop_wrapper_t *) arg;
	CANMsg *msg;
	rtems_interrupt_level oldLevel;
	int left;

	iop_debug("OCCAN: Writing %d bytes from 0x%x (%d)\n\r",device->count, wrapper->buffer->v_addr, sizeof(CANMsg));

	if ( !can->started )
		return RTEMS_RESOURCE_IN_USE; /* EBUSY */

	left = device->count;
	if ( (left < sizeof(CANMsg)) || (!wrapper->buffer->v_addr) ){
		return RTEMS_INVALID_NAME; /* EINVAL */
	}

//	msg = (CANMsg *)device->buffer;
	/* GMVS */
	msg = (CANMsg *) wrapper->buffer->v_addr;

	/* limit CAN message length to 8 */
	msg->len = (msg->len > 8) ? 8 : msg->len;

#ifdef DEBUG_VERBOSE
	pelican_regs_print(can->regs);
	occan_stat_print(&can->stats);
#endif

	/* turn off interrupts */
	rtems_interrupt_disable(oldLevel);

	/* A bus off interrupt may have occured after checking can->started */
	if ( can->status & (OCCAN_STATUS_ERR_BUSOFF|OCCAN_STATUS_RESET) ){
		rtems_interrupt_enable(oldLevel);
		device->bytes_moved = 0;
		return RTEMS_IO_ERROR; /* EIO */
	}

	/* If no messages in software tx fifo, we will
	 * try to send first message by putting it directly
	 * into the HW TX fifo.
	 */
	if ( occan_fifo_empty(can->tx_msg_queue ) ){
		/*pelican_regs_print(cans[minor+1].regs);*/
		if ( !pelican_send(can,msg) ) {
			/* First message put directly into HW TX fifo
			 * This will turn TX interrupt on.
			 */
			left -= sizeof(CANMsg);
			msg++;

			can->sending = 1;
			/* bump stat counters */
			can->stats.tx_msgs++;

			iop_debug("OCCAN: Sending direct via HW\n\r");
		}
	}

	/* Put messages into software fifo */
	while ( left >= sizeof(CANMsg) ){

		/* limit CAN message length to 8 */
		msg->len = (msg->len > 8) ? 8 : msg->len;

		/* TODO check if this condition is ok */
		if (!occan_fifo_put(can->tx_msg_queue, msg,0)){

			iop_debug("OCCAN: FIFO is full\n\r");
			/* Block only if no messages previously sent
			 * and not in blocking mode
			 */
			if ( !can->txblk  || (left != device->count) )
				break;

			/* turn on interupts again and wait
				INT_ON
				WAIT FOR FREE BUF;
				INT_OFF;
				CHECK_IF_FIFO_EMPTY ==> SEND DIRECT VIA HW;
			*/
			rtems_interrupt_enable(oldLevel);

			iop_debug("OCCAN: Waiting for tx int\n\r");

			rtems_semaphore_obtain(can->txsem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);

			/* did we get woken up by a BUS OFF error? */
			if ( can->status & (OCCAN_STATUS_ERR_BUSOFF|OCCAN_STATUS_RESET) ){
				iop_debug("OCCAN: Blocking write got woken up by BUS OFF error or RESET event\n\r");
				/* At this point it should not matter how many messages we handled */
				device->bytes_moved = device->count-left;
				return RTEMS_IO_ERROR; /* EIO */
			}

			rtems_interrupt_disable(oldLevel);

			if ( occan_fifo_empty(can->tx_msg_queue) ){
				if ( !pelican_send(can,msg) ) {
					/* First message put directly into HW TX fifo
				   * This will turn TX interrupt on.
				   */
					left -= sizeof(CANMsg);
					msg++;

					/* bump stat counters */
					can->stats.tx_msgs++;

					iop_debug("OCCAN: Sending direct2 via HW\n\r");
				}
			}
			continue;
		}

		/* copy message into fifo area */
//		*fifo_msg = *msg;

		/* tell interrupt handler about the message */
		/* At this point if we were not able to free space
		 * from the queue the message might be lost */
		occan_fifo_put(can->tx_msg_queue, msg, 0); /* TODO consider using the force signal at 1 and delete the oldest message */

		iop_debug("OCCAN: Put info fifo SW\n\r");

		/* Prepare insert of next message */
		msg++;
		left-=sizeof(CANMsg);
	}

	rtems_interrupt_enable(oldLevel);

	device->bytes_moved = device->count-left;
	iop_debug("OCCAN: Sent %d\n\r",device->bytes_moved);

	if ( left == device->count )
		return RTEMS_TIMEOUT; /* ETIMEDOUT should be EAGAIN/EWOULDBLOCK */
	return RTEMS_SUCCESSFUL;
}

uint32_t occan_ioctl(iop_can_device_t *device, void *arg){
	int ret;
	occan_speed_regs timing;
	occan_priv *can = device->dev.driver;
	unsigned int speed;
	rtems_libio_ioctl_args_t	*ioarg = (rtems_libio_ioctl_args_t *) arg;
	occan_filter *afilter;
	occan_stats *dststats;
//	unsigned int rxcnt,txcnt;

	iop_debug("OCCAN: IOCTL %d\n\r",ioarg->command);

	ioarg->ioctl_return = 0;
	switch(ioarg->command){
		case OCCAN_IOC_SET_SPEED:

			/* cannot change speed during run mode */
			if ( can->started )
				return RTEMS_RESOURCE_IN_USE; /* EBUSY */

			/* get speed rate from argument */
			speed = (unsigned int)ioarg->buffer;
			ret = occan_calc_speedregs(sys_freq_hz,speed,&timing);
			if ( ret )
				return  RTEMS_INVALID_NAME; /* EINVAL */

			/* set the speed regs of the CAN core */
			/* occan_set_speedregs(can,timing); */

			/* save timing/speed */
			can->speed = speed;
			can->timing = timing;
			break;

		case OCCAN_IOC_SET_BTRS:
			/* Set BTR registers manually
			 * Read OCCAN Manual.
			 */
			if ( can->started )
				return RTEMS_RESOURCE_IN_USE; /* EBUSY */

			can->speed = 0; /* custom */
			can->timing.btr1 = (unsigned int)ioarg->buffer & 0xff;
			can->timing.btr0 = ((unsigned int)ioarg->buffer>>8) & 0xff;
/*
			can->timing.sjw = (btr0 >> OCCAN_BUSTIM_SJW_BIT) & 0x3;
			can->timing.brp = btr0 & OCCAN_BUSTIM_BRP;
			can->timing.tseg1 = btr1 & 0xf;
			can->timing.tseg2 = (btr1 >> OCCAN_BUSTIM_TSEG2_BIT) & 0x7;
			can->timing.sam = (btr1 >> 7) & 0x1;
			*/
			break;

		case OCCAN_IOC_SPEED_AUTO:
			return RTEMS_NOT_IMPLEMENTED;
			if ( can->started )
				return RTEMS_RESOURCE_IN_USE; /* EBUSY */

			if ( (speed=pelican_speed_auto(can)) < 0 ){
				/* failed */
				return RTEMS_IO_ERROR;
			}

			/* set new speed */
			can->speed = speed;

			if ( (int *)ioarg->buffer ){
				*(int *)ioarg->buffer = speed;
			}
			return RTEMS_SUCCESSFUL;
			break;
			/* The commented case is not used anymore.
			 * Fifo allocation is no longer dynamic */
//		case OCCAN_IOC_SET_BUFLEN:
//			/* set rx & tx fifo buffer length */
//			if ( can->started )
//				return RTEMS_RESOURCE_IN_USE; /* EBUSY */
//
//			rxcnt = (unsigned int)ioarg->buffer & 0x0000ffff;
//			txcnt = (unsigned int)ioarg->buffer >> 16;
//
//			occan_fifo_free(can->rxfifo);
//			occan_fifo_free(can->txfifo);
//
//			/* allocate new buffers */
//			can->rxfifo = occan_fifo_create(rxcnt);
//			can->txfifo = occan_fifo_create(txcnt);
//
//			if ( !can->rxfifo || !can->txfifo )
//				return RTEMS_NO_MEMORY; /* ENOMEM */
//			break;

		case OCCAN_IOC_GET_CONF:
			return RTEMS_NOT_IMPLEMENTED;
			break;

		case OCCAN_IOC_GET_STATS:
			dststats = (occan_stats *)ioarg->buffer;
			if ( !dststats )
				return  RTEMS_INVALID_NAME; /* EINVAL */

			/* copy data stats into userspace buffer */
      if ( can->rx_msg_queue )
        can->stats.rx_sw_dovr = can->rx_msg_queue->ovcnt;
			*dststats = can->stats;
			break;

		case OCCAN_IOC_GET_STATUS:
			/* return the status of the */
			if ( !ioarg->buffer )
				return RTEMS_INVALID_NAME;

			*(unsigned int *)ioarg->buffer = can->status;
			break;

		/* Set physical link */
		case OCCAN_IOC_SET_LINK:
#ifdef REDUNDANT_CHANNELS
			if ( can->started )
				return RTEMS_RESOURCE_IN_USE; /* EBUSY */

			/* switch HW channel */
			can->channel = (unsigned int)ioargs->buffer;
#else
			return RTEMS_NOT_IMPLEMENTED;
#endif
			break;

		case OCCAN_IOC_SET_FILTER:
			if ( can->started )
				return RTEMS_RESOURCE_IN_USE; /* EBUSY */

			afilter = (occan_filter *) ioarg->buffer;

			if ( !afilter )
				return RTEMS_INVALID_NAME; /* EINVAL */

			/* copy acceptance filter */
			can->filter->code[0] = afilter->code[0];
			can->filter->code[1] = afilter->code[1];
			can->filter->code[2] = afilter->code[2];
			can->filter->code[3] = afilter->code[3];

			can->filter->mask[0] = afilter->mask[0];
			can->filter->mask[1] = afilter->mask[1];
			can->filter->mask[2] = afilter->mask[2];
			can->filter->mask[3] = afilter->mask[3];

			can->filter->single_mode = ( afilter->single_mode ) ? 1 : 0;

			/* TODO verify is this driver is using the can filters
			 * or not. */
			/* Acceptance filter is written to hardware
			 * when starting.
			 */
			/* pelican_set_accept(can,can->acode,can->amask);*/
			break;

		case OCCAN_IOC_SET_BLK_MODE:
			can->rxblk = (unsigned int)ioarg->buffer & OCCAN_BLK_MODE_RX;
			can->txblk = ((unsigned int)ioarg->buffer & OCCAN_BLK_MODE_TX) >> 1;
			break;

		case OCCAN_IOC_START:
			if ( can->started )
				return RTEMS_RESOURCE_IN_USE; /* EBUSY */
			if ( pelican_start(can) )
				return RTEMS_NO_MEMORY; /* failed because of no memory, can happen if SET_BUFLEN failed */
			can->started = 1;
			break;

		case OCCAN_IOC_STOP:
			if ( !can->started )
				return RTEMS_RESOURCE_IN_USE; /* EBUSY */
			pelican_stop(can);
			can->started = 0;
			break;

		default:
			return RTEMS_NOT_DEFINED;
	}
	return RTEMS_SUCCESSFUL;
}
//
static void occan_interrupt(occan_priv *can){
	unsigned char iflags;
	pelican_regs *regs = can->regs;
	CANMsg *msg_rx;
	CANMsg msg_tx; /* Temporary vairable to hold the message */
	int signal_rx=0, signal_tx=0;
	unsigned char tmp, errcode, arbcode;
	int tx_error_cnt,rx_error_cnt;

	can->stats.ints++;

	iflags = READ_REG(&can->regs->intflags);

	while ( iflags != 0 ){
		/* still interrupts to handle */

		if ( iflags & PELICAN_IF_RX ){
			/* the rx fifo is not empty
			 * put 1 message into rxfifo for later use
			 */

			/* get empty (or make room) message */
//			msg = occan_fifo_put_claim(can->rxfifo,1);
			tmp = READ_REG(&regs->rx_fi_xff);
			msg_tx.extended = tmp >> 7;
			msg_tx.rtr = (tmp >> 6) & 1;
			msg_tx.len = tmp = tmp & 0x0f;

			if ( msg_tx.extended ){
				/* extended message */
				msg_tx.id =  READ_REG(&regs->msg.rx_eff.id[0])<<(5+8+8) |
				           READ_REG(&regs->msg.rx_eff.id[1])<<(5+8) |
				           READ_REG(&regs->msg.rx_eff.id[2])<<5 |
				           READ_REG(&regs->msg.rx_eff.id[3])>>3;
				while(tmp--){
					msg_tx.data[tmp] = READ_REG(&regs->msg.rx_eff.data[tmp]);
				}
				/*
				msg->data[0] = READ_REG(&regs->msg.rx_eff.data[0]);
				msg->data[1] = READ_REG(&regs->msg.rx_eff.data[1]);
				msg->data[2] = READ_REG(&regs->msg.rx_eff.data[2]);
				msg->data[3] = READ_REG(&regs->msg.rx_eff.data[3]);
				msg->data[4] = READ_REG(&regs->msg.rx_eff.data[4]);
				msg->data[5] = READ_REG(&regs->msg.rx_eff.data[5]);
				msg->data[6] = READ_REG(&regs->msg.rx_eff.data[6]);
				msg->data[7] = READ_REG(&regs->msg.rx_eff.data[7]);
				*/
			}else{
				/* standard message */
				msg_tx.id =  READ_REG(&regs->msg.rx_sff.id[0])<<3 |
				           READ_REG(&regs->msg.rx_sff.id[1])>>5;

				while(tmp--){
					msg_tx.data[tmp] = READ_REG(&regs->msg.rx_sff.data[tmp]);
				}
				/*
				msg->data[0] = READ_REG(&regs->msg.rx_sff.data[0]);
				msg->data[1] = READ_REG(&regs->msg.rx_sff.data[1]);
				msg->data[2] = READ_REG(&regs->msg.rx_sff.data[2]);
				msg->data[3] = READ_REG(&regs->msg.rx_sff.data[3]);
				msg->data[4] = READ_REG(&regs->msg.rx_sff.data[4]);
				msg->data[5] = READ_REG(&regs->msg.rx_sff.data[5]);
				msg->data[6] = READ_REG(&regs->msg.rx_sff.data[6]);
				msg->data[7] = READ_REG(&regs->msg.rx_sff.data[7]);
				*/
			}

			/* Re-Enable RX buffer for a new message */
//			regs->cmd = PELICAN_CMD_RELRXBUF;
			WRITE_REG(regs->cmd, PELICAN_CMD_RELRXBUF);

			/* Queue the received message to make it available
			 * to the user */
			occan_fifo_put(can->rx_msg_queue, &msg,1);

			/* bump stat counters */
			can->stats.rx_msgs++;

			/* signal the semaphore only once */
			signal_rx = 1;
		}

		if ( iflags & PELICAN_IF_TX ){
			/* there is room in tx fifo of HW */

			if ( !occan_fifo_empty(can->tx_msg_queue) ){
				/* send 1 more messages */
				msg = occan_fifo_get(can->tx_msg_queue);

				if ( pelican_send(can,msg) ){
					/* ERROR! We got an TX interrupt telling us
					 * tx fifo is empty, yet it is not.
					 *
					 * Complain about this max 10 times
					 */
					if ( can->stats.tx_buf_error < 10 ){
						iop_debug("OCCAN: got TX interrupt but TX fifo in not empty (%d)\n\r",can->stats.tx_buf_error);
					}
					can->status |= OCCAN_STATUS_QUEUE_ERROR;
					can->stats.tx_buf_error++;
				}

				/* free software-fifo space taken by sent message */
//				occan_fifo_get(can->txfifo);

				/* bump stat counters */
				can->stats.tx_msgs++;

				/* wake any sleeping thread waiting for "fifo not full" */
				signal_tx = 1;
			}
		}

		if ( iflags & PELICAN_IF_ERRW ){
			tx_error_cnt = READ_REG(&regs->tx_err_cnt);
			rx_error_cnt = READ_REG(&regs->rx_err_cnt);

			/* 1. if bus off tx error counter = 127 */
			if ( (tx_error_cnt > 96) || (rx_error_cnt > 96) ){
				/* in Error Active Warning area or BUS OFF */
				can->status |= OCCAN_STATUS_WARN;

				/* check reset bit for reset mode */
				if ( READ_REG(&regs->mode) & PELICAN_MOD_RESET ){
					/* in reset mode ==> bus off */
					can->status |= OCCAN_STATUS_ERR_BUSOFF | OCCAN_STATUS_RESET;

					/***** pelican_stop(can) ******
					 * turn off interrupts
					 * enter reset mode (HW already done that for us)
					 */
//					regs->inten = 0;
					WRITE_REG(regs->inten, 0);

					/* Indicate that we are not started any more.
					 * This will make write/read return with EBUSY
					 * on read/write attempts.
					 *
					 * User must issue a ioctl(START) to get going again.
					 */
					can->started = 0;

					/* signal any waiting read/write threads, so that they
					 * can handle the bus error.
					 */
					signal_rx = 1;
					signal_tx = 1;

					/* ingnore any old pending interrupt */
					break;
				}

			}else{
				/* not in Upper Error Active area any more */
				can->status &= ~(OCCAN_STATUS_WARN);
			}
			can->stats.err_warn++;
		}

		if ( iflags & PELICAN_IF_DOVR){
			can->status |= OCCAN_STATUS_OVERRUN;
			can->stats.err_dovr++;
			iop_debug("OCCAN_INT: DOVR\n\r");
		}

		if ( iflags & PELICAN_IF_ERRP){
			/* Let the error counters decide what kind of
			 * interrupt it was. In/Out of EPassive area.
			 */
			tx_error_cnt = READ_REG(&regs->tx_err_cnt);
			rx_error_cnt = READ_REG(&regs->rx_err_cnt);

			if ( (tx_error_cnt > 127) || (rx_error_cnt > 127) ){
				can->status |= OCCAN_STATUS_ERR_PASSIVE;
			}else{
				can->status &= ~(OCCAN_STATUS_ERR_PASSIVE);
			}

			/* increase Error Passive In/out interrupt counter */
			can->stats.err_errp++;
		}

		if ( iflags & PELICAN_IF_ARB){
			arbcode = READ_REG(&regs->arbcode);
			can->stats.err_arb_bitnum[arbcode & PELICAN_ARB_BITS]++;
			can->stats.err_arb++;
			iop_debug("OCCAN_INT: ARB (0x%x)\n\r",arbcode & PELICAN_ARB_BITS);
		}

		if ( iflags & PELICAN_IF_BUS){
			/* Some kind of BUS error, only used for
			 * statistics. Error Register is decoded
			 * and put into can->stats.
			 */
			errcode = READ_REG(&regs->errcode);
			switch( errcode & PELICAN_ECC_CODE ){
				case PELICAN_ECC_CODE_BIT:
					can->stats.err_bus_bit++;
					break;
				case PELICAN_ECC_CODE_FORM:
					can->stats.err_bus_form++;
					break;
				case PELICAN_ECC_CODE_STUFF:
					can->stats.err_bus_stuff++;
					break;
				case PELICAN_ECC_CODE_OTHER:
					can->stats.err_bus_other++;
					break;
			}

			/* Get Direction (TX/RX) */
			if ( errcode & PELICAN_ECC_DIR ){
				can->stats.err_bus_rx++;
			}else{
				can->stats.err_bus_tx++;
			}

			/* Get Segment in frame that went wrong */
			can->stats.err_bus_segs[errcode & PELICAN_ECC_SEG]++;

			/* total number of bus errors */
			can->stats.err_bus++;
		}
	}

	/* signal Binary semaphore, messages available! */
	if ( signal_rx ){
		rtems_semaphore_release(can->rxsem);
	}

	if ( signal_tx ){
		rtems_semaphore_release(can->txsem);
	}
}

//#ifdef OCCAN_DEFINE_INTHANDLER
//static void occan_interrupt_handler(rtems_vector_number v){
//	int minor;
//
//	/* convert to */
//  for(minor = 0; minor < can_cores; minor++) {
//  	if ( v == (cans[minor].irq+0x10) ) {
//			occan_interrupt(&cans[minor]);
//			return;
//		}
//	}
//}
//#endif

#define OCCAN_DRIVER_TABLE_ENTRY { occan_initialize, occan_open, occan_close, occan_read, occan_write, occan_ioctl }

static rtems_driver_address_table occan_driver = OCCAN_DRIVER_TABLE_ENTRY;

int OCCAN_PREFIX(_register)(amba_confarea_type *bus){
	rtems_status_code r;
	rtems_device_major_number m;

	amba_bus = bus;
	if ( !bus )
		return 1;

  if ((r = rtems_io_register_driver(0, &occan_driver, &m)) == RTEMS_SUCCESSFUL) {
		iop_debug("OCCAN driver successfully registered, major: %d\n\r", m);
	}else{
		switch(r) {
			case RTEMS_TOO_MANY:
				iop_debug("OCCAN rtems_io_register_driver failed: RTEMS_TOO_MANY\n\r"); break;
			case RTEMS_INVALID_NUMBER:
				iop_debug("OCCAN rtems_io_register_driver failed: RTEMS_INVALID_NUMBER\n\r"); break;
			case RTEMS_RESOURCE_IN_USE:
				iop_debug("OCCAN rtems_io_register_driver failed: RTEMS_RESOURCE_IN_USE\n\r"); break;
			default:
				iop_debug("OCCAN rtems_io_register_driver failed\n\r");
		}
		return 1;
	}
	return 0;
}
