/*
 * GRCAN driver
 *
 * COPYRIGHT (c) 2007  Cobham Gaisler AB
 * Copyright (C) 2018  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE.RTEMS in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */
/**
 * @file
 * @brief The original driver has been modified to be included in the
 * Hypervisor AIR. It is assumed to be used for the GR740 board.
 *
 * @author gmvs
 */
#include <grcan.h>
#include <bsp.h>

#define WRAP_AROUND_TX_MSGS 1
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

#ifndef GRCAN_SAMPLING_POINT
#define GRCAN_SAMPLING_POINT 80
#endif


static int state2err[4] = {
	/* STATE_STOPPED */ GRCAN_RET_NOTSTARTED,
	/* STATE_STARTED */ GRCAN_RET_OK,
	/* STATE_BUSOFF  */ GRCAN_RET_BUSOFF,
	/* STATE_AHBERR  */ GRCAN_RET_AHBERR
};

int iop_grcan_get_state(iop_device_driver_t *iop_dev){
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

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

int iop_grcan_get_status(iop_device_driver_t *iop_dev, unsigned int *data)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev->driver;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	if ( !data )
		return -1;

	/* Read out the statsu register from the GRCAN core */
	data[0] = READ_REG(&pDev->regs->stat);

	return 0;
}

static void __inline__ grcan_hw_reset(struct grcan_regs *regs)
{
	regs->ctrl = GRCAN_CTRL_RESET;
}

