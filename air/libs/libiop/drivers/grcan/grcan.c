/*
 *  GRCAN driver  
 *
 *  COPYRIGHT (c) 2007.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 * 
 * Modified by: gmvs@gmv.com
 * 
 * The original driver has been modified to be included in the Hypervisor
 * AIR, developed by gmv. It is assumed to be used for the GR740 board.
 * 
 *
 * NOTE: When running this example on GR740, the following have to be configured:
 * - CAN pin multiplexing
 * - Enable GRCAN clock
 *
 * This can be done in GRMON2 using the following commands before running the
 * example:
 *   grmon2> wmem 0xffa0b000 0x000ffc3c
 *   grmon2> grcg enable 5
 *
 */

/*
 * Modified by gmvs @ GMV 2018
 * gmvs@gmv.com
 */

#include <bsp.h>

#include <grcan.h>
#include <ambaext.h>
#include <ambapp.h>
#include <can_support.h>
#include <iop.h>
#include <amba.h>
#include <air.h>

/* Maximum number of GRCAN devices supported by driver */
#define GRCAN_COUNT_MAX 8

#define WRAP_AROUND_TX_MSGS 1
#define WRAP_AROUND_RX_MSGS 2
#define GRCAN_MSG_SIZE sizeof(grcan_msg)
#define BLOCK_SIZE (16*4)

/* grcan needs to have it buffers aligned to 1k boundaries */
#define BUFFER_ALIGNMENT_NEEDS 1024

/* Default Maximium buffer size for statically allocated buffers */
#ifndef TX_BUF_SIZE
#define TX_BUF_SIZE (BLOCK_SIZE*16)
#endif

/* Make receiver buffers bigger than transmitt */
#ifndef RX_BUF_SIZE
#define RX_BUF_SIZE ((3*BLOCK_SIZE)*16)
#endif

#ifndef IRQ_CLEAR_PENDING
 #define IRQ_CLEAR_PENDING(irqno) 
#endif

#ifndef IRQ_UNMASK
 #define IRQ_UNMASK(irqno) 
#endif

#ifndef IRQ_MASK
 #define IRQ_MASK(irqno) 
#endif

#ifndef GRCAN_DEFAULT_BAUD
  /* default to 500kbits/s */
  #define GRCAN_DEFAULT_BAUD 500000
#endif

#ifndef GRCAN_SAMPLING_POINT
#define GRCAN_SAMPLING_POINT 80
#endif

/* This assumes that the driver is used on the GR740 board */
#define CAN_CLOCK_SPEED 250000000
/* From GRCAN datasheet */
#define GRCAN0 0xffa01000
#define GRCAN1 0xffa02000

/****************** DEBUG Definitions ********************/
#define DBG_TX 2
#define DBG_RX 4
#define DBG_STATE 8

#define DEBUG_FLAGS (DBG_STATE | DBG_RX | DBG_TX )
/*gmvs*/
#ifdef IOP_NEEDS_DEBUG
  #define DEBUG
  #define DEBUGFUNCS
#endif

#include <debug_defs.h>
/*********************************************************/

int state2err[4] = {
	/* STATE_STOPPED */ GRCAN_RET_NOTSTARTED,
	/* STATE_STARTED */ GRCAN_RET_OK,
	/* STATE_BUSOFF  */ GRCAN_RET_BUSOFF,
	/* STATE_AHBERR  */ GRCAN_RET_AHBERR
};

int grcan_get_state(iop_device_driver_t *iop_dev){
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	FUNCDBG();

	return pDev->started;
}

static void __inline__ grcan_hw_reset(struct grcan_regs *regs);

static int grcan_hw_read_try(
  grcan_priv *pDev,
  struct grcan_regs *regs,
  CANMsg *buffer,
  int max);

static int grcan_hw_write_try(
  grcan_priv *pDev,
  struct grcan_regs *regs,
  CANMsg *buffer,
  int count);

static void grcan_hw_config(
  struct grcan_regs *regs, 
  struct grcan_config *conf);
  
static void grcan_hw_accept(
  struct grcan_regs *regs,
  struct grcan_filter *afilter);

static void grcan_hw_sync(
  struct grcan_regs *regs,
  struct grcan_filter *sfilter);

#ifdef GRCAN_REG_BYPASS_CACHE
#define READ_REG(address) _grcan_read_nocache((unsigned int)(address))
#else
#define READ_REG(address) (*(volatile unsigned int *)(address))
#endif

#ifdef GRCAN_DMA_BYPASS_CACHE
#define READ_DMA_WORD(address) _grcan_read_nocache((unsigned int)(address))
#define READ_DMA_BYTE(address) _grcan_read_nocache_byte((unsigned int)(address))
static unsigned char __inline__ _grcan_read_nocache_byte(unsigned int address)
{
  unsigned char tmp;
	__asm__ (" lduba [%1]1, %0 "
    : "=r"(tmp)
    : "r"(address)
  );
  return tmp;
}
#else
#define READ_DMA_WORD(address) (*(volatile unsigned int *)(address))
#define READ_DMA_BYTE(address) (*(volatile unsigned char *)(address))
#endif

#if defined(GRCAN_REG_BYPASS_CACHE) || defined(GRCAN_DMA_BYPASS_CACHE)
static unsigned int __inline__ _grcan_read_nocache(unsigned int address)
{
  unsigned int tmp;
  __asm__ (" lda [%1]1, %0 "
    : "=r"(tmp)
    : "r"(address)
  );
  return tmp;
}
#endif

#define NELEM(a) ((int) (sizeof (a) / sizeof (a[0])))
static int grcan_count = 0;

int grcan_get_status(iop_device_driver_t *iop_dev, unsigned int *data)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev->driver;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	FUNCDBG();

      if ( !data )
		return -1;

      /* Read out the statsu register from the GRCAN core */
      data[0] = READ_REG(&pDev->regs->stat);

	return 0;
}

struct amba_dev_id grcan_ids[] =
{
	{VENDOR_GAISLER, GAISLER_GRCAN},
	{VENDOR_GAISLER, GAISLER_GRHCAN},
	{0, 0}		/* Mark end of table */
};

static void __inline__ grcan_hw_reset(struct grcan_regs *regs)
{
	regs->ctrl = GRCAN_CTRL_RESET;
}

