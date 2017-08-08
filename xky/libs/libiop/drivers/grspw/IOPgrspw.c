/**
 *  @file 
 *  @brief This file contains the interruptless SpaceWire Driver for LEON3.
 *
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT
 *
 *  @author Cl�udio Silva
 *	
 *
 */
 
/* 
 *  COPYRIGHT (c) 2007
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 */

#ifndef SPW_PREFIX
 #define SPW_PREFIX(name) spw##name
#endif

/* default to no translation */
#ifndef SPW_ADR_TO
 #define memarea_to_hw(x) ((unsigned int)(x))
#endif
#ifndef SPW_ADR_FROM
 #define hw_to_memarea(x) ((unsigned int)(x))
#endif

#define SPW_DONT_BYPASS_CACHE

#define GRSPW_SUPPORTED

#include <bsp.h>
#include <xky.h>
#include <rtems.h>
#include <string.h>
#include <amba.h>
#include <ambapp.h>
#include <ambaext.h>

#include <pprintf.h>
#include <IOPlibio.h>
#include <IOPgrspw.h>
#include <spw_support.h>
#include <IOPdriverconfig_interface.h>

#define DBGSPW_IOCALLS 1
#define DBGSPW_TX 2
#define DBGSPW_RX 4
#define DBGSPW_IOCTRL 1
#define DBGSPW_DUMP 16
#define DEBUG_SPACEWIRE_FLAGS (DBGSPW_IOCALLS | DBGSPW_TX | DBGSPW_RX )

/* #define DEBUG_SPACEWIRE_ONOFF */
 
#ifdef DEBUG_SPACEWIRE_ONOFF
#define SPACEWIRE_DBG(fmt, args...)    do { { pprintf(" : %03d @ %18s()]:" fmt , __LINE__,__FUNCTION__,## args); }} while(0)
#define SPACEWIRE_DBG2(fmt)            do { { pprintf(" : %03d @ %18s()]:" fmt , __LINE__,__FUNCTION__); }} while(0)
#define SPACEWIRE_DBGC(c,fmt, args...) do { if (DEBUG_SPACEWIRE_FLAGS & c) { pprintf(" : %03d @ %18s()]:" fmt , __LINE__,__FUNCTION__,## args);  }} while(0)
#else
#define SPACEWIRE_DBG(fmt, args...)
#define SPACEWIRE_DBG2(fmt, args...)
#define SPACEWIRE_DBGC(c, fmt, args...)
#endif

/** The Descriptor table has 1024bytes in size and a 1024 aligmment */
#define SPACEWIRE_BDTABLE_ALIGMENT 0x400

/** returns the first address found after p that is aligned with c  */
#define SPW_ALIGN(p,c) ((((unsigned int)(p))+(c)) & ~((c)-1))

#define CPU_SPARC_HAS_SNOOPING 0      /*TODO*/

/*Number of SpW cores*/
static int spw_cores;

/*Number of SpW2 cores */
static int spw_cores2;

/*Number of SpW2 with DMA cores */
static int spw_cores2_dma;

/*Counter with the number of ports already initialized */
static int cores_init = 0;

/*used to create semaphore names*/
static char c = 'a';

/*Number of user configured cores*/
static int configured_cores = 0;

/* System Frequency*/
static unsigned int sys_freq_khz;

/* Pointer to user configuration*/
static spw_user_config *user_config;

/* Pointer to the default configuration*/
static spw_user_config *defconf;

/* Pointer to amba bus structure*/
static amba_confarea_type *amba_bus;

#ifdef SPW_DONT_BYPASS_CACHE
#define _SPW_READ(address) (*(volatile unsigned int *)(address))
#define _MEM_READ(address) (*(volatile unsigned char *)(address))
#else

static unsigned int _SPW_READ(void *addr) {
	unsigned int tmp;
	__asm__ __volatile__ (" lda [%1]1, %0 "
							: "=r"(tmp)
							: "r"(addr)
						  );
	return tmp;
}

static unsigned int _MEM_READ(void *addr) {
	unsigned int tmp;
	__asm__ __volatile__ (" lduba [%1]1, %0 "
							: "=r"(tmp)
							: "r"(addr)
					     );
	return tmp;        

}
#endif

static int spw_hw_init(SPW_DEV *pDev);
static int spw_hw_send(SPW_DEV *pDev, unsigned int hlen, char *hdr, unsigned int dlen, char *data);
static int spw_hw_receive(SPW_DEV *pDev, char *hdr, char *b, int c);
static int spw_hw_startup (SPW_DEV *pDev, int timeout);
static int spw_hw_stop (SPW_DEV *pDev, int rx, int tx);
static int spw_hw_waitlink (SPW_DEV *pDev, int timeout);
static void spw_hw_reset(SPW_DEV *pDev);
static void spw_hw_sync_config(SPW_DEV *pDev);
static void spw_hw_handle_errors(SPW_DEV *pDev);

static void check_rx_errors(SPW_DEV *pDev, int ctrl);
static void spw_rxnext(SPW_DEV *pDev);
static void spw_check_tx(SPW_DEV *pDev);

int spw_get_conf_size(void);


/**
 * @brief Calculates the system frequency based on the Timer
 * 
 */
void set_sys_freq(){

    /* Get System clock frequency */
	sys_freq_khz = 0;
		
	/* 
	 * Auto Detect the SPW core frequency by assuming that the system frequency is
	 * is the same as the SPW core frequency.
	 */
	#ifndef SYS_FREQ_KHZ
	#ifdef LEON3
		/* LEON3: find timer address via AMBA Plug&Play info */	
		{
			amba_apb_device gptimer;
			LEON3_Timer_Regs_Map *tregs;
			
			/*search for gaisler timer in the amba bus*/
			if ( amba_find_apbslv(&amba_conf,VENDOR_GAISLER,GAISLER_GPTIMER,&gptimer) == 1 ){
				
				/*Timer memory mapped registers*/
				tregs = (LEON3_Timer_Regs_Map *)gptimer.start;
				
				/*Calculate System frequency based on the timer register*/
				sys_freq_khz = (tregs->scaler_reload+1)*1000;
				
			}else{
				/*Use default frequency:40Mhz*/
				sys_freq_khz = 40000; 
				pprintf("SPW: Failed to detect system frequency\n\r");
			}
			
		}
	#else
		#error CPU not supported by SPW driver
	#endif
	#else
		/* Use hardcoded frequency */
		sys_freq_khz = SYS_FREQ_KHZ;
	#endif        

}

/** 
 * @brief Get a value at least 6.4us in number of clock cycles 
 * @param [in] freq_khz system frequency in KHz
 * @return Number of clock cycles needed to make 6.4us
 */
static unsigned int spw_calc_timer64(int freq_khz){
	unsigned int timer64 = (freq_khz*64+9999)/10000;
	return timer64 & 0xfff;
}

/**
 * @brief Get a value at least 850ns in number of clock cycles - 3 
 * @param [in] freq_khz system frequency in KHz
 * @return Number of clock cycles needed to make 6.4us
 */
static unsigned int spw_calc_disconnect(int freq_khz){
	unsigned int disconnect = ((freq_khz*85+99999)/100000) - 3;
	return disconnect & 0x3ff;
}

/**
 * @brief Verifys if any error occured
 * @param [in] pDev device's internal structure
 *
 * This function reads the status and dmactrl registers and checks them for 
 * errors. Link errors are reported to FDIR if link_err_irq is selected on the
 * configuration. Every error is added to the statistics.
 */
static void spw_hw_handle_errors(SPW_DEV *pDev){
	/*DMA control register contents*/
	int dmactrl;
	
	/*status register contents*/
	int status;
	
	/*read status resgiter*/
	status = SPW_STATUS_READ(pDev);
	
	/*Clear status flags in the status register*/
	SPW_STATUS_WRITE(pDev, SPW_STATUS_CE | SPW_STATUS_ER | SPW_STATUS_DE | SPW_STATUS_PE | SPW_STATUS_WE | SPW_STATUS_IA | SPW_STATUS_EE);
	
	/*Read DMA control register*/
	dmactrl = SPW_READ(&pDev->regs->dma0ctrl);
	
	/*TODO: Clear DMA control status register*/
	SPW_WRITE(&pDev->regs->dma0ctrl, dmactrl | SPW_DMACTRL_PR);
	
	/* If linkinterrupts are enabled check if it was a linkerror irq and then send an event to the 
	   process set in the config */
	if (pDev->config.link_err_irq) {
		
		/*If a error was signaled*/
		if (status & (SPW_STATUS_CE | SPW_STATUS_ER | SPW_STATUS_DE | SPW_STATUS_PE | SPW_STATUS_WE)) {
			
			/*Send an event to the error handler task TODO: replace by rtems-edi error handler*/
			rtems_event_send(pDev->config.event_id, SPW_LINKERR_EVENT);
			
			/*If the "disable SpW link on error" option is activated*/
			if (pDev->config.disable_err) {
					
					/* disable link*/
					SPW_CTRL_WRITE(pDev, (SPW_CTRL_READ(pDev) & 0xFFFFFFFC) | SPW_CTRL_LINKDISABLED);
					
					/*HW has stopped until further notice*/
					pDev->config.linkdisabled = 1;
					pDev->config.linkstart = 0;
					pDev->running = 0;
			}
		}
	}
	
	/*Account any found errors on the device's statistics*/
	if (status & SPW_STATUS_CE) {
			pDev->stat.credit_err++;
	}
	if (status & SPW_STATUS_ER) {
			pDev->stat.escape_err++;
	}
	if (status & SPW_STATUS_DE) {
			pDev->stat.disconnect_err++;
	}
	if (status & SPW_STATUS_PE) {
			pDev->stat.parity_err++;
	}
	if (status & SPW_STATUS_WE) {
			pDev->stat.write_sync_err++;
	}
	if (status & SPW_STATUS_IA) {
			pDev->stat.invalid_address++;
	}
	if (status & SPW_STATUS_EE) {
			pDev->stat.early_ep++;
	}
	
	return;

}


/**
 * @brief Verifies if data that was previously marked to be sent, was really sent
 * @param [in] pDev device's internal structure
 *
 * spw_check_tx() verifies if previously enabled TX descriptors were sent. If 
 * there was an error while sending the data, the write operation is retried
 * and the error reported.
 */