static air_status_code_e grcan_hw_start(grcan_priv *pDev)
{
	/*
	 * tmp is set but never used. GCC gives a warning for this
	 * and we need to tell GCC not to complain.
	 */
	unsigned int tmp;

	/* Check that memory has been allocated successfully */
	if(pDev->tx == NULL || pDev->rx == NULL){
		iop_debug("No memory allocated\n");
		return AIR_OUT_OF_MEMORY;
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
			(unsigned int) air_syscall_get_physical_addr((air_uptr_t)pDev->rx);
	pDev->regs->rx0size = pDev->rxbuf_size;

	/* Setup Transmitter */
	pDev->regs->tx0addr =
			(unsigned int) air_syscall_get_physical_addr((air_uptr_t)pDev->tx);
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
	return AIR_SUCCESSFUL;
}

static void grcan_hw_stop(grcan_priv *pDev)
{
	/* Mask all IRQs */
	pDev->regs->imr = 0;

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
#if 0
	rtems_semaphore_release(pDev->rx_sem);
	rtems_semaphore_release(pDev->tx_sem);
	rtems_semaphore_release(pDev->txempty_sem);
#endif
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
  iop_debug("ARGS\nwp\trp\tsize\n%d\t%d\t%d \n", wp, rp, size);
  if ( rp == wp ) {
    /* read pointer and write pointer is equal only 
     * when TX buffer is empty.
     */
    return size/GRCAN_MSG_SIZE-WRAP_AROUND_TX_MSGS;
  }

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

int iop_grcan_start(iop_device_driver_t *iop_dev)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	if (iop_grcan_get_state(iop_dev) == STATE_STARTED) {
		return -1;
	}

	if ( (grcan_hw_start(pDev)) != AIR_SUCCESSFUL ){
		iop_debug("Hardware start unsuccessful\n");
		return -2;
	}

	/* Clear semaphore state. This is to avoid effects from previous
	 * bus-off/stop where semaphores where flushed() but the count remained.
	 */
#if 0
	rtems_semaphore_obtain(pDev->rx_sem, RTEMS_NO_WAIT, 0);
	rtems_semaphore_obtain(pDev->tx_sem, RTEMS_NO_WAIT, 0);
	rtems_semaphore_obtain(pDev->txempty_sem, RTEMS_NO_WAIT, 0);
#endif
	/* Read and write are now open... */
	pDev->started = STATE_STARTED;

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

  wp = READ_REG(&regs->rx0wr);
  rp = READ_REG(&regs->rx0rd);
  iop_debug("wp\trp\n %d\t%d\n", wp, rp);

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

	while(i>0){
		/* Read CAN message from DMA buffer */
		tmp.head[0] = READ_DMA_WORD(&source->head[0]);
		tmp.head[1] = READ_DMA_WORD(&source->head[1]);
		if (tmp.head[1] & 0x4) {
			iop_debug("overrun\n");
		}
		if (tmp.head[1] & 0x2) {
			iop_debug("bus-off mode\n");
		}
		if (tmp.head[1] & 0x1) {
			iop_debug("error-passive mode\n");
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
		if (pDev->started == STATE_STARTED) {
			regs->rx0rd = (unsigned int) source - addr;
			regs->rx0ctrl = GRCAN_RXCTRL_ENABLE;
		} else {
			iop_debug("cancelled due to a BUS OFF error\n");
			trunk_msg_cnt = state2err[pDev->started];
		}
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
	unsigned int addr;
	int ret;
	grcan_msg *dest;
	CANMsg *source;
	int space_left;
	unsigned int tmp;
	int i;

	rp = READ_REG(&regs->tx0rd);
	wp = READ_REG(&regs->tx0wr);
	size = READ_REG(&regs->tx0size);

	space_left = grcan_hw_txspace(rp,wp,size);
	/* is circular fifo full? */
	if ( space_left < 1 ){
		iop_debug("Space on hardware buffer %d\n", space_left);
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

		for ( i=0; i<source->len; i++){
			dest->data[i] = source->data[i];
		}

		source++; /* straight user buffer */
		dest =
			((unsigned int)dest >= txmax) ?
			(grcan_msg *)addr : dest + 1;
		space_left--;
		iop_debug("Space left %d\n", space_left);
	}
	{
		/* A bus off interrupt may have occured after checking pDev->started */
		if (pDev->started == STATE_STARTED) {
			regs->tx0wr = (unsigned int) dest - addr;
			regs->tx0ctrl = GRCAN_TXCTRL_ENABLE;
		} else {
			iop_debug("cancelled due to a BUS OFF error\n");
			ret = state2err[pDev->started];
		}
	}
	return ret;
}

/*
 * 1. Get the CANBUS register's address
 * 2. Obtain the clock speed
 * 3. Reset the CAN core
 * 4. Create the driver's semaphores
 * */

int iop_grcan_device_init(iop_device_driver_t *iop_dev)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv*) (device->dev.driver);
	amba_apb_dev_t grcandev;
	amba_apb_dev_t gptimer;
	timer_regmap_t *tregs;

	amba_confarea_t * ambabus = (amba_confarea_t *)air_syscall_get_ambaconf();

	/* Enable CAN Clock gate */
	clock_gating_enable(ambabus, GATE_CAN);

	memset(&grcandev, 0, sizeof(amba_apb_dev_t));

	if(!amba_get_apb_slave(ambabus, VENDOR_GAISLER, GAISLER_GRCAN, device->can_core, &grcandev))
		return AIR_DEVICE_NOT_FOUND;

	pDev->irq = grcandev.irq;
	pDev->regs = (struct grcan_regs *) (grcandev.start);
	pDev->minor = device->can_core;

	/*
	 * Auto Detect the CAN core frequency by assuming that the system frequency is
	 * is the same as the CAN core frequency.
	 */
	if (amba_get_apb_slave(ambabus,VENDOR_GAISLER,GAISLER_GPTIMER,0,&gptimer))
	{
		/*Timer memory mapped registers*/
		tregs = (timer_regmap_t*)gptimer.start;

		/*Calculate System frequency based on the timer register*/
		pDev->corefreq_hz = (tregs->scaler_reload+1)*1000000;
	}
	else
	{
		pDev->corefreq_hz = 250000000;
		iop_debug("GRCAN: Failed to detect system frequency\n\r");
	}

	/* Reset Hardware*/
	grcan_hw_reset(pDev->regs);

#if 0
	/* RX Semaphore created with count = 0 */
	if ( rtems_semaphore_create(rtems_build_name('G', 'C', 'R', '0' + pDev->minor),
		0,
		RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|\
		RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING,
		0,
		&(pDev->rx_sem)) != AIR_SUCCESSFUL ) {
		DBG("rx_sem couldn't be created\n");
		return AIR_OUT_OF_MEMORY;
	}
	DBG("rx_sem successful with ID: %d\n", (int)pDev->rx_sem);
	/* TX Semaphore created with count = 0 */

	if(rtems_semaphore_create(rtems_build_name('G', 'C', 'T', '0' + pDev->minor),
			0,
			RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|\
			RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING,
			0,
			&(pDev->tx_sem)) != AIR_SUCCESSFUL){
		DBG("tx_sem couldn't be created\n");
		return AIR_OUT_OF_MEMORY;
	}
	DBG("tx_sem successful with ID: %d\n", (int) pDev->tx_sem);
	/* TX Empty Semaphore created with count = 0 */
	if ( rtems_semaphore_create(rtems_build_name('G', 'C', 'E', '0' + pDev->minor),
		0,
		RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|\
		RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING,
		0,
		&(pDev->txempty_sem)) != AIR_SUCCESSFUL ) {
		DBG("txempty_sem couldn't be created\n");
		return AIR_OUT_OF_MEMORY;
	}
	DBG("txempty_sem successful with ID: %d\n", (int) pDev->txempty_sem);
	/* Device Semaphore created with count = 1 */
	if ( rtems_semaphore_create(rtems_build_name('G', 'C', 'A', '0' + pDev->minor),
		1,
		RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|\
		RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING,
		0,
		&(pDev->dev_sem)) != AIR_SUCCESSFUL ) {
		DBG("dev_sem couldn't be created\n");
		return AIR_OUT_OF_MEMORY;
	}
	DBG("dev_sem created with ID: %d\n", (int) pDev->dev_sem);
#endif
	return AIR_SUCCESSFUL;
}

/*
 * New function developed for AIR */
uint32_t iop_grcan_initialize(iop_device_driver_t *iop_dev, void *arg){

	if(AIR_SUCCESSFUL != iop_grcan_device_init(iop_dev)){
		// Couldn't initialize the device
		iop_debug("Internal error on grcan_device_init\n");
		return AIR_DEVICE_ERROR;
	}

	return AIR_SUCCESSFUL;
}

/*
 * Internal driver open routine. This corresponds more or less to the
 * original grcan_open on grcan driver from gaisler.
 */
air_status_code_e iop_grcan_open_internal(iop_device_driver_t *iop_dev, void *arg)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev =  (grcan_priv *) (device->dev.driver);

#if 0
	/* Wait until we get semaphore */
	DBG("Attempting to take semaphore ID: %d\n", pDev->dev_sem);
	if (rtems_semaphore_obtain(pDev->dev_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= AIR_SUCCESSFUL) {
		DBG("Failed to obtain the semaphore. Returning...\n");
		return AIR_DEVICE_ERROR;
	}

	DBG("Semaphore taken\n");
#endif

	pDev->txblock = pDev->rxblock = 0;
	pDev->txcomplete = pDev->rxcomplete = 0;
	pDev->started = STATE_STOPPED;
	pDev->config_changed = 1;
	pDev->config.silent = 0;
	pDev->config.abort = 0;
	pDev->flushing = 0;
	pDev->txbuf_adr = 0;
	pDev->rxbuf_adr = 0;
	/* If not configured by AIR use default*/
	if(pDev->txbuf_size  == 0){
		pDev->txbuf_size = TX_BUF_SIZE;
	}
	if(pDev->rxbuf_size == 0){
		pDev->rxbuf_size = RX_BUF_SIZE;
	}

	/* Convert the unaligned buffers into aligned buffers */
	/* AIR message _rx and _tx initialy point to same memory address*/
	pDev->tx = (grcan_msg *) (((unsigned int) pDev->_tx +
			(BUFFER_ALIGNMENT_NEEDS-1)) &
			~(BUFFER_ALIGNMENT_NEEDS-1));
	pDev->rx = (grcan_msg *) ((((unsigned int) pDev->_rx +
			(BUFFER_ALIGNMENT_NEEDS-1)) &
			~(BUFFER_ALIGNMENT_NEEDS-1)) + pDev->txbuf_size);

	iop_grcan_set_afilter(iop_dev, &(pDev->afilter));
	iop_grcan_set_sfilter(iop_dev, &(pDev->sfilter));

	memset(&pDev->stats,0,sizeof(struct grcan_stats));

#if 0
	rtems_semaphore_release(pDev->dev_sem);
#endif
	return AIR_SUCCESSFUL;
}

/*
 * Open routine for AIR
 * */
uint32_t iop_grcan_open(iop_device_driver_t *iop_dev, void *arg){

	iop_can_device_t *device = (iop_can_device_t *) iop_dev;

	int ret;

	if(iop_grcan_open_internal(iop_dev, NULL) != AIR_SUCCESSFUL){
		iop_debug("Failed to open GRCAN device %d\n", device->can_core);
		return AIR_DEVICE_ERROR;
	}

	/* Set up CAN driver:
	 *  ¤ Calculate grcan_timing
	 *  ¤ baud rate
	 *  ¤ Channel
	 *  ¤ Clear statistics
	 *  ¤ TX blocking, and wait for all data to be sent.
	 *  ¤ RX non-blocking depending on ONE_TA
	 *  SK mode
	 */

	/* Set baud */
	ret = iop_grcan_set_speed(iop_dev, device->baud_rate);
	if(ret){
		iop_debug("GRCAN%d: Failed to set speed. Error %d\n", device->can_core, ret);
		return AIR_DEVICE_ERROR;
	}

	if(iop_grcan_clr_stats(iop_dev)){
		iop_debug("GRCAN%d: Failed to clear statistics.\n", device->can_core);
		return AIR_DEVICE_ERROR;
	}

	/* Start GRCAN driver */
	if(iop_grcan_start(iop_dev)){
		iop_debug("GRCAN%d: Failed to start", device->can_core);
		return AIR_DEVICE_ERROR;
	}

	return AIR_SUCCESSFUL;
}

uint32_t iop_grcan_close(iop_device_driver_t * iop_dev, void *arg)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	iop_grcan_stop(iop_dev);

	grcan_hw_reset(pDev->regs);

	//~ grcan_free_buffers(pDev,1,1);

	return AIR_SUCCESSFUL;
}