static rtems_device_driver grcan_hw_start(grcan_priv *pDev)
{
	/*
	 * tmp is set but never used. GCC gives a warning for this
	 * and we need to tell GCC not to complain.
	 */
	//	unsigned int tmp RTEMS_UNUSED;
	//	unsigned int tmp RTEMS_SUCCESSFUL;
	unsigned int tmp;

	FUNCDBG();
	DBG("pDev address: 0x%04x\n", pDev);
	DBG("tx address: 0x%04x | rx address: 0x%04x \n",pDev->tx, pDev->rx);


	/* Check that memory has been allocated successfully */
	if(pDev->tx == NULL || pDev->rx == NULL){
		DBG("No memory allocated\n");
		return RTEMS_NO_MEMORY;
	}

	/*
	 * Configure FIFO configuration register
	 * and Setup timing
	 */
	if ( pDev->config_changed ){
		grcan_hw_config(pDev->regs,&pDev->config);
		pDev->config_changed = 0;
	}

	/* Setup receiver */
	pDev->regs->rx0addr =
			(unsigned int) air_syscall_get_physical_addr((uintptr_t)pDev->rx);
	pDev->regs->rx0size = pDev->rxbuf_size;

	DBG("RX buffer size %d | SW RX buffer size %d\n",
			pDev->regs->rx0size,
			pDev->rxbuf_size);

	/* Setup Transmitter */
	pDev->regs->tx0addr =
			(unsigned int) air_syscall_get_physical_addr((uintptr_t)pDev->tx);
	pDev->regs->tx0size = pDev->txbuf_size;

	/* Setup acceptance filters */
	grcan_hw_accept(pDev->regs,&pDev->afilter);

	/* Sync filters */
	grcan_hw_sync(pDev->regs,&pDev->sfilter);

	/* Clear status bits */
	tmp = READ_REG(&pDev->regs->stat);
	pDev->regs->stat = 0;

	/* Setup IRQ handling */

	/* Clear all IRQs */
	tmp = READ_REG(&pDev->regs->pir);
	pDev->regs->picr = 0x1ffff;

	/* unmask TxLoss|TxErrCntr|RxErrCntr|TxAHBErr|RxAHBErr|OR|OFF|PASS */
	pDev->regs->imr = 0x1601f;

	/* Enable receiver/transmitter */
	pDev->regs->rx0ctrl = GRCAN_RXCTRL_ENABLE;
	pDev->regs->tx0ctrl = GRCAN_TXCTRL_ENABLE;

	/* Enable HurriCANe core */
	pDev->regs->ctrl = GRCAN_CTRL_ENABLE;

	/* Leave transmitter disabled, it is enabled when
	* trying to send something.
	*/
	return RTEMS_SUCCESSFUL;
}

static void grcan_hw_stop(grcan_priv *pDev)
{
  FUNCDBG();
     
  /* Mask all IRQs */
  pDev->regs->imr = 0;
  IRQ_MASK(pDev->irq+GRCAN_IRQ_TXSYNC);
  IRQ_MASK(pDev->irq+GRCAN_IRQ_RXSYNC);
  IRQ_MASK(pDev->irq+GRCAN_IRQ_IRQ);
  
  /* Disable receiver & transmitter */
  pDev->regs->rx0ctrl = 0;
  pDev->regs->tx0ctrl = 0;
}
  
static void grcan_sw_stop(grcan_priv *pDev)
{
	/*
	 * Release semaphores to wake all threads waiting for an IRQ.
	 * The threads that
	 * get woken up must check started state in
   * order to determine that they should return to
   * user space with error status.
	 *
	 * Entering into started mode again will reset the
	 * semaphore count.
   */
	rtems_semaphore_release(pDev->rx_sem);
	rtems_semaphore_release(pDev->tx_sem);
	rtems_semaphore_release(pDev->txempty_sem);
}

static void grcan_hw_config(struct grcan_regs *regs, struct grcan_config *conf)
{
  unsigned int config=0;
  
  /* Reset HurriCANe Core */
  regs->ctrl = 0;
  
  if ( conf->silent )
    config |= GRCAN_CFG_SILENT;
     
  if ( conf->abort )
    config |= GRCAN_CFG_ABORT;
  
  if ( conf->selection.selection )
    config |= GRCAN_CFG_SELECTION;
      
  if ( conf->selection.enable0 )
    config |= GRCAN_CFG_ENABLE0;
      
  if ( conf->selection.enable1 )
    config |= GRCAN_CFG_ENABLE1;
  
  /* Timing */
  config |= (conf->timing.bpr<<GRCAN_CFG_BPR_BIT) & GRCAN_CFG_BPR;
  config |= (conf->timing.rsj<<GRCAN_CFG_RSJ_BIT) & GRCAN_CFG_RSJ;
  config |= (conf->timing.ps1<<GRCAN_CFG_PS1_BIT) & GRCAN_CFG_PS1;
  config |= (conf->timing.ps2<<GRCAN_CFG_PS2_BIT) & GRCAN_CFG_PS2;
  config |= 
	(conf->timing.scaler<<GRCAN_CFG_SCALER_BIT) & GRCAN_CFG_SCALER;
  
  /* Write configuration */
  regs->conf = config;
  
  /* Enable HurriCANe Core */
  regs->ctrl = GRCAN_CTRL_ENABLE;
}

static void grcan_hw_accept(
  struct grcan_regs *regs,
  struct grcan_filter *afilter
  )
{
  /* Disable Sync mask totaly (if we change scode or smask
   * in an unfortunate way we may trigger a sync match)
   */ 
  regs->rx0mask = 0xffffffff;
  
  /* Set Sync Filter in a controlled way */
  regs->rx0code = afilter->code;
  regs->rx0mask = afilter->mask;
}

static void grcan_hw_sync(struct grcan_regs *regs, struct grcan_filter *sfilter)
{
  /* Disable Sync mask totaly (if we change scode or smask
   * in an unfortunate way we may trigger a sync match)
   */ 
  regs->smask = 0xffffffff;
  
  /* Set Sync Filter in a controlled way */
  regs->scode = sfilter->code;
  regs->smask = sfilter->mask;
}

static unsigned int grcan_hw_rxavail(
  unsigned int rp,
	unsigned int wp, unsigned int size
  )
{ 
  if ( rp == wp ) {
    /* read pointer and write pointer is equal only 
     * when RX buffer is empty.
     */
    return 0;
  }
  
  if ( wp > rp ) {
    return (wp-rp)/GRCAN_MSG_SIZE;
  }else{
    return (size-(rp-wp))/GRCAN_MSG_SIZE;
  }
}

static unsigned int grcan_hw_txspace(
  unsigned int rp,
  unsigned int wp,
  unsigned int size
  )
{
  unsigned int left;
  DBG("ARGS\nwp\trp\tsize\n%d\t%d\t%d \n", wp, rp, size);
  if ( rp == wp ) {
    /* read pointer and write pointer is equal only 
     * when TX buffer is empty.
     */
	  DBG("TX buffer empty\n");
    return size/GRCAN_MSG_SIZE-WRAP_AROUND_TX_MSGS;
  }
  DBG("TX buffer not empty\n");
  /* size - 4 - abs(read-write) */
  if ( wp > rp ) {
    left = size-(wp-rp);
  }else{
    left = rp-wp;
  }
  
  return left/GRCAN_MSG_SIZE-WRAP_AROUND_TX_MSGS;
}

#define MIN_TSEG1 1
#define MIN_TSEG2 2
#define MAX_TSEG1 14
#define MAX_TSEG2 8

int grcan_start(iop_device_driver_t *iop_dev)
{
	FUNCDBG();
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	/* setup IOP buffers */
	setup_iop_buffers(pDev->iop_buffers,
			pDev->iop_buffers_storage,
			device->rx_count + device->tx_count);

	if (grcan_get_state(iop_dev) == STATE_STARTED) {
		return -1;
	}

	if ( (grcan_hw_start(pDev)) != RTEMS_SUCCESSFUL ){
		DBG("Hardware start unsuccessful\n");
		return -2;
	}

	/* Clear semaphore state. This is to avoid effects from previous
	 * bus-off/stop where semaphores where flushed() but the count remained.
	 */
	rtems_semaphore_obtain(pDev->rx_sem, RTEMS_NO_WAIT, 0);
	rtems_semaphore_obtain(pDev->tx_sem, RTEMS_NO_WAIT, 0);
	rtems_semaphore_obtain(pDev->txempty_sem, RTEMS_NO_WAIT, 0);

	/* Read and write are now open... */
	pDev->started = STATE_STARTED;
	DBGC(DBG_STATE, "STOPPED|BUSOFF|AHBERR->STARTED\n");

	return 0;
}

