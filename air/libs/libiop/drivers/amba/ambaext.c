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


/**
 * 	@brief Finds the nth ahb device
 *  @param [in] ambaconf: Amba bus plug and play memory structure
 *  @param [in] vendor: VendorID
 *  @param [in]	device: DeviceID
 *  @param [out] dev: plug and play data for the device
 *  @param [in]	index: Number of the devic to be found (e.g. 2nd GRETH core)
 */
int amba_find_next_ahbslv (amba_confarea_type * amba_conf, int vendor, int device,
						   amba_ahb_device * dev, int index)
{
	/*AHB IO area mem address*/
	unsigned int addr;
	
	/* AHB device configuration word */
	unsigned int conf;
	
	/* AHB mem bar */
	unsigned int mbar;
	
	/* iterator index through the AHB slave devices */
	int i;
	
	/* iterator index through the AHB device mem areas */
	int j;
	
	/*Number of apb devices of that type found*/
	int cnt;
	
	/* iterate through the AHB slave devices */
	for (cnt = i = 0; i < amba_conf->ahbslv.devnr; i++) 
	{	
		/*Get AHB device conf word*/
		conf = amba_get_confword (amba_conf->ahbslv, i, 0);
		
		/*If the device vendor and device id coincides with the ones requested*/
		if ((amba_vendor (conf) == vendor) && (amba_device (conf) == device)) 
		{
			/*If this in the n-th device of this type*/
			if (cnt == index) 
			{
				/*Search for the Device's IO area*/
				for (j = 0; j < 4; j++) 
				{
					/* get the io mem bar */
					mbar = amba_ahb_get_membar (amba_conf->ahbslv, i, j);
					
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
							/* no available memory translation available, will not be able to access*/
							addr = 0;         
						}
					}
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
int amba_get_number_apbslv_devices (amba_confarea_type * amba_conf, int vendor,
									int device)
{
	/* APB device configuration word */
	unsigned int conf;
	
	/* iterator index through the APB slave devices */
	int i;
	
	/*Number of apb devices of that type found*/
	int cnt;
	
	/* iterate through the APB slave devices */
	for (cnt = i = 0; i < amba_conf->apbslv.devnr; i++)
	{
		/*Get apb device conf word*/
		conf = amba_get_confword (amba_conf->apbslv, i, 0);
		
		/*If the device vendor and device id coincides with the ones requested*/
		if ((amba_vendor (conf) == vendor) && (amba_device (conf) == device))
		{
			/*Increment the number of devices found*/
			cnt++;
		}
	}
	
	/*Return the number of devices*/
	return cnt;
}


/**
 * 	@brief Finds the nth ahb device
 *  @param [in] ambaconf: Amba bus plug and play memory structure
 *  @param [in] vendor: VendorID
 *  @param [in]	device: DeviceID
 *  @param [out] dev: plug and play data for the device
 *  @param [in]	index: Number of the devic to be found (e.g. 2nd GRETH core)
 */
int amba_find_next_apbslv (amba_confarea_type * amba_conf, int vendor, int device,
						   amba_apb_device * dev, int index)
{
	/* APB device configuration word */
	unsigned int conf;
	
	/* APB slave mem bar */
	unsigned int iobar;
	
	/* iterator index through the APB slave devices */
	int i;
	
	/*Number of apb devices of that type found*/
	int cnt;
	
	/* iterate through the APB slave devices */
	for (cnt = i = 0; i < amba_conf->apbslv.devnr; i++) 
	{
		/*Get apb device conf word*/
		conf = amba_get_confword (amba_conf->apbslv, i, 0);
		
		/*If the device vendor and device id coincides with the ones requested*/
		if ((amba_vendor (conf) == vendor) && (amba_device (conf) == device)) 
		{
			/*If this is the n-th device(index-th) */
			if (cnt == index) 
			{
				/* get the io mem bar */
				iobar = amba_apb_get_membar (amba_conf->apbslv, i);
				
				/*get the device IO mem area*/
				dev->start = amba_iobar_start (amba_conf->apbslv.apbmst[i], iobar);
				
				/*get the device's IRQ*/
				dev->irq = amba_irq (conf);
				
				/*device was sucessfully found*/
				return 1;
			}
			/*this is not our device: increment the number of devices found*/
			cnt++;
		}
	}
	/*desired device was not found. Return*/
	return 0;
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