static void spw_check_tx(SPW_DEV *pDev){
	/*Control Register Contents*/
	int ctrl;
	
	/*DMA Control Register Contents*/
    unsigned int dmactrl;
	
	/* Check for descriptors whose data has been sent.*/
	while( (pDev->tx_sent != pDev->tx_cur) || pDev->tx_all_in_use) {
		/* Has data on this descriptor been sent? */
		
		/*Read control register*/
		ctrl = SPW_READ((volatile void *)&pDev->tx[pDev->tx_sent].ctrl);
		
		/*Is the current descriptor enabled*/
		if ( ctrl & SPW_TXBD_EN ) {
			
			/*The descriptor is enabled so its data was not yet sent*/
			break;
		}
		
		/*data on the current desc was sent so increment status counters*/
		pDev->stat.packets_sent++;
		
		/*Verify link errors*/
		if ( ctrl & SPW_TXBD_LE ) {
		
			/*Increment link error count*/
			pDev->stat.tx_link_err++;
			
			/*Should we retry to send the data?*/
			if (pDev->config.retry) {
				
				/*Is this the last descriptor on the table?*/
				if (pDev->tx_sent == (pDev->txbufcnt - 1) ) {
					
					/*Re-enable last descriptor with WRAP*/
					pDev->tx[pDev->tx_sent].ctrl = SPW_TXBD_WR | SPW_TXBD_EN | pDev->tx[pDev->tx_sent].ctrl;
				
				} else {
				
					/*re-enable descriptor*/
					pDev->tx[pDev->tx_sent].ctrl = SPW_TXBD_EN | pDev->tx[pDev->tx_sent].ctrl;
				}
				
				/*Read DMA control register*/
				dmactrl = SPW_READ(&pDev->regs->dma0ctrl);
				
				/*Enable TX*/
				SPW_WRITE(&pDev->regs->dma0ctrl, (dmactrl & SPW_PREPAREMASK_TX) | SPW_DMACTRL_TXEN);
			}
		}
				
		/* step to next descriptor */
		pDev->tx_sent = (pDev->tx_sent + 1) % pDev->txbufcnt;
		
		/* not all of the descriptors can be in use since we just freed one*/
		pDev->tx_all_in_use = 0; 
	}
	
	return;
}

/** 
 *  \brief Initializes the SPW cores
 *
 *  \param [in]  major : not used
 *  \param [in]  minor : not used
 *  \param [in]  arg : not used. 
 *
 *  \return Status of the operation:
 *     - RTEMS_TOO_MANY No SpW cores found
 *     - RTEMS_NO_MEMORY It was not possible to allocate memory for the device
 *     - RTEMS_SUCESSFULL operation completed successfully
 *	
 **/
rtems_device_driver spw_initialize(iop_device_driver_t *iop_dev, void *arg)
{
	
	iop_spw_device_t *device = (iop_spw_device_t *)iop_dev;
	SPW_DEV *pDev = (SPW_DEV *)(device->dev.driver);
	
	/*configuration that is being implemented*/
	spw_user_config *config;
	
	/*Amba APB device*/
	amba_apb_device dev;
	
	/*Get amba bus configuration*/
	amba_bus = &amba_conf;
	
	iop_debug("amba_conf->ahbmst: %d\n", amba_bus->ahbmst.devnr);
	iop_debug("amba_conf->ahbslv: %d\n", amba_bus->ahbslv.devnr);
	iop_debug("amba_conf->apbslv: %d\n", amba_bus->apbslv.devnr);
	
	unsigned int conf;
	int i;
	for(i = 0; i < amba_bus->ahbmst.devnr; i++)
    {
        /* get the configuration area */
        conf = amba_get_confword(amba_bus->ahbmst , i , 0);

		iop_debug("ahbmst:%d  AMBA VENDOR: 0x%x   AMBA DEV: 0x%x   conf: 0x%x\n",
					i, amba_vendor(conf), amba_device(conf), conf);
    }
	for(i = 0; i < amba_bus->ahbslv.devnr; i++)
    {
        /* get the configuration area */
        conf = amba_get_confword(amba_bus->ahbslv , i , 0);

		iop_debug("ahbslv:%d  AMBA VENDOR: 0x%x   AMBA DEV: 0x%x   conf: 0x%x\n",
					i, amba_vendor(conf), amba_device(conf), conf);
    }
	for(i = 0; i < amba_bus->apbslv.devnr; i++)
    {
        /* get the configuration area */
        conf = amba_get_confword(amba_bus->apbslv , i , 0);

		iop_debug("apbslv:%d  AMBA VENDOR: 0x%x   AMBA DEV: 0x%x   conf: 0x%x\n",
					i, amba_vendor(conf), amba_device(conf), conf);
    }
	
	iop_debug("on grspw\n");
	amba_apb_device apbdev;
	memset(&apbdev, 0, sizeof(amba_apb_device));
	int device_found = 0;
	device_found = amba_find_apbslv(amba_bus, VENDOR_GAISLER, GAISLER_GR1553B,
									&dev);
									
	if (device_found){
	    iop_debug("    MIL device found on apb...\n");
	}
	
	amba_ahb_device ahb_dev;
	memset(&ahb_dev, 0, sizeof(amba_ahb_device));
	device_found = amba_find_ahbslv(amba_bus, VENDOR_GAISLER, GAISLER_GR1553B,
									&dev);
	
	if (device_found){
	    iop_debug("    MIL device found on ahp...\n");
	}
	/* get memory for the internal structure */
	// spw_dev = get_spw_dev();
	
	spw_cores = amba_get_number_apbslv_devices(amba_bus,VENDOR_GAISLER,GAISLER_SPACEWIRE);
	spw_cores2 = amba_get_number_apbslv_devices(amba_bus,VENDOR_GAISLER,GAISLER_GRSPW2);
	spw_cores2_dma = amba_get_number_apbslv_devices(amba_bus,VENDOR_GAISLER,GAISLER_GRSPW2_DMA);
	
	iop_debug("n of spw dma cores: %d\n", spw_cores2_dma);
	/* zero out all memory */
	//memset(spw_dev,0,sizeof(SPW_DEV));
	
	/*get user configuration*/
	user_config = (spw_user_config *) &device->nodeaddr;
	
	// TODO
	/*get default configuration*/
	defconf = user_config;
	
	if (amba_find_next_apbslv(amba_bus,VENDOR_GAISLER,GAISLER_SPACEWIRE,&dev,cores_init)) {
			
		/*store core version in device's structure*/
		pDev->core_ver = 1;
	} else if (amba_find_next_apbslv(amba_bus,VENDOR_GAISLER,GAISLER_GRSPW2,&dev,cores_init)) {
		
		/*store core version in device's structure*/
		pDev->core_ver = 2;
	} else if (amba_find_next_apbslv(amba_bus,VENDOR_GAISLER,GAISLER_GRSPW2_DMA,&dev,cores_init)) {
		
		/*store core version in device's structure*/
		pDev->core_ver = 3;
	} else {
		iop_debug("    GRSPW device not found...\n");
		return RTEMS_INTERNAL_ERROR;
	}
	/* Pointer to device's memory mapped registers*/
	pDev->regs = (LEON3_SPACEWIRE_Regs_Map *)dev.start;
	
	/* store IRQ channel used by the spw device*/
	pDev->irq = dev.irq;
	
	/* store device minor number*/
	pDev->minor = cores_init;
	
	cores_init++;
	
	/* This device has not started (yet)*/
	pDev->open = 0;
	
	SPACEWIRE_DBG("spacewire core at [0x%x]\n", (unsigned int) pDev->regs);

	/* non configured cores are configured using default configuration */
	if(pDev->minor >= configured_cores)
	{
		/* This core was not configured by the user. Use defaults*/
		config = defconf;
		
	} else {
	
		/* Use user's configurations for this core*/
		config = user_config;
	}
		
	/* Configuration options defined by the user.
	 * These options don't need to be written to the Hardware
	 */
	pDev->config.rm_prot_id = config->rm_prot_id;
	pDev->config.rxmaxlen = config->rxmaxlen;
	pDev->config.retry = config->retry;
	pDev->txdbufsize = config->txdbufsize;
	pDev->txhbufsize = config->txhbufsize;
	pDev->rxbufsize = config->rxbufsize;
	
	/*Check user defined descriptor count against its limits*/
	if(config->txdesccnt > 64){	
		/*maximum number of allowed tx descriptors*/
		pDev->txbufcnt = 64;
	} else{
		pDev->txbufcnt = config->txdesccnt;
	}
	
	/*Check user defined descriptor count against its limits*/
	if(config->rxdesccnt > 128){	
		/*maximum number of allowed rx descriptors*/
		pDev->rxbufcnt = 128;
	} else{
		pDev->rxbufcnt = config->rxdesccnt;
	}
	
	/*	This set of options is defined by the partition supplier
	 *	For the moment we will let them here. If latter we see
	 *	that we need to configure this, we place these on a outside struct
	 * TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
	 */
	 
	/*Blocking Behavior*/
	pDev->config.tx_blocking = 0;
	pDev->config.tx_block_on_full = 0;
	pDev->config.rx_blocking = 0;
	
	/*Error Behavior: Currently Not Defined*/
	pDev->config.check_rmap_err = 0;
	pDev->config.disable_err = 1;
	pDev->config.link_err_irq = 0; 
	pDev->config.event_id = 0; 
	
	/*Allocate Descritors and data buffers*/
	setup_iop_buffers(pDev->iop_buffers,
					pDev->iop_buffers_storage,
					2 * pDev->txbufcnt + pDev->rxbufcnt);
					
	/*Pointers to data and header buffers*/
	pDev->ptr_rxbuf0 = (char *)pDev->iop_buffers[0].v_addr;
	pDev->ptr_txhbuf0 = (char *)pDev->iop_buffers[pDev->rxbufcnt].v_addr;
	pDev->ptr_txdbuf0 = (char *)pDev->iop_buffers[pDev->rxbufcnt + pDev->txbufcnt].v_addr;
	
	/**** Initialize Hardware and semaphores ****/
	/*Create TX semaphore*/
	rtems_semaphore_create(
			rtems_build_name('T', 'x', 'S', c), 
			0, 
			RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY | \
			RTEMS_NO_PRIORITY_CEILING, 
			0, 
			&(pDev->txsp));
			
	/*Create RX semaphore*/
	rtems_semaphore_create(
			rtems_build_name('R', 'x', 'S', c), 
			0, 
			RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY | \
			RTEMS_NO_PRIORITY_CEILING, 
			0, 
			&(pDev->rxsp));
	c++;
	
	/*Initialize Hardware*/
	spw_hw_init(pDev);
	
	/*Startup Hardware*/
	//spw_hw_startup(pDev,-1);
	
	return RTEMS_SUCCESSFUL;
}


/** 
 *  \brief prepares the device for communication
 *
 *  \param [in]  major : not used
 *  \param [in]  minor : minor number of the device to open
 *  \param [in]  arg : not used. 
 *
 *  \return Status of the operation:
 *		- RTEMS_INVALID_NUMBER if minor is invalid
 *		- RTEMS_RESOURCE_IN_USE if the device is already open
 *		- RTEMS_SUCCESSFUL if the operation completed sucessfully
 *	
 **/