static int grcan_calc_timing(
  unsigned int baud,          /* The requested BAUD to calculate timing for */
  unsigned int core_hz,       /* Frequency in Hz of GRCAN Core */
  unsigned int sampl_pt,
  struct grcan_timing *timing /* result is placed here */
  )
{
	int best_error = 1000000000;
	int error;
	int best_tseg = 0, best_brp = 0, brp = 0;
	int tseg=0, tseg1=0, tseg2=0;
	int sjw = 1;
  
  /* Default to 90% */
  if ( (sampl_pt < 50) || (sampl_pt>99) ){
    sampl_pt = GRCAN_SAMPLING_POINT;
  }

	if ( (baud<5000) || (baud>1000000) ){
		/* invalid speed mode */
		return -1;
	}
	
	/* find best match, return -2 if no good reg 
	 * combination is available for this frequency
   */

	/* some heuristic specials */
	if (baud > ((1000000 + 500000) / 2))
		sampl_pt = 75;

	if (baud < ((12500 + 10000) / 2))
		sampl_pt = 75;

	/* tseg even = round down, odd = round up */
	for (
		tseg = (MIN_TSEG1 + MIN_TSEG2 + 2) * 2;
	     tseg <= (MAX_TSEG2 + MAX_TSEG1 + 2) * 2 + 1;
		tseg++
	) {
		brp = core_hz / ((1 + tseg / 2) * baud) + tseg % 2;
		if (
			(brp <= 0) ||
        ( (brp > 256*1) && (brp <= 256*2) && (brp&0x1) ) ||
        ( (brp > 256*2) && (brp <= 256*4) && (brp&0x3) ) ||
        ( (brp > 256*4) && (brp <= 256*8) && (brp&0x7) ) ||
        (brp > 256*8)
        )
			continue;

		error = baud - core_hz / (brp * (1 + tseg / 2));
		if (error < 0) {
			error = -error;
		}

		if (error <= best_error) {
			best_error = error;
			best_tseg = tseg/2;
			best_brp = brp-1;
		}
	}

	if (best_error && (baud / best_error < 10)) {
		return -2;
	}else if ( !timing )
		return 0; /* nothing to store result in, but a valid bitrate can be calculated */

	tseg2 = best_tseg - (sampl_pt * (best_tseg + 1)) / 100;

	if (tseg2 < MIN_TSEG2) {
		tseg2 = MIN_TSEG2;
	}

	if (tseg2 > MAX_TSEG2) {
		tseg2 = MAX_TSEG2;
	}

	tseg1 = best_tseg - tseg2 - 2;

	if (tseg1 > MAX_TSEG1) {
		tseg1 = MAX_TSEG1;
		tseg2 = best_tseg - tseg1 - 2;
	}
  
  /* Get scaler and BRP from pseudo BRP */
  if ( best_brp <= 256 ){
    timing->scaler = best_brp;
    timing->bpr = 0;
  }else if ( best_brp <= 256*2 ){
    timing->scaler = ((best_brp+1)>>1) -1;
    timing->bpr = 1;
  }else if ( best_brp <= 256*4 ){
    timing->scaler = ((best_brp+1)>>2) -1;
    timing->bpr = 2;
  }else{
    timing->scaler = ((best_brp+1)>>3) -1;
    timing->bpr = 3;
  }
  
	timing->ps1    = tseg1+1;
	timing->ps2    = tseg2;
	timing->rsj    = sjw;

	return 0;
}

static int grcan_hw_read_try(
  grcan_priv *pDev,
  struct grcan_regs *regs,
  CANMsg *buffer, 
  int max
  )
{
  int i,j;
  CANMsg *dest;
  grcan_msg *source,tmp;
  unsigned int wp,rp,size,rxmax,addr;
  int trunk_msg_cnt;
  
  FUNCDBG();
  
  wp = READ_REG(&regs->rx0wr);
  rp = READ_REG(&regs->rx0rd);
  DBG("wp\trp\n %d\t%d\n", wp, rp);
  
  /*
   * Due to hardware wrap around simplification write pointer will 
   * never reach the read pointer, at least a gap of 8 bytes.
   * The only time they are equal is when the read pointer has
   * reached the write pointer (empty buffer)
   *
   */
  if ( wp != rp ){
    /* Not empty, we have received chars...
     * Read as much as possible from DMA buffer
     */
    size = READ_REG(&regs->rx0size);
    
    /* Get number of bytes available in RX buffer */
    trunk_msg_cnt = grcan_hw_rxavail(rp,wp,size);
    DBG("%d bytes to be read\n", trunk_msg_cnt);
    
    /* truncate size if user space buffer hasn't room for 
     * all received chars.
     */
    if ( trunk_msg_cnt > max )
      trunk_msg_cnt = max;
    
    /* Read until i is 0 */
    i=trunk_msg_cnt;
    
    addr = (unsigned int)pDev->rx;
    source = (grcan_msg *)(addr + rp);
    dest = buffer;
    rxmax = addr + (size-GRCAN_MSG_SIZE);
    
    /* Read as many can messages as possible */
    DBG("Trying to read a message\n");
    while(i>0){
		/* Read CAN message from DMA buffer */
		tmp.head[0] = READ_DMA_WORD(&source->head[0]);
		tmp.head[1] = READ_DMA_WORD(&source->head[1]);
		if (tmp.head[1] & 0x4) {
			DBGC(DBG_RX, "overrun\n");
		}
		if (tmp.head[1] & 0x2) {
			DBGC(DBG_RX, "bus-off mode\n");
		}
		if (tmp.head[1] & 0x1) {
			DBGC(DBG_RX, "error-passive mode\n");
		}
		/* Convert one grcan CAN message to one "software" CAN message */
		dest->extended = tmp.head[0]>>31;
		dest->rtr = (tmp.head[0] >>30) & 0x1;
		if ( dest->extended ){
			dest->id = tmp.head[0] & 0x3fffffff;
		}else{
			dest->id = (tmp.head[0] >>18) & 0xfff;
		}
		dest->len = tmp.head[1] >> 28;
		for(j=0; j<dest->len; j++)
			dest->data[j] = READ_DMA_BYTE(&source->data[j]);

		/* wrap around if neccessary */
		source = ((unsigned int)source >= rxmax) ? (grcan_msg *)addr : source + 1;
		dest++; /* straight user buffer */
		i--;
    }
	{
		/* A bus off interrupt may have occured after checking pDev->started */
	//			SPIN_IRQFLAGS(oldLevel);

	//			SPIN_LOCK_IRQ(&pDev->devlock, oldLevel);
		if (pDev->started == STATE_STARTED) {
			regs->rx0rd = (unsigned int) source - addr;
			regs->rx0ctrl = GRCAN_RXCTRL_ENABLE;
		} else {
			DBGC(DBG_STATE, "cancelled due to a BUS OFF error\n");
			trunk_msg_cnt = state2err[pDev->started];
		}
//			SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);
	}
    return trunk_msg_cnt;
  }
  return 0;
}