uint32_t iop_grcan_read(iop_device_driver_t *iop_dev, void *arg)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);
	CANMsg dest;
	unsigned int count;
	int nread;
	int req_cnt;
	can_header_t header;

	if(arg == NULL)
		return AIR_INVALID_PARAM;

	/* get IOP buffer */
	iop_wrapper_t *wrapper = (iop_wrapper_t *) arg;
	iop_buffer_t *iop_buffer = wrapper->buffer;

	/* Pointer to where the messages is going */
	req_cnt = 1; /* Process one message at the time */

	if ( (!iop_buffer) || (req_cnt<1) )
		return AIR_INVALID_SIZE;

	if (pDev->started != STATE_STARTED)
		return AIR_INVALID_MODE;

	nread = grcan_hw_read_try(pDev,pDev->regs,&dest,req_cnt);

	if (nread < 0) {
		return nread;
	}

	count = nread;
	if ( !( pDev->rxblock && pDev->rxcomplete && (count!=req_cnt) ) ){

		if ( count > 0 ) {
			/* Successfully received messages*/
			/* Complete the iop_buffer */
			iop_buffer->payload_off = iop_buffer->header_size = sizeof(can_header_t);
			iop_buffer->header_off = iop_buffer->header_size - sizeof(can_header_t);
			iop_buffer->payload_size = dest.len;

			header.id = (int) dest.id;
			header.extended = (int) dest.extended;
			header.rtr = (int) dest.rtr;
			header.sshot = 0;

			memcpy(get_header(iop_buffer),
					&header,
					get_header_size(iop_buffer));

			memcpy(get_payload(iop_buffer),
					dest.data,
					get_payload_size(iop_buffer));

			return AIR_SUCCESSFUL;
		}

		/* nothing read, shall we block? */
		if ( !pDev->rxblock ) {
			/* non-blocking mode */
			return AIR_TIMED_OUT;
		}
	}

	return AIR_SUCCESSFUL;
}
 