rtems_device_driver spw_open(iop_device_driver_t *iop_dev, void *arg)
{		
	/*Current SpW device*/
	iop_spw_device_t *device = (iop_spw_device_t *)iop_dev;
	SPW_DEV *pDev = (SPW_DEV *)(device->dev.driver);
	//SPACEWIRE_DBGC(DBGSPW_IOCALLS, "open [%i,%i]\n", major, minor);
	
	/*Check if device exists*/
	if ( pDev->minor >= (spw_cores+spw_cores2+spw_cores2_dma) ) {
			SPACEWIRE_DBG("minor %i too big\n", pDev->minor);
			return RTEMS_INVALID_NUMBER;
	}
	
	/*Check if device was already opened*/
	if ( pDev->open )
			return RTEMS_RESOURCE_IN_USE;
	
	/* Mark device as open */
	pDev->open = 1; 
	
	/*Clear SpW device statistics*/
	pDev->stat.tx_link_err = 0;
	pDev->stat.rx_rmap_header_crc_err = 0;
	pDev->stat.rx_rmap_data_crc_err = 0;
	pDev->stat.rx_eep_err = 0;
	pDev->stat.rx_truncated = 0;
	pDev->stat.parity_err = 0;
	pDev->stat.escape_err = 0;
	pDev->stat.credit_err = 0;
	pDev->stat.write_sync_err = 0;
	pDev->stat.disconnect_err = 0;
	pDev->stat.early_ep = 0;
	pDev->stat.invalid_address = 0;
	pDev->stat.packets_sent = 0;
	pDev->stat.packets_received = 0;
	
	pDev->running = 0;
	pDev->core_freq_khz = 0;
	
	/* Reset HW Core */
	spw_hw_reset(pDev);
	
	/* Sync the core configuration with the HW*/
	spw_hw_sync_config(pDev);
	
	/*Startup Hardware*/
	spw_hw_startup(pDev,-1);
	
	pDev->running = 1;
	
	return RTEMS_SUCCESSFUL;
}


/** 
 *  \brief stops the device from communicating
 *
 *  \param [in]  major : not used
 *  \param [in]  minor : minor number of the device to close
 *  \param [in]  arg : not used
 *
 *  \return Status of the operation:
 *		- RTEMS_SUCCESSFUL if the operation completed sucessfully
 *
 **/	
rtems_device_driver spw_close(iop_device_driver_t *iop_dev, void *arg)
{   
	/*Current SpW device*/
	iop_spw_device_t *device = (iop_spw_device_t *)iop_dev;
	SPW_DEV *pDev = (SPW_DEV *)(device->dev.driver);
	
	//SPACEWIRE_DBGC(DBGSPW_IOCALLS, "close [%i,%i]\n", major, minor);
	
	/*Delete Semaphores*/
	// rtems_semaphore_delete(pDev->txsp);
	// rtems_semaphore_delete(pDev->rxsp);
	
	/*Stop HW*/
	spw_hw_stop(pDev,1,1);
	
	/*Reset HW*/
	spw_hw_reset(pDev);
	
	/* Mark device closed - not open */
	pDev->open = 0;
	
	return RTEMS_SUCCESSFUL;
}


/** 
 *  \brief reads data from the SpW core with a specific minor
 *
 *  \param [in]  major : not used
 *  \param [in]  minor : minor number of the device to read from
 *  \param [in]  arg  #libio_rw_args_t with data buffer and data length
 *
 *  \return Status of the operation:
 *		- RTEMS_SUCCESSFUL if the operation completed sucessfully
 *		- RTEMS_INVALID_NAME: Device is stopped or the user arguments are invalid
 *		- RTEMS_RESOURCE_IN_USE: There is no data to be read and we can't block
 *
 **/	
rtems_device_driver spw_read(iop_device_driver_t *iop_dev, void *arg)
{
	/*Current SpW device*/
	iop_spw_device_t *device = (iop_spw_device_t *)iop_dev;
	SPW_DEV *pDev = (SPW_DEV *)(device->dev.driver);
		
	/* get IOP buffer */
    iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;
	iop_buffer_t *iop_buffer = wrapper->buffer;
	
	/*Number of packtes received*/
	unsigned int count = 0;
	
	/* is link up? */
	if ( !pDev->running ) {
		return RTEMS_INVALID_NAME;
	}
	
	/* Verify user arguments consistency*/
	if (iop_buffer->v_addr == NULL) {
		return RTEMS_INVALID_NAME;
	}
	
	//SPACEWIRE_DBGC(DBGSPW_IOCALLS, "read  [%i,%i]: buf:0x%x len:%i \n", major, minor, (unsigned int)rw_args->data, rw_args->data_len);
	
	/*Check for errors*/
	spw_hw_handle_errors(pDev);
	
	iop_debug("INSIDE READER\n");

	/*While we have read no data*/
	while ((count = spw_hw_receive(pDev, ((uint8_t *)iop_buffer->v_addr + iop_buffer->header_off),
			((uint8_t *)iop_buffer->v_addr + iop_buffer->payload_off), iop_buffer->payload_size)) == 0) {
		
		iop_debug("RTEMS_RESOURCE_IN_USE: %d\n", count);
		/*Are we allowed to block?*/
		if (pDev->config.rx_blocking) {
			SPACEWIRE_DBG2("Rx blocking\n");
			
			/* wait a moment for any RX descriptors to get available */
			rtems_task_wake_after(pDev->config.wait_ticks);
		} else {
			SPACEWIRE_DBG2("Rx non blocking\n");
			/*We can't block waiting, so we return*/
			return RTEMS_RESOURCE_IN_USE;
		}
	}
	
	iop_debug("count: %d\n", count);
/*	
#ifdef DEBUG_SPACEWIRE_ONOFF  
	if (DEBUG_SPACEWIRE_FLAGS & DBGSPW_DUMP) {
		int k;
		for (k = 0; k < count; k++){
			if (k % 16 == 0) {
					pprintf ("\n");
			}
			pprintf ("%.2x(%c) ", rw_args->data[k] & 0xff, isprint(rw_args->data[k] & 0xff) ? rw_args->data[k] & 0xff : ' ');
		}
		pprintf ("\n");
	}
#endif
*/
//	/*Write number of bytes that were written to the user's buffer*/
//	rw_args->bytes_moved = count;
	
	/*We were able to read something, return success*/
	return RTEMS_SUCCESSFUL;    
}

/** Struct to save


/** 
 *  \brief writes data to a SpW device with a specific minor number
 *
 *  \param [in]  major : not used
 *  \param [in]  minor : minor number of the device to write to
 *  \param [in]  arg : #libio_rw_args_t with data buffer and data length
 *
 *  \return Status of the operation:
 *		- RTEMS_SUCCESSFUL if the operation completed sucessfully
 *		- RTEMS_INVALID_NAME: Device is stopped or the user arguments are invalid
 *		- RTEMS_RESOURCE_IN_USE: Write buffers are full and we can't block waiting
 *
 **/	
rtems_device_driver spw_write(iop_device_driver_t *iop_dev, void *arg)
{   
	/*Current SpW device*/
	iop_spw_device_t *device = (iop_spw_device_t *)iop_dev;
	SPW_DEV *pDev = (SPW_DEV *)(device->dev.driver);

	/* get IOP buffer */
    iop_wrapper_t *wrapper = (iop_wrapper_t *)arg;
	iop_buffer_t *iop_buffer = wrapper->buffer;
	
	//SPACEWIRE_DBGC(DBGSPW_IOCALLS, "write [%i,%i]: buf:0x%x len:%i\n", major, minor, (unsigned int)rw_args->data, rw_args->data_len);

	/* is link up? */
	if ( !pDev->running ) {
		iop_debug("dev not running?\n");
		return RTEMS_INVALID_NAME;
	}
	
	/* Check if the request size is feasible and if the buffer has data*/
	if ((iop_buffer->payload_size > pDev->txdbufsize) || (iop_buffer->payload_size < 1) || (iop_buffer->v_addr == NULL)) {
		iop_debug("the buffer hasnt data or its not feasible (too big).    ");
		iop_debug("len: %ld, data: %s, bufsize: %ld\n",	iop_buffer->payload_size, (char *)iop_buffer->v_addr, pDev->txdbufsize);
		return RTEMS_INVALID_NAME;
	}
	
	/* Check For errors*/
	spw_hw_handle_errors(pDev);
	
	/* See if new decriptors were sent*/
	spw_check_tx(pDev);
	
	/* try to send data while its possible*/
	int data_sent = 0;
	while ((data_sent = spw_hw_send(pDev, iop_buffer->header_size - iop_buffer->header_off, ((char *)iop_buffer->v_addr + iop_buffer->header_off), iop_buffer->payload_size, ((char *)iop_buffer->v_addr + iop_buffer->payload_off)) ) == 0) {
	
		/* Can we block?*/
		if (pDev->config.tx_block_on_full == 1) { 
			SPACEWIRE_DBG2("Tx Block on full \n");
			
			/* Sleep for a while*/
			rtems_task_wake_after(pDev->config.wait_ticks);
		} else {
			SPACEWIRE_DBG2("Tx non blocking return when full \n");
			
			/* we cannot block and all descriptors are used, so we have to return*/
			return RTEMS_RESOURCE_IN_USE;
		}
	}
	
	/* We were able to write*/
	return RTEMS_SUCCESSFUL;
}