static int grcan_hw_write_try(
  grcan_priv *pDev,
  struct grcan_regs *regs,
  CANMsg *buffer,
  int count
  )
{
	unsigned int rp, wp, size, txmax;
	uint32_t addr;
	int ret;
	grcan_msg *dest;
	CANMsg *source;
	int space_left;
	unsigned int tmp;
	int i;

//	DBGC(DBG_TX,"\n");
	FUNCDBG();

	rp = READ_REG(&regs->tx0rd);
	wp = READ_REG(&regs->tx0wr);
	size = READ_REG(&regs->tx0size);

	space_left = grcan_hw_txspace(rp,wp,size);
	/* is circular fifo full? */
	if ( space_left < 1 ){
		DBG("Space on hardware buffer %d\n", space_left);
		return 0;
	}

	/* Truncate size */
	if ( space_left > count )
		space_left = count;
	ret = space_left;

	/* We have to get the tx physical address */
	addr = (unsigned int)pDev->tx;

	dest = (grcan_msg *)(addr + wp);
	source = (CANMsg *)buffer;
	txmax = addr + (size-GRCAN_MSG_SIZE);

	DBG("Trying to send a message\n");
	while ( space_left>0 ) {
		/* Convert and write CAN message to DMA buffer */
		if ( source->extended ){
		  tmp = (1<<31) | (source->id & 0x3fffffff);
		}else{
		  tmp = (source->id&0xfff)<<18;
		}
		if ( source->rtr )
		  tmp|=(1<<30);
		dest->head[0] = tmp;
		dest->head[1] = source->len<<28;
		iop_debug("data: ");
		for ( i=0; i<source->len; i++){
			dest->data[i] = source->data[i];
			iop_debug("%c", source->data[i]);
		}
		iop_debug(" [end]\n");
		source++; /* straight user buffer */
		dest =
			((unsigned int)dest >= txmax) ?
			(grcan_msg *)addr : dest + 1;
		space_left--;
		DBG("Space left %d\n", space_left);
	}
	{
		/* A bus off interrupt may have occured after checking pDev->started */
//		SPIN_IRQFLAGS(oldLevel);

//		SPIN_LOCK_IRQ(&pDev->devlock, oldLevel);
		if (pDev->started == STATE_STARTED) {
			regs->tx0wr = (unsigned int) dest - addr;
			regs->tx0ctrl = GRCAN_TXCTRL_ENABLE;
		} else {
			DBGC(DBG_STATE, "cancelled due to a BUS OFF error\n");
			ret = state2err[pDev->started];
		}
//		SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);
	}
	return ret;
}