uint32_t iop_grcan_write(iop_device_driver_t *iop_dev, void *arg)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);
	CANMsg source;
	unsigned int count;
	int nwritten;
	int req_cnt;
	int i;
	char *tmp;
	can_header_t *can_header;

	/* check proper length and buffer pointer */
	if(arg == NULL)
		return AIR_INVALID_PARAM;

	iop_wrapper_t *wrapper = (iop_wrapper_t *) arg;
	iop_buffer_t *iop_buffer = wrapper->buffer;

	if(iop_buffer->v_addr == NULL){
		iop_debug("iop_grcan_write error iop_buffer\n");
		return AIR_INVALID_SIZE;
	}

	if ((pDev->started != STATE_STARTED) || pDev->config.silent || pDev->flushing)
		return AIR_INVALID_MODE;

	req_cnt = 1;

	/* Setup the the can message in the CANMsg format */
	can_header = (can_header_t *) get_header(iop_buffer);

	source.id = can_header->id;
	source.rtr = can_header->rtr;
	source.extended = can_header->extended;

	for(i = 0 ; i < get_payload_size(iop_buffer); i++){
		tmp = (char *) (get_payload(iop_buffer) + i);
		source.data[i] = *tmp;
	}

	source.len = get_payload_size(iop_buffer);

	nwritten = grcan_hw_write_try(pDev,pDev->regs,&source,req_cnt);
	if (nwritten < 0) {
		return nwritten;
	}
	count = nwritten;
	if ( !(pDev->txblock && pDev->txcomplete && (count!=req_cnt)) ) {
		if ( count > 0 ) {
		  /* Successfully transmitted chars (at least one char) */
			iop_debug("Successfully transmitted %d chars\n", count);
			return AIR_SUCCESSFUL;
		}

		/* nothing written, shall we block? */
		if ( !pDev->txblock ) {
		  /* non-blocking mode */
		  return AIR_TIMED_OUT;
		}
	}

	return AIR_SUCCESSFUL;
}