//rtems_device_driver spw_control(
//        rtems_device_major_number major,
//        rtems_device_minor_number minor,
//        void                    * arg
//        )
//{
//        SPW_DEV *pDev = &spw_dev[minor];
//        spw_ioctl_pkt_send *args;
//        unsigned int tmp,nodeaddr,nodemask;
//        int timeout;
//        rtems_device_driver ret;
//        libio_ioctl_args_t	*ioarg = (libio_ioctl_args_t *) arg;
//        SPACEWIRE_DBGC(DBGSPW_IOCALLS, "ctrl [%i,%i]\n", major, minor);
//        
//        if (!ioarg)
//                return RTEMS_INVALID_NAME;
//
//		
//        ioarg->ioctl_return = 0;
//        switch(ioarg->command) {
//                case SPACEWIRE_IOCTRL_SET_NODEADDR:
//                        /*set node address*/
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "SPACEWIRE_IOCTRL_SET_NODEADDR %i\n",(unsigned int)ioarg->buffer);
//                        if ((unsigned int)ioarg->buffer > 255) {
//                                return RTEMS_INVALID_NAME;
//                        }
//                        nodeaddr = ((unsigned int)ioarg->buffer) & 0xff;
//                        tmp = SPW_READ(&pDev->regs->nodeaddr);
//                        tmp &= 0xffffff00; /* Remove old address */
//                        tmp |= nodeaddr;
//                        SPW_WRITE(&pDev->regs->nodeaddr, tmp);
//                        if ((SPW_READ(&pDev->regs->nodeaddr)&0xff) != nodeaddr) {
//                                return RTEMS_IO_ERROR;
//                        }
//                        pDev->config.nodeaddr = nodeaddr;
//                        break;
//                case SPACEWIRE_IOCTRL_SET_NODEMASK:
//                        /*set node address*/
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "SPACEWIRE_IOCTRL_SET_NODEMASK %i\n",(unsigned int)ioarg->buffer);
//                        if ( pDev->core_ver > 1 ){
//                          if ((unsigned int)ioarg->buffer > 255) {
//                                  return RTEMS_INVALID_NAME;
//                          }
//                          nodemask = ((unsigned int)ioarg->buffer) & 0xff;
//                          tmp = SPW_READ(&pDev->regs->nodeaddr);
//                          tmp &= 0xffff00ff; /* Remove old mask */
//                          tmp |= nodemask<<8;
//                          SPW_WRITE(&pDev->regs->nodeaddr, tmp);
//                          if (((SPW_READ(&pDev->regs->nodeaddr)>>8)&0xff) != nodemask) {
//                                  return RTEMS_IO_ERROR;
//                          }
//                          pDev->config.nodemask = nodemask;
//                        }else{
//                          SPACEWIRE_DBG("SPACEWIRE_IOCTRL_SET_NODEMASK: not implemented in SPW1 HW\n");
//                        }
//                        break;
//                case SPACEWIRE_IOCTRL_SET_RXBLOCK:
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "SPACEWIRE_IOCTRL_SET_RXBLOCK %i \n", (unsigned int)ioarg->buffer);
//                        if ((unsigned int)ioarg->buffer > 1) {
//                                return RTEMS_INVALID_NAME;
//                        }
//                        pDev->config.rx_blocking = (unsigned int)ioarg->buffer;
//                        break;
//                case SPACEWIRE_IOCTRL_SET_DESTKEY:
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_DESTKEY %i\n", (unsigned int)ioarg->buffer);
//                        if (!pDev->config.is_rmap) {
//                                return RTEMS_NOT_IMPLEMENTED;
//                        }
//                        if ((unsigned int)ioarg->buffer > 255) {
//                                return RTEMS_INVALID_NAME;
//                        }
//                        SPW_WRITE(&pDev->regs->destkey, (unsigned int)ioarg->buffer);
//                        if (SPW_READ(&pDev->regs->destkey) != (unsigned int)ioarg->buffer) {
//                                return RTEMS_IO_ERROR;
//                        }
//                        pDev->config.destkey = (unsigned int)ioarg->buffer;
//                        break;
//                case SPACEWIRE_IOCTRL_SET_CLKDIV:
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_CLKDIV %i\n", (unsigned int)ioarg->buffer);
//                        if ((unsigned int)ioarg->buffer > 255) {
//                                return RTEMS_INVALID_NAME;
//                        }
//                        tmp = SPW_READ(&pDev->regs->clkdiv);
//                        tmp &= ~0xff; /* Remove old Clockdiv Setting */
//                        tmp |= ((unsigned int)ioarg->buffer) & 0xff; /* add new clockdiv setting */
//                        SPW_WRITE(&pDev->regs->clkdiv, tmp);
//                        if (SPW_READ(&pDev->regs->clkdiv) != tmp) {
//                                return RTEMS_IO_ERROR;
//                        }
//                        pDev->config.clkdiv = tmp;
//                        break;
//                case SPACEWIRE_IOCTRL_SET_CLKDIVSTART:
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_CLKDIVSTART %i\n", (unsigned int)ioarg->buffer);
//                        if ((unsigned int)ioarg->buffer > 255) {
//                                return RTEMS_INVALID_NAME;
//                        }
//                        tmp = SPW_READ(&pDev->regs->clkdiv);
//                        tmp &= ~0xff00; /* Remove old Clockdiv Start Setting */
//                        tmp |= (((unsigned int)ioarg->buffer) & 0xff)<<8; /* add new clockdiv start setting */
//                        SPW_WRITE(&pDev->regs->clkdiv, tmp);
//                        if (SPW_READ(&pDev->regs->clkdiv) != tmp) {
//                                return RTEMS_IO_ERROR;
//                        }
//                        pDev->config.clkdiv = tmp;
//                        break;                        
//                case SPACEWIRE_IOCTRL_SET_TIMER:
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_TIMER %i\n", (unsigned int)ioarg->buffer);
//                        if ( pDev->core_ver <= 1 ) {
//                          if ((unsigned int)ioarg->buffer > 4095) {
//                                  return RTEMS_INVALID_NAME;
//                          }
//                          SPW_WRITE(&pDev->regs->timer, (SPW_READ(&pDev->regs->timer) & 0xFFFFF000) | ((unsigned int)ioarg->buffer & 0xFFF));
//                          if ((SPW_READ(&pDev->regs->timer) & 0xFFF) != (unsigned int)ioarg->buffer) {
//                                  return RTEMS_IO_ERROR;
//                          }
//                          pDev->config.timer = (unsigned int)ioarg->buffer;
//                        }else{
//                          SPACEWIRE_DBG("SPACEWIRE_IOCTRL_SET_TIMER: removed in SPW2 HW\n");
//                        }
//                        break;
//                case SPACEWIRE_IOCTRL_SET_DISCONNECT:
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_DISCONNECT %i\n", (unsigned int)ioarg->buffer);
//                        if ( pDev->core_ver <= 1 ) {
//                          if ((unsigned int)ioarg->buffer > 1023) {
//                                  return RTEMS_INVALID_NAME;
//                          }
//                          SPW_WRITE(&pDev->regs->timer, (SPW_READ(&pDev->regs->timer) & 0xFFC00FFF) | (((unsigned int)ioarg->buffer & 0x3FF) << 12));
//                          if (((SPW_READ(&pDev->regs->timer) >> 12) & 0x3FF) != (unsigned int)ioarg->buffer) {
//                                  return RTEMS_IO_ERROR;
//                          }
//                          pDev->config.disconnect = (unsigned int)ioarg->buffer;
//                        }else{
//                          SPACEWIRE_DBG("SPACEWIRE_IOCTRL_SET_DISCONNECT: not implemented for SPW2\n");
//                        }
//                        break;
//                case SPACEWIRE_IOCTRL_SET_PROMISCUOUS:        
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_PROMISCUOUS %i \n", (unsigned int)ioarg->buffer);
//                        if ((unsigned int)ioarg->buffer > 1) {
//                                return RTEMS_INVALID_NAME;
//                        }
//                        SPW_CTRL_WRITE(pDev, SPW_CTRL_READ(pDev) | ((unsigned int)ioarg->buffer << 5));
//                        if (((SPW_CTRL_READ(pDev) >> 5) & 1) != (unsigned int)ioarg->buffer) {
//                                return RTEMS_IO_ERROR;
//                        }
//                        pDev->config.promiscuous = (unsigned int)ioarg->buffer;
//                        break;
//                case SPACEWIRE_IOCTRL_SET_RMAPEN:
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_RMAPEN %i \n", (unsigned int)ioarg->buffer);
//                        if ((unsigned int)ioarg->buffer > 1) {
//                                return RTEMS_INVALID_NAME;
//                        }
//                        SPW_CTRL_WRITE(pDev, (SPW_STATUS_READ(pDev) & 0xFFFEFFFF) | ((unsigned int)ioarg->buffer << 16));
//                        if (((SPW_CTRL_READ(pDev) >> 16) & 1) != (unsigned int)ioarg->buffer) {
//                                return RTEMS_IO_ERROR;
//                        }
//                        pDev->config.rmapen = (unsigned int)ioarg->buffer;
//                        break;
//                case SPACEWIRE_IOCTRL_SET_RMAPBUFDIS: 
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_RMAPBUFDIS %i \n", (unsigned int)ioarg->buffer);
//                        if ((unsigned int)ioarg->buffer > 1) {
//                                return RTEMS_INVALID_NAME;
//                        }
//                        SPW_CTRL_WRITE(pDev, (SPW_STATUS_READ(pDev) & 0xFFFDFFFF) | ((unsigned int)ioarg->buffer << 17));
//                        if (((SPW_CTRL_READ(pDev) >> 17) & 1) != (unsigned int)ioarg->buffer) {
//                                return RTEMS_IO_ERROR;
//                        }
//                        pDev->config.rmapbufdis = (unsigned int)ioarg->buffer;
//                        break;
//                case SPACEWIRE_IOCTRL_SET_CHECK_RMAP: 
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_SET_CHECK_RMAP %i \n", (unsigned int)ioarg->buffer);
//                        if ((unsigned int)ioarg->buffer > 1) {
//                                return RTEMS_INVALID_NAME;
//                        }
//                        pDev->config.check_rmap_err = (unsigned int)ioarg->buffer;
//                        break;
//                case SPACEWIRE_IOCTRL_SET_RM_PROT_ID: 
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "SPACEWIRE_IOCTRL_SET_RM_PROT_ID %i \n", (unsigned int)ioarg->buffer);
//                        if ((unsigned int)ioarg->buffer > 1) {
//                                return RTEMS_INVALID_NAME;
//                        }
//                        pDev->config.rm_prot_id = (unsigned int)ioarg->buffer;
//                        break;
//                case SPACEWIRE_IOCTRL_SET_TXBLOCK: 
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "SPACEWIRE_IOCTRL_SET_TXBLOCK %i \n", (unsigned int)ioarg->buffer);
//                        if ((unsigned int)ioarg->buffer > 1) {
//                                return RTEMS_INVALID_NAME;
//                        }
//                        pDev->config.tx_blocking = (unsigned int)ioarg->buffer;
//                        break;
//                case SPACEWIRE_IOCTRL_SET_TXBLOCK_ON_FULL: 
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "SPACEWIRE_IOCTRL_SET_TXBLOCK_ON_FULL %i \n", (unsigned int)ioarg->buffer);
//                        if ((unsigned int)ioarg->buffer > 1) {
//                                return RTEMS_INVALID_NAME;
//                        }
//                        pDev->config.tx_block_on_full = (unsigned int)ioarg->buffer;
//                        break;        
//                case SPACEWIRE_IOCTRL_SET_DISABLE_ERR: 
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "SPACEWIRE_IOCTRL_SET_DISABLE_ERR %i \n", (unsigned int)ioarg->buffer);
//                        if ((unsigned int)ioarg->buffer > 1) {
//                                return RTEMS_INVALID_NAME;
//                        }
//                        pDev->config.disable_err = (unsigned int)ioarg->buffer;
//                        break;
//                case SPACEWIRE_IOCTRL_SET_LINK_ERR_IRQ: 
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "SPACEWIRE_IOCTRL_SET_LINK_ERR_IRQ %i \n", (unsigned int)ioarg->buffer);
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "CTRL REG: %x\n", SPW_CTRL_READ(pDev));
//                        if ((unsigned int)ioarg->buffer > 1) {
//                                return RTEMS_INVALID_NAME;
//                        }
//                        SPW_CTRL_WRITE(pDev, (SPW_CTRL_READ(pDev) & 0xFFFFFDF7) | ((unsigned int)ioarg->buffer << 9) | ((unsigned int)ioarg->buffer << 3));
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "CTRL REG: %x\n", SPW_CTRL_READ(pDev));
//                        if (((SPW_CTRL_READ(pDev) >> 9) & 1) != (unsigned int)ioarg->buffer) {
//                                return RTEMS_IO_ERROR;
//                        }
//                        pDev->config.link_err_irq = (unsigned int)ioarg->buffer;
//                        break;
//                case SPACEWIRE_IOCTRL_SET_EVENT_ID:
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "SPACEWIRE_IOCTRL_SET_EVENT_ID %i \n", (unsigned int)ioarg->buffer);
//                        pDev->config.event_id = (rtems_id)ioarg->buffer;
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL, "Event id: %i\n", pDev->config.event_id);
//                        break;
//                case SPACEWIRE_IOCTRL_GET_CONFIG:
//                        if (ioarg->buffer == NULL)
//                                return RTEMS_INVALID_NAME;
//                        SPACEWIRE_DBG2("SPACEWIRE_IOCTRL_GET_CONFIG \n");
//                        (*(spw_config *)ioarg->buffer).nodeaddr = pDev->config.nodeaddr;
//                        (*(spw_config *)ioarg->buffer).nodemask = pDev->config.nodemask;
//                        (*(spw_config *)ioarg->buffer).destkey = pDev->config.destkey;
//                        (*(spw_config *)ioarg->buffer).clkdiv = pDev->config.clkdiv;
//                        (*(spw_config *)ioarg->buffer).rxmaxlen = pDev->config.rxmaxlen;
//                        (*(spw_config *)ioarg->buffer).timer = pDev->config.timer;
//                        (*(spw_config *)ioarg->buffer).disconnect = pDev->config.disconnect;
//                        (*(spw_config *)ioarg->buffer).promiscuous = pDev->config.promiscuous;
//                        (*(spw_config *)ioarg->buffer).rmapen = pDev->config.rmapen;
//                        (*(spw_config *)ioarg->buffer).rmapbufdis = pDev->config.rmapbufdis;
//                        (*(spw_config *)ioarg->buffer).check_rmap_err = pDev->config.check_rmap_err;
//                        (*(spw_config *)ioarg->buffer).rm_prot_id = pDev->config.rm_prot_id;
//                        (*(spw_config *)ioarg->buffer).tx_blocking = pDev->config.tx_blocking;
//                        (*(spw_config *)ioarg->buffer).disable_err = pDev->config.disable_err;
//                        (*(spw_config *)ioarg->buffer).link_err_irq = pDev->config.link_err_irq;
//                        (*(spw_config *)ioarg->buffer).event_id = pDev->config.event_id;
//                        (*(spw_config *)ioarg->buffer).is_rmap = pDev->config.is_rmap;
//                        (*(spw_config *)ioarg->buffer).is_rmapcrc = pDev->config.is_rmapcrc;
//                        (*(spw_config *)ioarg->buffer).is_rxunaligned = pDev->config.is_rxunaligned;
//                        (*(spw_config *)ioarg->buffer).linkdisabled = pDev->config.linkdisabled;
//                        (*(spw_config *)ioarg->buffer).linkstart = pDev->config.linkstart;
//                        (*(spw_config *)ioarg->buffer).rx_blocking = pDev->config.rx_blocking;
//                        (*(spw_config *)ioarg->buffer).tx_block_on_full = pDev->config.tx_block_on_full;
//                        break;
//                case SPACEWIRE_IOCTRL_GET_LINK_STATUS:
//                        SPACEWIRE_DBGC(DBGSPW_IOCTRL,"SPACEWIRE_IOCTRL_GET_STATUS=%i \n", (unsigned int)((SPW_STATUS_READ(pDev) >> 21) & 0x7));
//                        *(unsigned int *)ioarg->buffer = (unsigned int )((SPW_STATUS_READ(pDev) >> 21) & 0x7);
//                        break;
//                case SPACEWIRE_IOCTRL_GET_STATISTICS: 
//                        if (ioarg->buffer == NULL)
//                                return RTEMS_INVALID_NAME;
//                        SPACEWIRE_DBG2("SPACEWIRE_IOCTRL_GET_STATISTICS \n");
//                        (*(spw_stats *)ioarg->buffer).tx_link_err = pDev->stat.tx_link_err;
//                        (*(spw_stats *)ioarg->buffer).rx_rmap_header_crc_err = pDev->stat.rx_rmap_header_crc_err;
//                        (*(spw_stats *)ioarg->buffer).rx_rmap_data_crc_err = pDev->stat.rx_rmap_data_crc_err;
//                        (*(spw_stats *)ioarg->buffer).rx_eep_err =  pDev->stat.rx_eep_err;
//                        (*(spw_stats *)ioarg->buffer).rx_truncated = pDev->stat.rx_truncated;
//                        (*(spw_stats *)ioarg->buffer).parity_err = pDev->stat.parity_err;
//                        (*(spw_stats *)ioarg->buffer).escape_err = pDev->stat.escape_err;
//                        (*(spw_stats *)ioarg->buffer).credit_err = pDev->stat.credit_err;
//                        (*(spw_stats *)ioarg->buffer).write_sync_err = pDev->stat.write_sync_err;
//                        (*(spw_stats *)ioarg->buffer).disconnect_err = pDev->stat.disconnect_err;
//                        (*(spw_stats *)ioarg->buffer).early_ep = pDev->stat.early_ep;
//                        (*(spw_stats *)ioarg->buffer).invalid_address = pDev->stat.invalid_address;
//                        (*(spw_stats *)ioarg->buffer).packets_sent = pDev->stat.packets_sent;
//                        (*(spw_stats *)ioarg->buffer).packets_received = pDev->stat.packets_received;
//                        break;
//                case SPACEWIRE_IOCTRL_CLR_STATISTICS:
//                        SPACEWIRE_DBG2("SPACEWIRE_IOCTRL_CLR_STATISTICS \n");
//                        pDev->stat.tx_link_err = 0;
//                        pDev->stat.rx_rmap_header_crc_err = 0;
//                        pDev->stat.rx_rmap_data_crc_err = 0;
//                        pDev->stat.rx_eep_err = 0;
//                        pDev->stat.rx_truncated = 0;
//                        pDev->stat.parity_err = 0;
//                        pDev->stat.escape_err = 0;
//                        pDev->stat.credit_err = 0;
//                        pDev->stat.write_sync_err = 0;
//                        pDev->stat.disconnect_err = 0;
//                        pDev->stat.early_ep = 0;
//                        pDev->stat.invalid_address = 0;
//                        pDev->stat.packets_sent = 0;
//                        pDev->stat.packets_received = 0;
//                        break;
//                case SPACEWIRE_IOCTRL_SEND:
//                        if (ioarg->buffer == NULL)
//                                return RTEMS_INVALID_NAME;
//                        args = (spw_ioctl_pkt_send *)ioarg->buffer;
//                        args->sent = 0;
//                        
//                        /* is link up? */
//                        if ( !pDev->running ) {
//                                return RTEMS_INVALID_NAME;
//                        }
//					
//                        SPACEWIRE_DBGC(DBGSPW_IOCALLS, "write [%i,%i]: hlen: %i hbuf:0x%x dlen:%i dbuf:0x%x\n", major, minor, 
//                                       (unsigned int)args->hlen, (int)args->hdr,(unsigned int)args->dlen, (int)args->data);
//                        
//                        /*Check For errors*/
//						spw_hw_handle_errors(pDev);
//						/*See if new decriptors were sent*/
//						spw_check_tx(pDev);
//		
//						if ((args->hlen > pDev->txhbufsize) || (args->dlen > pDev->txdbufsize) || 
//                            ((args->hlen+args->dlen) < 1) || 
//                            ((args->hdr == NULL) && (args->hlen != 0)) || ((args->data == NULL) && (args->dlen != 0))) {
//                                return RTEMS_INVALID_NAME;
//                        }
//                        while ((args->sent = spw_hw_send(pDev, args->hlen, args->hdr, args->dlen, args->data)) == 0) {
//                                if (pDev->config.tx_block_on_full == 1) { 
//                                        SPACEWIRE_DBG2("Tx Block on full \n");
//                                        rtems_task_wake_after(pDev->config.wait_ticks);
//                                } else {
//                                        SPACEWIRE_DBG2("Tx non blocking return when full \n");
//                                        return RTEMS_RESOURCE_IN_USE;
//                                }
//                        }
//                        SPACEWIRE_DBGC(DBGSPW_IOCALLS, "Tx ioctl return: %i  \n", args->sent);
//                        break;
//
//                case SPACEWIRE_IOCTRL_LINKDISABLE:
//                        pDev->config.linkdisabled = 1;
//                        pDev->config.linkstart = 0;
//                        SPW_CTRL_WRITE(pDev, (SPW_CTRL_READ(pDev) & 0xFFFFFFFC) | 1);
//                        if ((SPW_CTRL_READ(pDev) & 3) != 1) {
//                                return RTEMS_IO_ERROR;
//                        }
//                        break;
//
//                case SPACEWIRE_IOCTRL_LINKSTART:
//                        pDev->config.linkdisabled = 0;
//                        pDev->config.linkstart = 1;
//                        SPW_CTRL_WRITE(pDev, (SPW_CTRL_READ(pDev) & 0xFFFFFFFC) | 2);
//                        if ((SPW_CTRL_READ(pDev) & 3) != 2) {
//                                return RTEMS_IO_ERROR;
//                        }
//                        break;
//                
//                /* Calculate timer register from SPW Core frequency 
//                 * Also possible to set disconnect and timer64 from
//                 *  - SPACEWIRE_IOCTRL_SET_DISCONNECT
//                 *  - SPACEWIRE_IOCTRL_SET_TIMER
//                 */
//                case SPACEWIRE_IOCTRL_SET_COREFREQ:
//                        pDev->core_freq_khz = (unsigned int)ioarg->buffer;
//                        if ( pDev->core_freq_khz == 0 ){
//                                /* Get SPW clock frequency from system clock.
//                                 * System clock has been read from timer inited
//                                 * by RTEMS loader (mkprom)
//                                 */
//                                 pDev->core_freq_khz = sys_freq_khz;
//                        }
//                        
//                        /* Only SPW1 needs the Timer64 and Disconnect values 
//                         * SPW2 and onwards doesn't have this register.
//                         */
//                        if ( pDev->core_ver <= 1 ){
//                          /* Calculate Timer64 & Disconnect */
//                          pDev->config.timer = spw_calc_timer64(pDev->core_freq_khz);
//                          pDev->config.disconnect = spw_calc_disconnect(pDev->core_freq_khz);
//                          
//                          /* Set Timer64 & Disconnect Register */
//                          SPW_WRITE(&pDev->regs->timer, 
//                                 (SPW_READ(&pDev->regs->timer) & 0xFFC00000) |
//                                 ((pDev->config.disconnect & 0x3FF)<<12) |
//                                 (pDev->config.timer & 0xFFF));
//                        
//                          /* Check that the registers were written successfully */
//                          tmp = SPW_READ(&pDev->regs->timer) & 0x003fffff;
//                          if ( ((tmp & 0xFFF) != pDev->config.timer) ||
//                               (((tmp >> 12) & 0x3FF) != pDev->config.disconnect) ) {
//                                  return RTEMS_IO_ERROR;
//                          }
//                        }
//                        break;
//                        
//                case SPACEWIRE_IOCTRL_START:
//                        if ( pDev->running ){
//                                return RTEMS_INVALID_NAME;
//                        }
//                        
//                        /* Get timeout from userspace
//                         *  timeout:
//                         *   �  -1           = Default timeout
//                         *   �  less than -1 = forever
//                         *   �  0            = no wait, proceed if link is up
//                         *   �  positive     = specifies number of system clock ticks that 
//                         *                     startup will wait for link to enter ready mode.
//                         */
//                        timeout = (int)ioarg->buffer;
//                        
//                        if ( (ret=spw_hw_startup(pDev,timeout)) != RTEMS_SUCCESSFUL ) {
//                                return ret;
//                        }
//                        pDev->running = 1;
//                        break;
//                
//                case SPACEWIRE_IOCTRL_STOP:
//                        if ( !pDev->running ){
//                                return RTEMS_INVALID_NAME;
//                        }
//                        pDev->running = 0;
//                        
//                        /* Stop Receiver and transmitter */
//                        spw_hw_stop(pDev,1,1);
//                        break;
//                
//                default:
//                        return RTEMS_NOT_IMPLEMENTED;
//        }
//                   
//        SPACEWIRE_DBGC(DBGSPW_IOCALLS, "SPW_IOCTRL Return\n");
//        return RTEMS_SUCCESSFUL;
//}