static int grcan_wait_rxdata(grcan_priv *pDev, int min)
{
  unsigned int wp, rp, size, irq;
  unsigned int irq_trunk, dataavail;
  int wait, state;
//	SPIN_IRQFLAGS(oldLevel);
  
  FUNCDBG();

	/*** block until receive IRQ received
	* Set up a valid IRQ point so that an IRQ is received
	* when one or more messages are received
	*/
	//	SPIN_LOCK_IRQ(&pDev->devlock, oldLevel);
	state = pDev->started;

	/* A bus off interrupt may have occured after checking pDev->started */
	if (state != STATE_STARTED) {
	//		SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);
		if (state == STATE_BUSOFF) {
			DBGC(DBG_STATE, "cancelled due to a BUS OFF error\n");
		} else if (state == STATE_AHBERR) {
			DBGC(DBG_STATE, "cancelled due to a AHB error\n");
		} else {
			DBGC(DBG_STATE, "cancelled due to STOP (unexpected) \n");
		}
		return state2err[state];
	}
    
	size = READ_REG(&pDev->regs->rx0size);
	rp = READ_REG(&pDev->regs->rx0rd);
	wp = READ_REG(&pDev->regs->rx0wr);

	/**** Calculate IRQ Pointer ****/
	irq = wp + min*GRCAN_MSG_SIZE;
	/* wrap irq around */
	if ( irq >= size ){
		irq_trunk = irq-size;
	}else
	irq_trunk = irq;

	/* init IRQ HW */
	pDev->regs->rx0irq = irq_trunk;

	/* Clear pending Rx IRQ */
	pDev->regs->picr = GRCAN_RXIRQ_IRQ;

	wp = READ_REG(&pDev->regs->rx0wr);

	/* Calculate messages available */
	dataavail = grcan_hw_rxavail(rp,wp,size);

	if ( dataavail < min ){
		/* Still empty, proceed with sleep - Turn on IRQ (unmask irq) */
		pDev->regs->imr = READ_REG(&pDev->regs->imr) | GRCAN_RXIRQ_IRQ;
		wait=1;
	}else{
		/* enough message has been received, abort sleep - don't unmask interrupt */
		wait=0;
	}
	//	SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);

	/* Wait for IRQ to fire only if has been triggered */
	if ( wait ){
		rtems_semaphore_obtain(pDev->rx_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		/*
		 * The semaphore is released either due to the expected IRQ
		 * condition or by BUSOFF, AHBERROR or another thread calling
		 * grcan_stop(). In either case, state2err[] has the correnct
		 * return value.
		 */
		return state2err[pDev->started];
	}
  
  return 0;
}

/* Wait for TX circular buffer to have room for min CAN messagges. TXIRQ is used to pin
 * point the location of the CAN message corresponding to min.
 * 
 * min must be at least WRAP_AROUND_TX_MSGS less than max buffer capacity
 * (pDev->txbuf_size/GRCAN_MSG_SIZE) for this algo to work.
 */
static int grcan_wait_txspace(grcan_priv *pDev, int min)
{
	int wait, state;
  unsigned int irq, rp, wp, size, space_left;
  unsigned int irq_trunk;
  
  DBGC(DBG_TX,"\n");
  /*FUNCDBG();*/
  
	state = pDev->started;
	/* A bus off interrupt may have occured after checking pDev->started */
	if (state != STATE_STARTED) {
		if (state == STATE_BUSOFF) {
			DBGC(DBG_STATE, "cancelled due to a BUS OFF error\n");
		} else if (state == STATE_AHBERR) {
			DBGC(DBG_STATE, "cancelled due to a AHB error\n");
		} else {
			DBGC(DBG_STATE, "cancelled due to STOP (unexpected)\n");
		}
		return state2err[state];
	}

	pDev->regs->tx0ctrl = GRCAN_TXCTRL_ENABLE;
  
  size = READ_REG(&pDev->regs->tx0size);
  wp = READ_REG(&pDev->regs->tx0wr);  
  
  rp = READ_REG(&pDev->regs->tx0rd);
  
  /**** Calculate IRQ Pointer ****/
  irq = rp + min*GRCAN_MSG_SIZE;
  /* wrap irq around */
  if ( irq >= size ){
    irq_trunk = irq - size;
  }else
    irq_trunk = irq;
  
  /* trigger HW to do a IRQ when enough room in buffer */
  pDev->regs->tx0irq = irq_trunk;
  
  /* Clear pending Tx IRQ */
  pDev->regs->picr = GRCAN_TXIRQ_IRQ;
  
  /* One problem, if HW already gone past IRQ place the IRQ will 
   * never be received resulting in a thread hang. We check if so
   * before proceeding.
   * 
   * has the HW already gone past the IRQ generation place? 
   *  == does min fit info tx buffer?
   */
  rp = READ_REG(&pDev->regs->tx0rd);
  
  space_left = grcan_hw_txspace(rp,wp,size);
  
  if ( space_left < min ){
    /* Still too full, proceed with sleep - Turn on IRQ (unmask irq) */
    pDev->regs->imr = READ_REG(&pDev->regs->imr) | GRCAN_TXIRQ_IRQ;
    wait=1;
  }else{
    /* There are enough room in buffer, abort wait - don't unmask interrupt */
    wait=0;
  }
  
  /* Wait for IRQ to fire only if it has been triggered */
  if ( wait ){
		rtems_semaphore_obtain(pDev->tx_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		return state2err[pDev->started];
  }
  
  /* At this point the TxIRQ has been masked, we ned not to mask it */
  return 0;
}

//static int grcan_tx_flush(grcan_priv *pDev)
//{
//	int wait, state;
//  unsigned int rp, wp;
//	SPIN_IRQFLAGS(oldLevel);
//  FUNCDBG();
//
//  /* loop until all data in circular buffer has been read by hw.
//   * (write pointer != read pointer )
//   *
//   * Hardware doesn't update write pointer - we do
//   */
//	while (
//		(wp = READ_REG(&pDev->regs->tx0wr)) !=
//		(rp = READ_REG(&pDev->regs->tx0rd))
//	) {
//    /* Wait for TX empty IRQ */
//		SPIN_LOCK_IRQ(&pDev->devlock, oldLevel);
//		state = pDev->started;
//
//		/* A bus off interrupt may have occured after checking pDev->started */
//		if (state != STATE_STARTED) {
//			SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);
//			if (state == STATE_BUSOFF) {
//				DBGC(DBG_STATE, "cancelled due to a BUS OFF error\n");
//			} else if (state == STATE_AHBERR) {
//				DBGC(DBG_STATE, "cancelled due to a AHB error\n");
//			} else {
//				DBGC(DBG_STATE, "cancelled due to STOP (unexpected)\n");
//			}
//			return state2err[state];
//		}
//
//    /* Clear pending TXEmpty IRQ */
//    pDev->regs->picr = GRCAN_TXEMPTY_IRQ;
//
//    if ( wp != READ_REG(&pDev->regs->tx0rd) ) {
//      /* Still not empty, proceed with sleep - Turn on IRQ (unmask irq) */
//			pDev->regs->imr =
//			    READ_REG(&pDev->regs->imr) | GRCAN_TXEMPTY_IRQ;
//      wait = 1;
//    }else{
//      /* TX fifo is empty */
//      wait = 0;
//    }
//		SPIN_UNLOCK_IRQ(&pDev->devlock, oldLevel);
//    if ( !wait )
//      break;
//
//    /* Wait for IRQ to wake us */
//		rtems_semaphore_obtain(pDev->txempty_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
//		state = pDev->started;
//		if (state != STATE_STARTED) {
//			return state2err[state];
//    }
//  }
//  return 0;
//}

int grcan_dev_count(void)
{
	return grcan_count;
}

void print_grtiming(unsigned int corefreq, unsigned int rate, struct grcan_timing *timing)
{
  unsigned int baud;
  double err;
  iop_debug("---------- %d bits/s ----------\n",corefreq);
  iop_debug("SCALER: 0x%x\n",timing->scaler);
  iop_debug("PS1:    0x%x\n",timing->ps1);
  iop_debug("PS2:    0x%x\n",timing->ps2);
  iop_debug("RSJ:    0x%x\n",timing->rsj);
  iop_debug("BPR:    0x%x\n",timing->bpr);

  baud = corefreq/(timing->scaler+1);
  baud = baud/(1<<timing->bpr);
  baud = baud/(2+timing->ps1+timing->ps2);
  iop_debug("Baud:   %d\n",baud);
  iop_debug("Sampl:  %d%%\n",(100*(1+timing->ps1))/(2+timing->ps1+timing->ps2));
  err = baud;
  err = err/rate;
  err = err*100-100; /* to % */
  iop_debug("Err:    %f%%\n",err);
}

/*
 * 1. Get the CANBUS register's address
 * 2. Obtain the clock speed
 * 3. Reset the CAN core
 * 4. Create the driver's semaphores
 * */

int grcan_device_init(iop_device_driver_t *iop_dev)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv*) (device->dev.driver);
	struct ambapp_core *pnpinfo;
	amba_apb_device grcandev;
	int dev_count = 0;

	/* Get device information from AMBA PnP information */
	/* GMVS find the bus on which the device is */

	FUNCDBG();
	/*
	 * The can Cores correspond to the 5 device on the gr740 */
	memset(&grcandev, 0, sizeof(amba_apb_device));
	dev_count = amba_find_apbslv(&amba_conf,
			VENDOR_GAISLER,
			GAISLER_GRCAN,
			&grcandev);
	if(amba_find_next_apbslv(&amba_conf,
			grcan_ids[0].vendor,
			grcan_ids[0].device,
			&grcandev,
			5))
//	if(dev_count < 1)
	{
		DBG("amba_find_apbslv did not work, %d devices found.\n", dev_count);
		return -1;
	}
	DBG("amba_find_apbslv found %d devices\n", dev_count);
	iop_debug("Start: 0x%04x - IRQ 0x%04x - bus_id 0x%04x\n",
			grcandev.start,
			grcandev.irq,
			grcandev.bus_id);

//	pnpinfo = &ambadev->info;
//	pDev->irq = pnpinfo->irq;
//			pDev->irq = ambadev.irq;
	pDev-> irq = 1090543196;
//	pDev->regs = (struct grcan_regs *)pnpinfo->apb_slv->start;
//	pDev->regs = (struct grcan_regs *)(ambadev.start); // ERROR on this line
	pDev->regs = (struct grcan_regs *) 0xffa01000; //HARDCODED gmvs
//	pDev->minor = pDev->dev->minor_drv;
	pDev->minor = device->can_core;
	DBG ("REGS: 0x%04x - IRQ: 0x%04x - Minor: %d\n", pDev->regs, pDev->irq, pDev->minor);

	/* Get frequency in Hz */
	//~ if ( drvmgr_freq_get(pDev->dev, DEV_APB_SLV, &pDev->corefreq_hz) ) {
		//~ return -1;
	//~ }
	/* Hard coded for lack of a beter way */
	pDev->corefreq_hz = CAN_CLOCK_SPEED;
	DBG("GRCAN frequency: %d Hz\n", pDev->corefreq_hz);

	/* Reset Hardware before attaching IRQ handler */
	grcan_hw_reset(pDev->regs);
//	iop_debug("GRCAN: System frequency set at: %dHz\n", pDev->corefreq_hz);
	DBG("Registers are set\n");
	/* TODO compute grcan_timing from clock frequency and configured baudrate
	 * from iop config */

	/* RX Semaphore created with count = 0 */
	if ( rtems_semaphore_create(rtems_build_name('G', 'C', 'R', '0' + pDev->minor),
		0,
		RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|\
		RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING,
		0,
		&(pDev->rx_sem)) != RTEMS_SUCCESSFUL ) {
		DBG("rx_sem couldn't be created\n");
		return RTEMS_INTERNAL_ERROR;
	}
	DBG("rx_sem successful with ID: %d\n", (int)pDev->rx_sem);
	/* TX Semaphore created with count = 0 */

	if(rtems_semaphore_create(rtems_build_name('G', 'C', 'T', '0' + pDev->minor),
			0,
			RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|\
			RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING,
			0,
			&(pDev->tx_sem)) != RTEMS_SUCCESSFUL){
		DBG("tx_sem couldn't be created\n");
		return RTEMS_INTERNAL_ERROR;
	}
	DBG("tx_sem successful with ID: %d\n", (int) pDev->tx_sem);
	/* TX Empty Semaphore created with count = 0 */
	if ( rtems_semaphore_create(rtems_build_name('G', 'C', 'E', '0' + pDev->minor),
		0,
		RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|\
		RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING,
		0,
		&(pDev->txempty_sem)) != RTEMS_SUCCESSFUL ) {
		DBG("txempty_sem couldn't be created\n");
		return RTEMS_INTERNAL_ERROR;
	}
	DBG("txempty_sem successful with ID: %d\n", (int) pDev->txempty_sem);
//	DBG("Sempahore address: 0x%04x", pDev->txempty_sem);
	/* Device Semaphore created with count = 1 */
	if ( rtems_semaphore_create(rtems_build_name('G', 'C', 'A', '0' + pDev->minor),
		1,
		RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|\
		RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING,
		0,
		&(pDev->dev_sem)) != RTEMS_SUCCESSFUL ) {
		DBG("dev_sem couldn't be created\n");
		return RTEMS_INTERNAL_ERROR;
	}
	DBG("dev_sem created with ID: %d\n", (int) pDev->dev_sem);
//	DBG("Semaphore address: 0x%04x\n", &(pDev->dev_sem));
	return 0;
}

