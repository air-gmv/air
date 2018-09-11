/** 
 *  @file ambaext.c
 * 
 *  COPYRIGHT (c) 2011.
 *  GMV-SKYSOFT 
 * 
 *  The license and distribution terms for this file may be 
 *  found in the file LICENSE at: 
 *  http://www.rtems.com/license/LICENSE.
 * 	
 *  @author Cl�udio Silva
 *  @brief This driver add-on hook performs amba bus scanning
 * 
 *  This file contains helper functions to search for devices
 *  on the amba bus. Drivers need these functions to identify 
 *  their devices. 
 */ 
 
#include <bsp.h>
#include <ambapp.h>
#include <pprintf.h>
#include <ambaext.h>

struct ambapp_bus amba_conf;

struct ambapp_dev_find_match_arg {

    int      index;
    int      count;
    int      type;
    void      *dev;

};

/* AMBA PP find routines */

static int ambapp_dev_find_match(struct ambapp_dev *dev, int index, void *arg)
{
    struct ambapp_dev_find_match_arg *p = arg;

    if (p->index == 0) {

        /* Found controller, stop */
        if (p->type == DEV_APB_SLV) {
            *(struct ambapp_apb_info *)p->dev = *DEV_TO_APB(dev);
            p->dev = ((struct ambapp_apb_info *)p->dev)+1;
        } else {
            *(struct ambapp_ahb_info *)p->dev = *DEV_TO_AHB(dev);
            p->dev = ((struct ambapp_ahb_info *)p->dev)+1;
        }

        p->count--;

        if (p->count < 1)
            return 1;

    } else {
        p->index--;
    }

    return 0;
}

/**
 * 	@brief Finds the nth ahb device
 *  @param [in] ambaconf: Amba bus plug and play memory structure
 *  @param [in] vendor: VendorID
 *  @param [in]	device: DeviceID
 *  @param [out] dev: plug and play data for the device
 *  @param [in]	index: Number of the devic to be found (e.g. 2nd GRETH core)
 */
int amba_find_next_ahbslv (struct ambapp_bus * amba_conf, int vendor, int device,
						struct  ambapp_ahb_info * dev, int index)
{
    int maxno = 1;

    struct ambapp_dev_find_match_arg arg;

    arg.index = index;
    arg.count = maxno;
    arg.type = DEV_AHB_SLV; /* AHB SLV */
    arg.dev = dev;

    ambapp_for_each(amba_conf, (OPTIONS_ALL|OPTIONS_AHB_SLVS), vendor, device, ambapp_dev_find_match, &arg);

    return maxno - arg.count;
}



#ifdef CODE_ON_HOLD
/**
 * 	@brief Finds the nth ahb master device
 *  @param [in] ambaconf: Amba bus plug and play memory structure
 *  @param [in] vendor: VendorID
 *  @param [in]	device: DeviceID
 *  @param [out] dev: plug and play data for the device
 *  @param [in]	index: Number of the devic to be found (e.g. 2nd GRETH core)
 */
int amba_find_next_ahbmst(amba_confarea_type * amba_conf, int vendor, int device,
                          amba_ahb_device * dev, int index)
{
	/* AHB device configuration word */
	unsigned int conf;
	
	/* AHB mem bar */
	unsigned int mbar;
	
	/*AHB IO area mem address*/
	unsigned int addr;
	
	/* iterator index through the AHB master devices */
	int i;
	
	/* iterator index through the AHB device mem areas */
	int j;
	
	/*Number of ahb devices of that type found*/
	int cnt;

	/* iterate through the AHB slave devices */
	for (cnt = i = 0; i < amba_conf->ahbmst.devnr; i++) 
	{
		/*Get AHB device conf word*/
		conf = amba_get_confword (amba_conf->ahbmst, i, 0);
		
		/*If the device vendor and device id coincides with the ones requested*/
		if ((amba_vendor (conf) == vendor) && (amba_device (conf) == device)) {
			
			/*If this in the n-th device of this type*/
			if (cnt == index) 
			{	
				/*Search for the Device's IO area*/
				for (j = 0; j < 4; j++) 
				{
					/* get the io mem bar */
					mbar = amba_ahb_get_membar (amba_conf->ahbmst, i, j);
					
					/*Get realtive addr of mem area*/
					addr = amba_membar_start (mbar);
					
					/*Check memory type*/
					if (amba_membar_type (mbar) == AMBA_TYPE_AHBIO) 
					{
						/*obtain real memory address*/
						addr = AMBA_TYPE_AHBIO_ADDR (addr, amba_conf->ioarea);
					
					/*It's not IO memory*/
					} else 
					{
						/* convert address if needed */
						if ((addr = amba_addr_from (amba_conf->mmaps, addr)) == 1) 
						{	 
							/* no available memory translation available */
							addr = 0;        
						}
					}
					/* store mem zone address */
					dev->start[j] = addr;
				}
				/*Get device IRQ line*/
				dev->irq = amba_irq (conf);
				
				/*Get device version*/
				dev->ver = amba_ver (conf);
				
				/*we have found the device*/
				return 1;
			}
			/*This is not the n-th device. Increment Count*/
			cnt++;
		}
	}
	/*We didn't find the device*/
	return 0;
}
#endif