/* ============================================================================== */

/**
 * @brief Writes the maximum reception length to the dma rxmax register
 * @param [in] pDev device's internal structure
 * @return 
 * 	- 1 if the operation was successful.
 * 	- 0 Value was not written to HW
 */
static int spw_set_rxmaxlen(SPW_DEV *pDev) {
	
	/* Maximum reception packet size*/
	unsigned int rxmax;
	
	/* Write maximum size (rxmaxlen) to respective register*/
	SPW_WRITE(&pDev->regs->dma0rxmax,pDev->config.rxmaxlen); 
	
	/* Read maximum size register*/
	rxmax = SPW_READ(&pDev->regs->dma0rxmax);
	
	/* Check if value was correctly written to hardware*/
	if (rxmax != pDev->config.rxmaxlen) {
		
		/* we had a io error*/
		return 0;
	}
	
	/* RX max length successfully changed*/
	return 1;
}

/**
 * @brief Inserts the descriptor tables, and verifies the core's capabilities
 * @param [in] pDev device's internal structure
 * @return 
 * 	- 0 the operation was successful.
 */
static int spw_hw_init(SPW_DEV *pDev) 
{
   /*Control register contents*/
	unsigned int ctrl;
        
	/*Read control register*/
	ctrl = SPW_CTRL_READ(pDev);

	/**Each descritor table has to be 0x400 aligned and has 0x400 of size*/
	/*Beginning of the RX descriptor table. 0x400 alignement ensured by the user*/
    pDev->rx = (SPACEWIRE_RXBD *) pDev->bdtable;
	
	/*Beginning of the TX descriptor table. 0x400 aligned*/
    pDev->tx = (SPACEWIRE_TXBD *) pDev->bdtable + SPACEWIRE_BDTABLE_ALIGMENT;
    SPACEWIRE_DBG("hw_init [minor %i]\n", pDev->minor);
    
	/*Check if the RMAP sub core is present*/
	pDev->config.is_rmap = ctrl & SPW_CTRL_RA;
	
	/*RMAP accepts unaligned memory access*/
	pDev->config.is_rxunaligned = ctrl & SPW_CTRL_RX;
	
	/*RMAP CRC is available*/
	pDev->config.is_rmapcrc = ctrl & SPW_CTRL_RC;
	
	return 0;
}