int iop_grcan_stop(iop_device_driver_t *iop_dev)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *)(device->dev.driver);
	int do_sw_stop;

	if (pDev->started == STATE_STOPPED)
		return -1;

	if (pDev->started == STATE_STARTED) {
		grcan_hw_stop(pDev);
		do_sw_stop = 1;
	} else {
		/*
		 * started == STATE_[STOPPED|BUSOFF|AHBERR] so grcan_hw_stop()
		 * might already been called from ISR.
		 */
		do_sw_stop = 0;
	}
	pDev->started = STATE_STOPPED;

	if (do_sw_stop)
		grcan_sw_stop(pDev);

	return 0;
}
			
int iop_grcan_set_silent(iop_device_driver_t *iop_dev, int silent)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	if (pDev->started == STATE_STARTED)
		return -1;

	pDev->config.silent = silent;
      pDev->config_changed = 1;

	return 0;
}

int iop_grcan_set_abort(iop_device_driver_t *iop_dev, int abort)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	if (pDev->started == STATE_STARTED)
		return -1;

	pDev->config.abort = abort;
	/* This Configuration parameter doesn't need HurriCANe reset
	* ==> no pDev->config_changed = 1;
	*/

	return 0;
}

int iop_grcan_clr_stats(iop_device_driver_t *iop_dev)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	memset(&pDev->stats,0,sizeof(struct grcan_stats));

	return 0;
}

int iop_grcan_set_selection(iop_device_driver_t *iop_dev, const struct grcan_selection *selection)
{
	iop_can_device_t * device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	if (pDev->started == STATE_STARTED)
		return -1;

      if ( !selection )
		return -2;

      pDev->config.selection = *selection;
      pDev->config_changed = 1;

	return 0;
}

int iop_grcan_set_rxblock(iop_device_driver_t *iop_dev, int block)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	pDev->rxblock = block;

	return 0;
}

int iop_grcan_set_txblock(iop_device_driver_t *iop_dev, int block)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	pDev->txblock = block;

	return 0;
}

int iop_grcan_set_txcomplete(iop_device_driver_t *iop_dev, int complete)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	pDev->txcomplete = complete;

	return 0;
}

int iop_grcan_set_rxcomplete(iop_device_driver_t *iop_dev, int complete)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	pDev->rxcomplete = complete;

	return 0;
}

int iop_grcan_get_stats(iop_device_driver_t *iop_dev, struct grcan_stats *stats)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

	if ( !stats )
		return -1;

	*stats = pDev->stats;

	return 0;
}

int iop_grcan_set_speed(iop_device_driver_t *iop_dev, unsigned int speed)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);
	struct grcan_timing timing;
	int ret;

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

int iop_grcan_set_btrs(iop_device_driver_t *iop_dev, const struct grcan_timing *timing)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

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

int iop_grcan_set_afilter(iop_device_driver_t *iop_dev, const struct grcan_filter *filter)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);

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

int iop_grcan_set_sfilter(iop_device_driver_t *iop_dev, const struct grcan_filter *filter)
{
	iop_can_device_t *device = (iop_can_device_t *) iop_dev;
	grcan_priv *pDev = (grcan_priv *) (device->dev.driver);
//	SPIN_IRQFLAGS(oldLevel);

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