/*
 * New function developed for AIR */
iop_device_operation grcan_initialize(iop_device_driver_t *iop_dev, void *arg){

	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv*) (device->dev.driver);

	FUNCDBG();

//	DBG("GRCAN[%d] on bus %s\n", pDev->minor_drv, pDev->parent->dev->name);
	// File system name should be configured in the iop_can_physical components
	DBG("Can core %d\n", device->can_core);

	if( grcan_device_init(iop_dev) == RTEMS_INTERNAL_ERROR){
		// Couldn't initialize the device
		DBG("Internal error on grcan_device_init\n");
		return RTEMS_IO_ERROR;
	}

	return RTEMS_SUCCESSFUL;
}

/*
 * Internal driver open routine. This corresponds more or less to the
 * original grcan_open on grcan driver from gaisler.
 */
iop_device_operation grcan_open_internal(iop_device_driver_t *iop_dev, void *arg)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev =  (grcan_priv *) (device->dev.driver);
	void *ret;

	FUNCDBG();
	DBG("CAN core %d\n", device->can_core);

	/* Wait until we get semaphore */
	DBG("Attempting to take semaphore ID: %d\n", pDev->dev_sem);
	if (rtems_semaphore_obtain(pDev->dev_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL) {
		DBG("Failed to obtain the semaphore. Returning...\n");
		return NULL;
	}

	DBG("Semaphore taken\n");

	/* is device busy/taken? */
	if  ( pDev->open ) {
		DBG("Device taken\n");
		ret = NULL;
		goto out;
	}

	/* Mark device taken */
	pDev->open = 1;

	pDev->txblock = pDev->rxblock = 0;
	pDev->txcomplete = pDev->rxcomplete = 0;
	pDev->started = STATE_STOPPED;
	pDev->config_changed = 1;
	pDev->config.silent = 0;
	pDev->config.abort = 0;
	pDev->config.selection.selection = 0;
	pDev->config.selection.enable0 = 0;
	pDev->config.selection.enable1 = 1;
	pDev->flushing = 0;
	pDev->txbuf_adr = 0;
	pDev->rxbuf_adr = 0;
	pDev->txbuf_size = TX_BUF_SIZE;
	pDev->rxbuf_size = RX_BUF_SIZE;

	/* Convert the unligned buffers into aligned buffers */
	pDev->tx = (grcan_msg *) (((unsigned int) pDev->_tx +
			(BUFFER_ALIGNMENT_NEEDS-1)) &
			~(BUFFER_ALIGNMENT_NEEDS-1));
	pDev->rx = (grcan_msg *) (((unsigned int) pDev->_rx +
			(BUFFER_ALIGNMENT_NEEDS-1)) &
			~(BUFFER_ALIGNMENT_NEEDS-1));
	DBG("Using rxbufsize: %d, txbufsize: %d\n",pDev->txbuf_size,pDev->rxbuf_size);

	grcan_set_afilter(iop_dev, &(pDev->afilter));
	grcan_set_sfilter(iop_dev, &(pDev->sfilter));
	/* Calculate default timing register values */
//	if(grcan_calc_timing(GRCAN_DEFAULT_BAUD,
//			pDev->corefreq_hz,
//			GRCAN_SAMPLING_POINT,
//			&(pDev->config.timing))){
//		DBG("Failed to calc timings\n");
//	}

	memset(&pDev->stats,0,sizeof(struct grcan_stats));
	memset(pDev->_tx, 0x0f, 3072);

	ret = pDev;
	out:
	rtems_semaphore_release(pDev->dev_sem);
	return ret;
}

/*
 * Open routine for AIR
 * */
iop_device_operation grcan_open(iop_device_driver_t *iop_dev, void *arg){

	FUNCDBG();

	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	/* Select CAN channel */
	/* Hardcoded for channel A */
	/* TODO think about channel selection */
	struct grcan_selection selection = \
	{
		.selection = 0,
		.enable0 = 0,
		.enable1 = 1,
	};

	int ret;

	if(grcan_open_internal(iop_dev, NULL) == NULL){
		DBG("Failed to open GRCAN device %d\n", device->can_core);
		return RTEMS_IO_ERROR;
	}

	/* Set /* Set up CAN driver:
		 *  ¤ Calculate grcan_timing
		 *  ¤ baud rate
		 *  ¤ Channel
		 *  ¤ Clear statistics
		 *  ¤ TX blocking, and wait for all data to be sent.
		 *  ¤ RX non-blocking depending on ONE_TAboa
		 *  SK mode
		 */

	/* Set baud */
	ret = grcan_set_speed(iop_dev, device->baud_rate);
	if(ret){
		iop_debug("GRCAN%d: Failed to set speed. Error %d\n", device->can_core, ret);
		return RTEMS_IO_ERROR;
	}else{
		print_grtiming(pDev->corefreq_hz,device->baud_rate, &(pDev->config.timing));
	}

	if(grcan_set_selection(iop_dev, &selection)){
		iop_debug("GRCAN%d: Failed to select channel.\n", device->can_core);
		return RTEMS_IO_ERROR;
	}
	iop_debug("GRCAN%d: Channel selected.\n", device->can_core);
	if(grcan_clr_stats(iop_dev)){
		iop_debug("GRCAN%d: Failed to clear statistics.\n", device->can_core);
		return RTEMS_IO_ERROR;
	}
	iop_debug("GRCAN%d: Stats cleared.\n", device->can_core);

	/* Start GRCAN driver */
	if(grcan_start(iop_dev)){
		iop_debug("GRCAN%d: Failed to start", device->can_core);
		return RTEMS_IO_ERROR;
	}
	iop_debug("GRCAN%d: Device started", device->can_core);

	return RTEMS_SUCCESSFUL;
}

iop_device_operation grcan_close(iop_device_driver_t * iop_dev)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);
  
	FUNCDBG();

	grcan_stop(iop_dev);

	grcan_hw_reset(pDev->regs);

	//~ grcan_free_buffers(pDev,1,1);

	/* Mark Device as closed */
	pDev->open = 0;

	return RTEMS_SUCCESSFUL;
}