/**
 * @brief Wait for the SpW link to get to ready state
 * @param [in] pDev device's internal structure
 * @param [in] timeout Maximum duration of the wait
 * @return 
 * 	- 0 if the operation was successful.
 * 	- 1 Timeout was reached and the Link is not ready
 */
static int spw_hw_waitlink (SPW_DEV *pDev, int timeout) {
	
	/*ticks counter*/
	int j;
	
	/* No actual link interface on a DMA-only GRSPW2 connected to the
	 * SPW router
	 */
	if (pDev->core_ver == 3)
		return 0;
	
	/*User Configuration*/
	spw_user_config *config;
	
	if(pDev->minor >= configured_cores){
		/*This core was not configured by the user. Use defaults*/
		config = defconf;
	} else {
		/*Use user's configurations for this core*/
		config = user_config;
	}
	
	/* Check if a timeout value was provided on function call*/
	if ( timeout == -1 ){
			/* Wait default timeout */
			timeout = config->init_timeout;
	}

	j=0;
	/*Read SpW link status and wait for ready value*/
	while (SPW_LINKSTATE(SPW_STATUS_READ(pDev)) != 5) {
		/*If timeout is -1 wait forever*/
		if ( timeout <= -1 ) {
			/* wait forever */	
		}else if ( j > timeout ){
			/* timeout reached, return fail */
			return 1;
		}
		
		/* Sleep for 10 ticks */
		rtems_task_wake_after(10);
		
		/*Increment ticks counter */
		j+=10;
	}
	/*Link is in ready state return success.*/
	return 0;
}


/**
 * @brief Resets the SpW HW core
 * @param [in] pDev device's internal structure
 */
static void spw_hw_reset(SPW_DEV *pDev){
	
	/*Reset SpW core*/
	SPW_CTRL_WRITE(pDev, SPW_CTRL_RESET);
	
	/*Clear Status Register*/
	SPW_STATUS_WRITE(pDev, SPW_STATUS_TO | SPW_STATUS_CE | SPW_STATUS_ER 
					| SPW_STATUS_DE | SPW_STATUS_PE | SPW_STATUS_WE 
					| SPW_STATUS_IA | SPW_STATUS_EE);
	
	/*Start link finite state machine*/
	SPW_CTRL_WRITE(pDev, SPW_CTRL_LINKSTART); 
}


/**
 * @brief Reads the core capabilities and writes the user configuration to the HW
 * @param [in] pDev device's internal structure
 */
static void spw_hw_sync_config(SPW_DEV *pDev){
	/*used to temporarly contain register values*/
	unsigned int tmp;
	
	/*User core configuration*/
	spw_user_config *config;
	
	
	if(pDev->minor >= configured_cores){
		/*This core was not configured by the user. Use defaults*/
		config = defconf;
	} else {
		/*Use user's configurations for this core*/
		config = &user_config[pDev->minor];
	}
	
	
	/*verify node address value*/
	if(config->nodeaddr == -1){
		/*read node address from register*/
		tmp = SPW_READ(&pDev->regs->nodeaddr);
		
		/*Obtain node address*/
		pDev->config.nodeaddr = 0xFF & tmp;
		
		pprintf("Node Address: %d\n", pDev->config.nodeaddr);
		
		/*Obtain node mask*/
		pDev->config.nodemask = 0xFF & (tmp>>8);
		
	/*Write user defined values*/	
	} else {
	
		/*Read node adress register*/
		tmp = SPW_READ(&pDev->regs->nodeaddr);
		
		/* Remove old address */
		tmp &= 0xffffff00; 
		
		/*Insert new node address*/
		tmp |= config->nodeaddr;
		
		/*Write new value to the HW*/
		SPW_WRITE(&pDev->regs->nodeaddr, tmp);
		
		/*Store new nodeaddress in device's structure*/
		pDev->config.nodeaddr = config->nodeaddr;
		
		/*READ node address register*/
		tmp = SPW_READ(&pDev->regs->nodeaddr);
		
		/*Clear Old node mask*/
		tmp &= 0xffff00ff; 
		
		/*Insert new mask*/
		tmp |= config->nodemask<<8;
		
		/*Write new mask to hardware*/
		SPW_WRITE(&pDev->regs->nodeaddr, tmp);
		
		/*Store new nodemask in device's structure*/
		pDev->config.nodemask = config->nodemask;
	}
	
	
	/*Verify destination key*/
	if(config->destkey == -1){
		/*read destination key from register*/
		pDev->config.destkey = 0xFF & SPW_READ(&pDev->regs->destkey);
	} else {
		/*store user defined value*/
		pDev->config.destkey = config->destkey;
		
		/*Write new destination key to hardware*/
		SPW_WRITE(&pDev->regs->destkey, (unsigned int)config->destkey);
	}
	
	
	/*Verify clock divisor*/
	if(config->clkdiv == -1){
	
		/*read clock divisor from register*/
		pDev->config.clkdiv = 0xFFFF & SPW_READ(&pDev->regs->clkdiv);
		
	} else {
	
		/*store user defined value*/
		pDev->config.clkdiv = config->clkdiv;
		
		/*Read Clock Divisor register*/
		tmp = SPW_READ(&pDev->regs->clkdiv);
		
		/* Remove old Clockdiv Setting */
		tmp &= ~0xff; 
		
		 /* add new clockdiv setting */
		tmp |= ((unsigned int)pDev->config.clkdiv) & 0xff;
		
		/*Write new value to hardware*/
		SPW_WRITE(&pDev->regs->clkdiv, tmp);
	}
	
	/*Write promiscuous option to HW*/
	SPW_CTRL_WRITE(pDev, SPW_CTRL_READ(pDev) | ( config->promiscuous << 5));
	
	/*Store user defined value*/
	pDev->config.promiscuous = config->promiscuous;
	
	
	/*Write "RMAP enable" option to HW*/
	SPW_CTRL_WRITE(pDev, SPW_CTRL_READ(pDev) | ( config->rmapen << 16));
	
	/*Store user defined value*/
	pDev->config.rmapen = config->rmapen;
	
	
	/*Write "Disable RMAP buffer" option to HW*/
	SPW_CTRL_WRITE(pDev, SPW_CTRL_READ(pDev) | ( config->rmapbufdis  << 17));
	
	/*Store user defined value*/
	pDev->config.rmapbufdis  = config->rmapbufdis;
	
	
	/*Read control register*/
	tmp = SPW_CTRL_READ(pDev);
	
	/*Is SpW link disabled*/
	pDev->config.linkdisabled = 1 & tmp;
	
	/*Has SpW link started*/
	pDev->config.linkstart = 1 & (tmp >> 1);
	
	
	/*If core is from version 1*/
	if ( pDev->core_ver <= 1 ){
		/*read timer register*/
		tmp = SPW_READ(&pDev->regs->timer);
		
		/*Store timer value*/
		pDev->config.timer = 0xFFF & tmp;
		
		/*I don't know what this is. TODO*/
		pDev->config.disconnect = 0x3FF & (tmp >> 12);
	}else{
		/*Clear timer and disconnect if core's version is 2*/
		pDev->config.timer = 0;
		pDev->config.disconnect = 0;
	}
	
	return;
}