/**
 * 	@brief Returns the number of APB Slave devices that have a given vendorid
 *   and deviceid
 *  @param [in] ambaconf: Amba bus plug and play memory structure
 *  @param [in] vendor: VendorID
 *  @param [in]	device: DeviceID
 */
int amba_get_number_apbslv_devices (struct ambapp_bus * amba_conf, int vendor,
									int device)
{
    int count = 10000;

    ambapp_for_each(amba_conf, (OPTIONS_ALL|OPTIONS_APB_SLVS) , vendor, device, ambapp_find_by_idx, &count);

    return 10000 - count;
}


/**
 * 	@brief Finds the nth ahb device
 *  @param [in] ambaconf: Amba bus plug and play memory structure
 *  @param [in] vendor: VendorID
 *  @param [in]	device: DeviceID
 *  @param [out] dev: plug and play data for the device
 *  @param [in]	index: Number of the devic to be found (e.g. 2nd GRETH core)
 */
int amba_find_next_apbslv (struct ambapp_bus * amba_conf, int vendor, int device,
						 struct  ambapp_apb_info * dev, int index)
{
    int maxno = 1;

    struct ambapp_dev_find_match_arg arg;

    arg.index = index;
    arg.count = maxno;
    arg.type = DEV_APB_SLV; /* APB */  
    arg.dev = dev;
            
    ambapp_for_each(amba_conf, (OPTIONS_ALL|OPTIONS_APB_SLVS), vendor, device, ambapp_dev_find_match, &arg);
    return maxno - arg.count;
}

#ifdef CODE_ON_HOLD
/**
 * @fn 
 * @brief Prints vendor and deviceid for every device on AHB and APB buses.
 * @warning debug only! remove!
 */
void amba_debug_scan(unsigned int ioarea)
{
    /* address to configuration area */
    unsigned int *cfg_area;

    /* AHB slave mem bar value */
    unsigned int mbar;

    /* AHB slave configuration word */
    unsigned int conf;

    /* AHB slave iterator */
    int i;
    
    /* APB slave device iterator */
    int j;

    /* APB master address */
    unsigned int apbmst;
	
	/* number of ahbslv */
	unsigned int ahbslb_nr = 0;
	
	unsigned int addr[64];
	
    /* number of maximum AHB devices to search for */
    int maxloops;

    /* scan first bus for 64 devices*/
    maxloops = 64;

    /* initialize the config area (master) */
    cfg_area = (unsigned int *) ( ioarea | AMBA_CONF_AREA );

    /* loop for the number of devices */
    for(i = 0; i < maxloops; i++)
    {
		
		/*Get AHB device conf word*/
		conf = *cfg_area;
		
		pprintf("ahbmst: %d: %d\n", amba_vendor (conf), amba_device (conf));
		
        /* increment the config area */
        cfg_area += AMBA_AHB_CONF_WORDS;
    }

    /* set the config area to the io area (slave) */
    cfg_area = (unsigned int *) ( ioarea | AMBA_CONF_AREA | AMBA_AHB_SLAVE_CONF_AREA );

    /* loop for the number of devices */
    for(i = 0; i < maxloops; i++)
    {	
		if(*cfg_area){
		
			/*Get AHB device conf word*/
			conf = *cfg_area;
			
			addr[i] = (unsigned int)(cfg_area);
		
			/* Print the device id */
			pprintf("ahbslv: %d: %d\n", amba_vendor (conf), amba_device (conf));
		
			ahbslb_nr++;
		}
		
		/* increment the config area */
		cfg_area += AMBA_AHB_CONF_WORDS;
		
    }

    /* loop for the number of slave devices */
    for(i = 0; i < ahbslb_nr; i++)
    {
        /* get the configuration for the device */
        conf = *((unsigned int *)addr[i]);

        mbar = *((unsigned int *)addr[i]+4);

        /* if this is a GAISLER APB Master */
        if(( amba_vendor(conf) == VENDOR_GAISLER ) && ( amba_device(conf) == GAISLER_APBMST ))
        {
            apbmst = amba_membar_start(mbar);

            /* initialize the config area to APB Master */
            cfg_area = (unsigned int *) ( apbmst | AMBA_CONF_AREA );
			
			pprintf("\nAPBMST found: %d\n", cfg_area);
			
            /* iterate for the APB slave devices */
            for(j = 0; j < 17; j++)
            {
				
				/*Get AHB device conf word*/
			    conf = *cfg_area;
				
				/* Print the device id */
				pprintf("apbslv: %d: %d\n", amba_vendor(conf), amba_device(conf));
				
                /* increment the config area */
                cfg_area += AMBA_APB_CONF_WORDS;
            }
        }
    }
}
#endif