iop_device_operation grcan_read(iop_device_driver_t *iop_dev, void *arg)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);
	CANMsg dest;
	unsigned int count, left;
	int nread;
	int req_cnt;
	int i;
	can_header_t header;

	/* get IOP buffer */
	iop_wrapper_t *wrapper = (iop_wrapper_t *) arg;
	iop_buffer_t *iop_buffer = wrapper->buffer;

	FUNCDBG();

	/* Pointer to where the messages is going */
//	dest = msg;
//	dest = (CANMsg *)iop_buffer->v_addr;
	req_cnt = 1; /* Process one message at the time */

	DBG("grcan_read on device grcan%d\n", device->can_core);
	if ( (!iop_buffer) || (req_cnt<1) ){
		DBG("Invalid argument\n");
		return GRCAN_RET_INVARG;
	}

	if (pDev->started != STATE_STARTED) {
		DBG("Trying to read a device not started\n");
		return GRCAN_RET_NOTSTARTED;
	}
	DBG("Attempting to read %d message(s)\n", req_cnt);
	nread = grcan_hw_read_try(pDev,pDev->regs,&dest,req_cnt);
	if (nread < 0) {
		return nread;
	}

	DBG("%d message(s) read\n", nread);
	if(nread > 0){
		iop_debug("ID: %d Extendend: %d RTR: %d\n",
				dest.id,
				dest.extended,
				dest.rtr);
		for( i = 0; i< dest.len; i++){
			iop_debug("%c", dest.data[i]);
		}
	}
	iop_debug("\n");

	/* Complete the iop_buffer */
	iop_buffer->payload_off = iop_buffer->header_size = sizeof(can_header_t);
	iop_buffer->header_off = iop_buffer->header_size - sizeof(can_header_t);
	iop_buffer->payload_size = dest.len;
	header.id = (int) dest.id;
	header.extended = (int) dest.extended;
	header.rtr = (int) dest.rtr;
	header.sshot = 0;
	iop_debug("ID: %d Extendend: %d RTR: %d\n",
		header.id,
		header.extended,
		header.rtr);
	memcpy(iop_buffer->v_addr + iop_buffer->header_off,
			&header,
			iop_buffer->header_size);
	memcpy(iop_buffer->v_addr + iop_buffer->payload_off,
			dest.data,
			iop_buffer->payload_size);

	count = nread;
	if ( !( pDev->rxblock && pDev->rxcomplete && (count!=req_cnt) ) ){
		if ( count > 0 ) {
			/* Successfully received messages (at least one) */
			iop_debug("%d messages read\n", count);
			return RTEMS_SUCCESSFUL;
		}

		/* nothing read, shall we block? */
		if ( !pDev->rxblock ) {
			/* non-blocking mode */
			return GRCAN_RET_TIMEOUT;
		}
	}

//	while(count == 0 || (pDev->rxcomplete && (count>=req_cnt)) ){
//		if ( !pDev->rxcomplete ){
//		  left = 1; /* return as soon as there is one message available */
//		}else{
//			left = req_cnt - count;     /* return as soon as all data are available */
//
//			/* never wait for more than the half the maximum size of the receive buffer
//			* Why? We need some time to copy buffer before to catch up with hw,
//			* otherwise we would have to copy everything when the data has been
//			* received.
//			*/
//			if ( left > ((pDev->rxbuf_size/GRCAN_MSG_SIZE)/2) ){
//				left = (pDev->rxbuf_size/GRCAN_MSG_SIZE)/2;
//			}
//		}
//
//		nread = grcan_wait_rxdata(pDev, left);
//		if (nread) {
//			/* The wait has been aborted, probably due to
//			* the device driver has been closed by another
//			* thread or a bus-off. Return error code.
//			*/
//			return nread;
//		}
//
//		/* Try read bytes from circular buffer */
//		nread = grcan_hw_read_try(
//		  pDev,
//		  pDev->regs,
//		  dest+count,
//		  req_cnt-count);
//
//		if (nread < 0) {
//			/* The read was aborted by bus-off. */
//			return nread;
//		}
//		count += nread;
//	}
	/* no need to unmask IRQ as IRQ Handler do that for us. */
//	return count;
	return RTEMS_SUCCESSFUL;
}
 
iop_device_operation grcan_write(iop_device_driver_t *iop_dev, void *arg)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);
	CANMsg source;
	unsigned int count, left;
	int nwritten;
	int req_cnt;
	int i;
	char *tmp;
	can_header_t *can_header;

	/* check proper length and buffer pointer */
	if(arg == NULL){
		return GRCAN_RET_INVARG;
	}

	iop_wrapper_t *wrapper = (iop_wrapper_t *) arg;
	iop_buffer_t *iop_buffer = wrapper->buffer;

	if(iop_buffer->v_addr == NULL){
		DBG("Bad iop_buffer\n");
		return RTEMS_INVALID_NAME;
	}

	DBGC(DBG_TX,"\n");

	if ((pDev->started != STATE_STARTED) || pDev->config.silent || pDev->flushing)
		return GRCAN_RET_NOTSTARTED;

	req_cnt = 1;

	/* Setup the the can message in the CANMsg format */
	can_header = (can_header_t *) (iop_buffer->v_addr + iop_buffer->header_off);
	DBG("From iop_buffers\nSource ID: %d RTR: %d Ext: %d\n",
			can_header->id,
			can_header->rtr,
			can_header->extended);
	source.id = can_header->id;
	source.rtr = can_header->rtr;
	source.extended = can_header->extended;
	DBG("Message:\n");
	for(i = 0 ; i < iop_buffer->payload_size; i++){
		tmp = (char *) (iop_buffer->v_addr + iop_buffer->payload_off + i);
		source.data[i] = *tmp;
		iop_debug("%c", source.data[i]);
	}
	iop_debug("\n");

//	source.extended = 0; //hardcoded for testing TODO remove this
	source.len = iop_buffer->payload_size;
	DBG("Payload size %d\n", source.len);

	nwritten = grcan_hw_write_try(pDev,pDev->regs,&source,req_cnt);
	if (nwritten < 0) {
		return nwritten;
	}
	count = nwritten;
	if ( !(pDev->txblock && pDev->txcomplete && (count!=req_cnt)) ) {
		if ( count > 0 ) {
		  /* Successfully transmitted chars (at least one char) */
			DBG("Successfully transmitted %d chars\n", count);
//			return count;
			return RTEMS_SUCCESSFUL;
		}

		/* nothing written, shall we block? */
		if ( !pDev->txblock ) {
		  /* non-blocking mode */
		  return GRCAN_RET_TIMEOUT;
		}
	}

	/* if in txcomplete mode we need to transmit all chars */