/**
 * @brief Prepares the core and starts the communication
 * @param [in] pDev device's internal structure
 * @param [in] timeout Maximum duration of the wait
 * @return 
 * 	- RTEMS_SUCCESSFUL if the operation was successful.
 * 	- RTEMS_IO_ERROR Error while writing a register
 *  - RTEMS_TIMEOUT Link was not ready in time
 *
 *  This fucntion clears the status registers, distrubtes the buffer memory
 *  through all descriptors and enables transmission and reception
 */
static int spw_hw_startup (SPW_DEV *pDev, int timeout){
	
	/*iterates through buffers*/
	int i;
	
	/*DMA Control Register Contents*/
	unsigned int dmactrl;
	
	/*Clear STATUS register flags*/
	SPW_WRITE(&pDev->regs->status, (SPW_STATUS_TO|SPW_STATUS_CE|SPW_STATUS_ER
									|SPW_STATUS_DE|SPW_STATUS_PE|SPW_STATUS_WE
									|SPW_STATUS_IA|SPW_STATUS_EE)); 
	
	/*Wait for the SPW link to be ready*/
	if (spw_hw_waitlink(pDev,timeout)) {
		SPACEWIRE_DBG2("Device open. Link is not up\n");
		
		/*We have an error: link was not ready inside timeout*/
		return RTEMS_TIMEOUT;
	}
	
	/*Clear DMA Control/Status (values are cleared when set)*/
	SPW_WRITE(&pDev->regs->dma0ctrl, SPW_DMACTRL_PS | SPW_DMACTRL_PR | SPW_DMACTRL_TA | SPW_DMACTRL_RA); 
	
	/*Verify if DMA Control Register is cleared*/
	if ((dmactrl = SPW_READ(&pDev->regs->dma0ctrl)) != 0) {
		SPACEWIRE_DBG2("DMACtrl is not cleared\n");
		
		/*There were problems writing to SpW regsiters*/
		return RTEMS_IO_ERROR;
	}

	/* prepare transmit buffers. Iterate through all descriptors */
	for (i = 0; i < pDev->txbufcnt; i++) {
	
		/*Clear control field*/
		pDev->tx[i].ctrl = 0;
		
		/*Set tx header buffer pointers*/
		pDev->tx[i].addr_header = (uint32_t)xky_syscall_get_physical_addr(((unsigned int)&pDev->ptr_txhbuf0[0]) + (i * pDev->txhbufsize));
		
		/*Set tx data buffer pointers*/
		pDev->tx[i].addr_data = (uint32_t)xky_syscall_get_physical_addr(((unsigned int)&pDev->ptr_txdbuf0[0]) + (i * pDev->txdbufsize));
	}
	
	/*Current TX descriptor*/
	pDev->tx_cur = 0;
	
	/*Last sent descritor*/
	pDev->tx_sent = 0;
	
	/*All descritor in use*/
	pDev->tx_all_in_use = 0;
	
	/* prepare recieve buffers. Iterate through all RX descriptors */
	for (i = 0; i < pDev->rxbufcnt; i++) 
	{
		/*Check if we are setting up the last descriptor*/
		if (i+1 == pDev->rxbufcnt){
			
			/*Enable the descritor and enable wrap*/
			pDev->rx[i].ctrl = SPW_RXBD_EN | SPW_RXBD_WR;
		} else {
			
			/*Enable descriptor*/
			pDev->rx[i].ctrl = SPW_RXBD_EN;
		}
		
		/*Set rx data buffer pointers*/
		pDev->rx[i].addr = (uint32_t)xky_syscall_get_physical_addr(((unsigned int)&pDev->ptr_rxbuf0[0]) + (i * pDev->rxbufsize));
	}
	
	/*Current RX descriptor. It will be used on next read call*/
	pDev->rxcur = 0;
	
	/*Current position on the RX buffer*/
	pDev->rxbufcur = -1;
	
	/*Set maximum reception size*/
	spw_set_rxmaxlen(pDev);
	
	/*write pointer to the beginning of TX descritor table in respective register*/
	SPW_WRITE(&pDev->regs->dma0txdesc, (uint32_t)xky_syscall_get_physical_addr((unsigned int) pDev->tx));
	
	/*write pointer to the beginning of RX descritor table in respective register*/
	SPW_WRITE(&pDev->regs->dma0rxdesc, (uint32_t)xky_syscall_get_physical_addr((unsigned int) pDev->rx));
	
	/*Read DMA Control register*/
	dmactrl = SPW_READ(&pDev->regs->dma0ctrl);
	
	/*Enable RX*/
	SPW_WRITE(&pDev->regs->dma0ctrl, (dmactrl & SPW_PREPAREMASK_RX) | SPW_DMACTRL_RD | SPW_DMACTRL_RXEN | SPW_DMACTRL_NS);
	
	SPACEWIRE_DBGC(DBGSPW_TX,"0x%x: setup complete\n", (unsigned int)pDev->regs);
	
	/*Hardware has successfully setup*/
	return RTEMS_SUCCESSFUL;
}

/**
 * @brief Stop the rx or/and tx by disabling the receiver/transmitter
 * @param [in] pDev device's internal structure
 * @return RTEMS_SUCCESSFUL
 */
static int spw_hw_stop (SPW_DEV *pDev, int rx, int tx) {
	/*DMA control register contents*/
	unsigned int dmactrl;

	/*Read DMA control register*/
	dmactrl = SPW_READ(&pDev->regs->dma0ctrl);
	if (rx){
		/*Disable reception*/
		dmactrl &= ~(SPW_DMACTRL_RXEN|SPW_DMACTRL_RXIE|SPW_DMACTRL_RD);
	}
	if (tx){
		/*Disable Transmission*/
		dmactrl &= ~(SPW_DMACTRL_TXEN|SPW_DMACTRL_TXIE);
	}
	
	/* don't clear status flags, but clear dma ctrl */
	dmactrl &= ~(SPW_DMACTRL_RA|SPW_DMACTRL_PR|SPW_DMACTRL_AI);
	SPW_WRITE(&pDev->regs->dma0ctrl, dmactrl);
	
	return RTEMS_SUCCESSFUL;
}


/**
 * @brief Send data through SpaceWire
 * @param [in] pDev device's internal structure
 * @param [in] hlen header length
 * @param [in] *hdr pointer a buffer containing the SpW header
 * @param [in] dlen data length
 * @param [in] *data pointer containing data
 * @return Number of bytes that were written
 */
int spw_hw_send(SPW_DEV *pDev, unsigned int hlen, char *hdr, unsigned int dlen, char *data) {
	
	int j = 0;
	iop_debug("       %dWRAPPER->BUFFER: ", j);
	for (j = 0; j <  hlen; j++) {
		iop_debug("%x ", *(hdr+j));
	} iop_debug("\n       size: %d\n", j);
	for (j = 0; j <  dlen; j++) {
		iop_debug("%x ", *(data+j));
	} iop_debug("\n       size: %d\n", j);
	
	/*DMA control register contentes*/
	unsigned int dmactrl;
	
	/*control register contentes*/
	unsigned int ctrl;
	
#ifdef DEBUG_SPACEWIRE_ONOFF
	unsigned int k;
#endif

	// rtems_interrupt_level level;
	
	/*Descriptor currently in use*/
	unsigned int cur = pDev->tx_cur;
	
	/*Pointer to header buffer*/
	char *txh = pDev->ptr_txhbuf0 + (cur * pDev->txhbufsize);

	/*Pointer to data buffer*/
	char *txd = pDev->ptr_txdbuf0 + (cur * pDev->txdbufsize);
	
	/*Read Control register*/
	ctrl = SPW_READ((volatile void *)&pDev->tx[cur].ctrl);
	
	/*Current Descriptor is enabled, so we have no more available descriptors*/
	if (ctrl & SPW_TXBD_EN) {
		return 0;
	}
	
	/*Copy data*/
	memcpy(&txd[0], data, dlen);
	
	/*Copy header*/
	memcpy(&txh[0], hdr, hlen);
	
#ifdef DEBUG_SPACEWIRE_ONOFF  
	if (DEBUG_SPACEWIRE_FLAGS & DBGSPW_DUMP) {
			for (k = 0; k < hlen; k++){
					if (k % 16 == 0) {
							pprintf ("\n");
					}
					pprintf ("%.2x(%c) ",txh[k] & 0xff,isprint(txh[k] & 0xff) ? txh[k] & 0xff : ' ');
			}
			pprintf ("\n");
	}
	if (DEBUG_SPACEWIRE_FLAGS & DBGSPW_DUMP) {
			for (k = 0; k < dlen; k++){
					if (k % 16 == 0) {
							pprintf ("\n");
					}
					pprintf ("%.2x(%c) ",txd[k] & 0xff,isprint(txd[k] & 0xff) ? txd[k] & 0xff : ' ');
			}
			pprintf ("\n");
	}
#endif
	
	/* Setup header address on descriptor*/
	pDev->tx[cur].addr_header = (uint32_t)xky_syscall_get_physical_addr((unsigned int)txh);
	
	/* insert data size*/
	pDev->tx[cur].len = dlen;
	
	/* setup data address in descriptor*/
	pDev->tx[cur].addr_data = (uint32_t)xky_syscall_get_physical_addr((unsigned int)txd);
	
	/* Is this the last descriptor*/
	if (pDev->tx_cur == (pDev->txbufcnt - 1) ) {
	
		/* enable descriptor with Wrap and insert header length*/
		pDev->tx[cur].ctrl = SPW_TXBD_WR | SPW_TXBD_EN | hlen;
		
	} else {
	
		/* enable descriptor and insert header length*/
		pDev->tx[cur].ctrl = SPW_TXBD_EN | hlen;
	}
	
	/* Read DMA control register*/	
	dmactrl = SPW_READ(&pDev->regs->dma0ctrl);
	
	/* Enable TX*/
	SPW_WRITE(&pDev->regs->dma0ctrl, (dmactrl & SPW_PREPAREMASK_TX) | SPW_DMACTRL_TXEN);
	
	/* Increment current descriptor*/
	pDev->tx_cur = (pDev->tx_cur + 1) % pDev->txbufcnt;
	
	/* Do we have more descriptors available?*/
	if (pDev->tx_cur == pDev->tx_sent) {
		
		/*all in use :(*/
		pDev->tx_all_in_use = 1;
	}

	/* In blocking mode wait until message is sent */
	if (pDev->config.tx_blocking) {
	
		/* When TX gets disabled it means that the data was sent*/
		while ( SPW_READ(&pDev->regs->dma0ctrl) & SPW_DMACTRL_TXEN) {
			SPACEWIRE_DBGC(DBGSPW_TX, "Tx blocking\n");
			
			/*Sleep for a while*/
			rtems_task_wake_after(pDev->config.wait_ticks);
			
			/*TODO: This is assuming that the TX is disabled because data
			 * was sent. This may also be due to an error. If it is an error
			 * we should return 0 to the user. This way the user knows it
			 * should retry send the data.
			 */
		}
	}
	SPACEWIRE_DBGC(DBGSPW_TX, "0x%x: transmitted <%i> bytes\n", (unsigned int) pDev->regs, dlen+hlen);
	iop_debug("0x%x: transmitted <%i> bytes\n", (unsigned int) pDev->regs, dlen+hlen);
	
	/*Return total data size that was written into the bus = header + data*/
	return hlen + dlen;
}