//	while((count == 0) || (pDev->txcomplete && (count!=req_cnt)) ){
//		/*** block until room to fit all or as much of transmit buffer as possible
//		* IRQ comes. Set up a valid IRQ point so that an IRQ is received
//		* when we can put a chunk of data into transmit fifo
//		*/
//		if ( !pDev->txcomplete ){
//		  left = 1; /* wait for anything to fit buffer */
//		}else{
//		  left = req_cnt - count; /* wait for all data to fit in buffer */
//
//				/* never wait for more than the half the maximum size of the transmit
//				 * buffer
//		   * Why? We need some time to fill buffer before hw catches up.
//		   */
//		  if ( left > ((pDev->txbuf_size/GRCAN_MSG_SIZE)/2) ){
//			left = (pDev->txbuf_size/GRCAN_MSG_SIZE)/2;
//		  }
//		}
//
//		nwritten = grcan_wait_txspace(pDev,left);
//		/* Wait until more room in transmit buffer */
//		if ( nwritten ) {
//		  /* The wait has been aborted, probably due to
//		   * the device driver has been closed by another
//		   * thread. To avoid deadlock we return directly
//		   * with error status.
//		   */
//			return nwritten;
//		}
//
//		/* Try read bytes from circular buffer */
//		nwritten = grcan_hw_write_try(
//		  pDev,
//		  pDev->regs,
//		  source+count,
//		  req_cnt-count);
//
//		if (nwritten < 0) {
//			/* Write was aborted by bus-off. */
//			return nwritten;
//		}
//		count += nwritten;
//	}
	/* no need to unmask IRQ as IRQ Handler do that for us. */

//	return count;
	return RTEMS_SUCCESSFUL;
}
    
int grcan_stop(iop_device_driver_t *iop_dev)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *)(device->dev.driver);
	int do_sw_stop;

	FUNCDBG();
    
	if (pDev->started == STATE_STOPPED)
		return -1;

	if (pDev->started == STATE_STARTED) {
		grcan_hw_stop(pDev);
		do_sw_stop = 1;
		DBGC(DBG_STATE, "STARTED->STOPPED\n");
	} else {
		/*
		 * started == STATE_[STOPPED|BUSOFF|AHBERR] so grcan_hw_stop()
		 * might already been called from ISR.
		 */
		DBGC(DBG_STATE, "[STOPPED|BUSOFF|AHBERR]->STOPPED\n");
		do_sw_stop = 0;
	}
	pDev->started = STATE_STOPPED;

	if (do_sw_stop)
		grcan_sw_stop(pDev);

	return 0;
}
			
int grcan_set_silent(iop_device_driver_t *iop_dev, int silent)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	FUNCDBG();

	if (pDev->started == STATE_STARTED)
		return -1;

	pDev->config.silent = silent;
      pDev->config_changed = 1;

	return 0;
}

int grcan_set_abort(iop_device_driver_t *iop_dev, int abort)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	FUNCDBG();

	if (pDev->started == STATE_STARTED)
		return -1;

	pDev->config.abort = abort;
	/* This Configuration parameter doesn't need HurriCANe reset
	* ==> no pDev->config_changed = 1;
	*/
    
	return 0;
}

int grcan_clr_stats(iop_device_driver_t *iop_dev)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	FUNCDBG();

    memset(&pDev->stats,0,sizeof(struct grcan_stats));

	return 0;
}

int grcan_set_selection(iop_device_driver_t *iop_dev, const struct grcan_selection *selection)
{
	iop_can_device_t * device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	FUNCDBG();

	if (pDev->started == STATE_STARTED)
		return -1;

      if ( !selection )
		return -2;
      
      pDev->config.selection = *selection;
      pDev->config_changed = 1;
       
	return 0;
}
      
int grcan_set_rxblock(iop_device_driver_t *iop_dev, int block)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);
    
	FUNCDBG();

	pDev->rxblock = block;
          
	return 0;
}

int grcan_set_txblock(iop_device_driver_t *iop_dev, int block)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	FUNCDBG();

	pDev->txblock = block;

	return 0;
}

int grcan_set_txcomplete(iop_device_driver_t *iop_dev, int complete)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	FUNCDBG();

	pDev->txcomplete = complete;

	return 0;
}

int grcan_set_rxcomplete(iop_device_driver_t *iop_dev, int complete)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	FUNCDBG();

	pDev->rxcomplete = complete;

	return 0;
}

int grcan_get_stats(iop_device_driver_t *iop_dev, struct grcan_stats *stats)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	FUNCDBG();

	if ( !stats )
		return -1;

    *stats = pDev->stats;
    
	return 0;
}

int grcan_set_speed(iop_device_driver_t *iop_dev, unsigned int speed)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);
	struct grcan_timing timing;
	int ret;

	FUNCDBG();
		
	/* cannot change speed during run mode */
	if (pDev->started == STATE_STARTED)
		return -1;
			
	/* get speed rate from argument */
	ret = grcan_calc_timing(speed*1000,pDev->corefreq_hz,GRCAN_SAMPLING_POINT,&timing);
	if ( ret )
		return -2;
			
	/* save timing/speed */
	pDev->config.timing = timing;
	pDev->config_changed = 1;
    
	return 0;
}

int grcan_set_btrs(iop_device_driver_t *iop_dev, const struct grcan_timing *timing)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	FUNCDBG();

	/* Set BTR registers manually
	 * Read GRCAN/HurriCANe Manual.
	 */
	if (pDev->started == STATE_STARTED)
		return -1;
        
	if ( !timing )
		return -2;
      
	pDev->config.timing.bpr = timing->bpr;
	pDev->config.timing.ps1 = timing->ps1;
	pDev->config.timing.ps2 = timing->ps2;
	pDev->config.timing.rsj = timing->rsj;
	pDev->config.timing.scaler = timing->scaler;
    pDev->config_changed = 1;
    
	return 0;
}

int grcan_set_afilter(iop_device_driver_t *iop_dev, const struct grcan_filter *filter)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	FUNCDBG();

	if ( !filter ){
	/* Disable filtering - let all messages pass */
	pDev->afilter.mask = 0x0;
	pDev->afilter.code = 0x0;
	}else{
	/* Save filter */
	pDev->afilter = *filter;
	}
	/* Set hardware acceptance filter */
	grcan_hw_accept(pDev->regs,&pDev->afilter);

	return 0;
}

int grcan_set_sfilter(iop_device_driver_t *iop_dev, const struct grcan_filter *filter)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);
//	SPIN_IRQFLAGS(oldLevel);

	FUNCDBG();

      if ( !filter ){
        /* disable TX/RX SYNC filtering */
        pDev->sfilter.mask = 0xffffffff; 
        pDev->sfilter.mask = 0;
        
        /* disable Sync interrupt */
        pDev->regs->imr = READ_REG(&pDev->regs->imr) & ~(GRCAN_RXSYNC_IRQ|GRCAN_TXSYNC_IRQ);
      }else{
        /* Save filter */
        pDev->sfilter = *filter;
        
        /* Enable Sync interrupt */
        pDev->regs->imr = READ_REG(&pDev->regs->imr) | (GRCAN_RXSYNC_IRQ|GRCAN_TXSYNC_IRQ);
      }
      /* Set Sync RX/TX filter */
      grcan_hw_sync(pDev->regs,&pDev->sfilter);

	return 0;
}

/* Error indicators */
#define GRCAN_IRQ_ERRORS \
		(GRCAN_RXAHBERR_IRQ | GRCAN_TXAHBERR_IRQ | GRCAN_OFF_IRQ)
#define GRCAN_STAT_ERRORS (GRCAN_STAT_AHBERR | GRCAN_STAT_OFF)
/* Warning & RX/TX sync indicators */
#define GRCAN_IRQ_WARNS \
		(GRCAN_ERR_IRQ | GRCAN_OR_IRQ | GRCAN_TXLOSS_IRQ | \
		 GRCAN_RXSYNC_IRQ | GRCAN_TXSYNC_IRQ)
#define GRCAN_STAT_WARNS (GRCAN_STAT_OR | GRCAN_STAT_PASS)