/**
 * @brief Receives data from a SpW link
 * @param [in] pDev device's internal structure
 * @param [in] c data length (count)
 * @param [in] *b pointer for the data buffer 
 * @return Number of bytes that were received
 */
static int spw_hw_receive(SPW_DEV *pDev, char *hdr, char *b, int c) {
	
	iop_debug("ENTERED hw_receive. data len: %d\n", c);
	/*Real size of the received data*/
	unsigned int len;
	
	/*data size to be copied*/
	unsigned int rxlen;
	
	/*Control register contents*/
	unsigned int ctrl;
	
	/*Number of the current RX descriptor*/
	unsigned int cur; 
	
	/*Number of characters to dump*/
	unsigned int dump_start_len;
	
	/*Iterator through the rx buffer*/
	int i;
	
	/*Pointer to current buffer*/
	char *rxb;  
    
	/** -In promiscous mode the packet is completly dumped in the Rx buffer
	 *	-If remove protocol id option is activated, the protocol id and
	 *  	the source address will be removed from the packet
	 *  - In default mode only the source address is removed
	 */
    if ( pDev->config.promiscuous ) {
		
		/* make sure address and prot can be read in promiscuous mode */
		dump_start_len = 1;  /** @todo SpaceWire Routing with promiscous mode*/
		
	} else if (pDev->config.rm_prot_id) {
		
		/* skip source address and protocol id */
		dump_start_len = 2; 
	} else {
		
		/* default: skip only source address. WHEN USING ROUTERS, THERE 
				SHOULD BE NO HEADER.*/
		dump_start_len = 0; 
	}
    
	/* We haven't received nothing yet*/
	rxlen = 0;
	
	/* Current Descriptor*/
	cur = pDev->rxcur;
	
	/* Pointer to current RX buffer*/
	rxb = pDev->ptr_rxbuf0 + (cur * pDev->rxbufsize);

	SPACEWIRE_DBGC(DBGSPW_RX, "0x%x: waitin packet at pos %i\n", (unsigned int) pDev->regs, cur);
	
	/*Read descriptor control word*/
	ctrl = SPW_READ((volatile void *)&pDev->rx[cur].ctrl);
	
	/*Check if the descriptor is enabled*/
	if (ctrl & SPW_RXBD_EN) {
		iop_debug("ctrl: %x & SPW_RXBD_EN: %x == true\n", ctrl, SPW_RXBD_EN);
		/*Descriptor is enabled, so we didn't received anything*/
		return rxlen;
	}
	
	SPACEWIRE_DBGC(DBGSPW_RX, "checking packet\n");
	
	/*Obtain the real size of received data*/
	len = SPW_RXBD_LENGTH & ctrl;
	
	iop_debug("ctrl: %x, len: %d \n", ctrl, len);
	
	/*Check for errors*/
	if (!((ctrl & SPW_RXBD_ERROR) || (pDev->config.check_rmap_err && (ctrl & SPW_RXBD_RMAPERROR)))) {
		
		SPACEWIRE_DBGC(DBGSPW_RX, "incoming packet len %i\n", len);
		
		/*increment statistics*/
		pDev->stat.packets_received++;
		
		/*should we dump something from the packet?*/
		pDev->rxbufcur = dump_start_len;

		
		/*obtain the data size to be copied = real data size - dumped size*/
		rxlen = len - pDev->rxbufcur;
		
		SPACEWIRE_DBGC(DBGSPW_RX, "C %i\n", c);
		SPACEWIRE_DBGC(DBGSPW_RX, "Dump %i\n", dump_start_len);
		SPACEWIRE_DBGC(DBGSPW_RX, "Bufcur %i\n", pDev->rxbufcur);
		SPACEWIRE_DBGC(DBGSPW_RX, "Rxlen %i\n", rxlen );
		
		/*Check if the received size (rxlen) exceeds what the user requested (c)*/
		if (rxlen > c) {
			
			/*We cannot exceed what the user requested*/
			iop_debug("RXLEN exceeds user request rxlen: %x > user: %x\n", rxlen, c);
			rxlen = 0;
		}
		
		iop_debug("gonna start");
	   /**
		* If CPU has snooping we can use memcpy directly;
		* else, we copy character by character using a alternate call (lduba  []1)
		*/
		if(rxlen > 0){
			if (CPU_SPARC_HAS_SNOOPING) {
				
				/* Copy header */
				memcpy(hdr, rxb, 1);
				
				/*Copy received data to user's buffer*/
				memcpy(b, rxb+pDev->rxbufcur, rxlen);
			} else {
				
				hdr[0] = MEM_READ(rxb);
				
				/*Iterate through received buffer*/
				for(i = 0; i < rxlen; i++) {
				
					/*Copy received character to user's buffer TODO*/
					b[i] = MEM_READ(rxb+pDev->rxbufcur+i);
				}
			}
		}
		
		SPACEWIRE_DBGC(DBGSPW_RX, "Next descriptor\n");
		
		/*Reactivate this descritor so it can receive new data*/
		spw_rxnext(pDev);
		
	} else {
	
		/*Check for RX errors*/
		check_rx_errors(pDev, ctrl);
		
		/*Reactivate this descritor so it can receive new data*/
		spw_rxnext(pDev);
	}
	
	
	iop_debug("IM HERE and rxlen is : %d \n", rxlen);
	/*Return the size of transfered data*/
	return rxlen;
}


/**
 * @brief Enables the last used descriptor
 * @param [in] pDev device's internal structure
 */
static void spw_rxnext(SPW_DEV *pDev) {

	/*DMA control register contents*/
	unsigned int dmactrl;
	
	/*Number of the current descriptor*/
	unsigned int cur = pDev->rxcur;
	
	/*control register contents*/
	unsigned int ctrl = 0;
	
	/*Is this the last descriptor of the table*/
	if (cur == (pDev->rxbufcnt - 1)) {
			
		/*Enable the descriptor and enable wrap*/
		pDev->rx[cur].ctrl = ctrl | SPW_RXBD_EN | SPW_RXBD_WR;
		
		/*Wrap to the beginning of the table*/
		cur = 0;
	} else {
	
		/*Enable the descriptor*/
		pDev->rx[cur].ctrl = ctrl | SPW_RXBD_EN;
		
		/*Increment Current descriptor*/
		cur++;
	}
	
	/*Store new current descriptor*/        
	pDev->rxcur = cur;
	
	/*This is a new descriptor, so we start reading at the beggining of its buffer*/
	pDev->rxbufcur = -1;
	
	/*read DMA control register contents*/
	dmactrl = SPW_READ(&pDev->regs->dma0ctrl);
	
	/* start RX */
	SPW_WRITE(&pDev->regs->dma0ctrl, (dmactrl & SPW_PREPAREMASK_RX) | SPW_DMACTRL_RD | SPW_DMACTRL_RXEN | SPW_DMACTRL_NS);
  
}

/**
 * @brief Chekcs for error during reception
 * @param [in] pDev device's internal structure
 * @param [in] ctrl Control Register contents
 */
static void check_rx_errors(SPW_DEV *pDev, int ctrl) {
    /*Check different errors and increment its statistics*/
	if (ctrl & SPW_RXBD_EEP) {
		pDev->stat.rx_eep_err++;
	}
	if (ctrl & SPW_RXBD_EHC) {
		if (pDev->config.check_rmap_err) {
			pDev->stat.rx_rmap_header_crc_err++;
		}
	}  
	if (ctrl & SPW_RXBD_EDC) {
		if (pDev->config.check_rmap_err) {
			pDev->stat.rx_rmap_data_crc_err++;
		}
	}
	if (ctrl & SPW_RXBD_ETR) {
		pDev->stat.rx_truncated++;
	}
}

// int SPW_PREFIX(_get_conf_size)(void){
	// int i=0;
	// int size=0;
	// int core_number;
	
	// core_number = spw_cores+spw_cores2;
	
	// for(i=0; i<core_number; i++){
		// if(i >= configured_cores){
			// /*This core was not configured by the user. Use defaults*/
			// size = size + (defconf.txdbufsize+defconf.txhbufsize)*defconf.txdesccnt + defconf.rxbufsize*defconf.rxdesccnt;
		// } else if(user_config[i] == NULL){
			// /*This core was not configured by the user. Use defaults*/
			// size = size + (defconf.txdbufsize+defconf.txhbufsize)*defconf.txdesccnt + defconf.rxbufsize*defconf.rxdesccnt;
		// } else {
			// /*Use user's configurations for this core*/
			// size = size + (user_config[i]->txdbufsize+user_config[i]->txhbufsize)*user_config[i]->txdesccnt + user_config[i]->rxbufsize*user_config[i]->rxdesccnt;
		// }
	// }
	
	// size = core_number*2*1024 + size;
	
	// return size;

// }
