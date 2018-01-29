/*
 *  AMBA Plug & Play routines
 *
 *  COPYRIGHT (c) 2011.
 *  Aeroflex Gaisler.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <string.h>
#include <stdlib.h>
#include <string.h>

#include <ambapp.h>
#include <bsp.h>

#define AMBA_CONF_AREA 0xff000
#define AMBA_AHB_SLAVE_CONF_AREA (1 << 11)
#define AMBA_APB_SLAVES 16

/* Allocate one AMBA device */
static struct ambapp_dev *ambapp_alloc_dev_struct(int dev_type)
{
  struct ambapp_dev *dev;
  size_t size = sizeof(*dev);

  if (dev_type == DEV_APB_SLV)
    size += sizeof(struct ambapp_apb_info);
  else
    size += sizeof(struct ambapp_ahb_info); /* AHB */
  dev = (struct ambapp_dev *)calloc(1, size);
  if (dev != NULL)
    dev->dev_type = dev_type;
  return dev;
}

static unsigned int
ambapp_addr_from (struct ambapp_mmap *mmaps, unsigned int address)
{
  /* no translation? */
  if (!mmaps)
    return address;

  while (mmaps->size) {
    if ((address >= mmaps->remote_adr) &&
        (address <= (mmaps->remote_adr + (mmaps->size - 1)))) {
      return (address - mmaps->remote_adr) + mmaps->local_adr;
    }
    mmaps++;
  }
  return 1;
}

static void ambapp_ahb_dev_init(
  unsigned int ioarea,
  struct ambapp_mmap *mmaps,
  struct ambapp_pnp_ahb *ahb,
  struct ambapp_dev *dev,
  int ahbidx
  )
{
  int bar;
  struct ambapp_ahb_info *ahb_info;
  unsigned int addr, mask, mbar;

  /* Setup device struct */
  dev->vendor = ambapp_pnp_vendor(ahb->id);
  dev->device = ambapp_pnp_device(ahb->id);
  ahb_info = DEV_TO_AHB(dev);
  ahb_info->ver = ambapp_pnp_ver(ahb->id);
  ahb_info->irq = ambapp_pnp_irq(ahb->id);
  ahb_info->ahbidx = ahbidx;
  ahb_info->custom[0] = (unsigned int)ahb->custom[0];
  ahb_info->custom[1] = (unsigned int)ahb->custom[1];
  ahb_info->custom[2] = (unsigned int)ahb->custom[2];

  /* Memory BARs */
  for (bar=0; bar<4; bar++) {
    mbar = ahb->mbar[bar];
    if (mbar == 0) {
      addr = 0;
      mask = 0;
    } else {
      addr = ambapp_pnp_start(mbar);
      if (ambapp_pnp_mbar_type(mbar) == AMBA_TYPE_AHBIO) {
        /* AHB I/O area is releative IO_AREA */
        addr = AMBA_TYPE_AHBIO_ADDR(addr, ioarea);
        mask = (((unsigned int)(ambapp_pnp_mbar_mask(~mbar) << 8) | 0xff)) + 1;
      } else {
        /* AHB memory area, absolute address */
        addr = ambapp_addr_from(mmaps, addr);
        mask = (~((unsigned int)(ambapp_pnp_mbar_mask(mbar) << 20))) + 1;
      }
    }
    ahb_info->start[bar] = addr;
    ahb_info->mask[bar] = mask;
    ahb_info->type[bar] = ambapp_pnp_mbar_type(mbar);
  }
}

static void ambapp_apb_dev_init(
  unsigned int base,
  struct ambapp_mmap *mmaps,
  struct ambapp_pnp_apb *apb,
  struct ambapp_dev *dev,
  int ahbidx
  )
{
  struct ambapp_apb_info *apb_info;

  /* Setup device struct */
  dev->vendor = ambapp_pnp_vendor(apb->id);
  dev->device = ambapp_pnp_device(apb->id);
  apb_info = DEV_TO_APB(dev);
  apb_info->ver = ambapp_pnp_ver(apb->id);
  apb_info->irq = ambapp_pnp_irq(apb->id);
  apb_info->ahbidx = ahbidx;
  apb_info->start = ambapp_pnp_apb_start(apb->iobar, base);
  apb_info->mask = ambapp_pnp_apb_mask(apb->iobar);
}

static int ambapp_add_ahbbus(
  struct ambapp_bus *abus,
  unsigned int ioarea
  )
{
  int i;
  for (i=0; i<AHB_BUS_MAX; i++) {
    if (abus->ahbs[i].ioarea == 0) {
      abus->ahbs[i].ioarea = ioarea;
      return i;
    } else if (abus->ahbs[i].ioarea == ioarea) {
      /* Bus already added */
      return -1;
    }
  }
  return -1;
}

/* Internal AMBA Scanning Function */
static int ambapp_scan2(
  struct ambapp_bus *abus,
  unsigned int ioarea,
  ambapp_memcpy_t memfunc,
  struct ambapp_dev *parent,
  struct ambapp_dev **root
  )
{
  struct ambapp_pnp_ahb *ahb, ahb_buf;
  struct ambapp_pnp_apb *apb, apb_buf;
  struct ambapp_dev *dev, *prev, *prevapb, *apbdev;
  struct ambapp_ahb_info *ahb_info;
  int maxloops = 64;
  unsigned int apbbase, bridge_adr;
  int i, j, ahbidx;

  *root = NULL;

  if (parent) {
    /* scan first bus for 64 devices, rest for 16 devices */
    maxloops = 16;
  }

  ahbidx = ambapp_add_ahbbus(abus, ioarea);
  if (ahbidx < 0) {
    /* Bus already scanned, stop */
    return 0;
  }

  prev = parent;

  /* AHB MASTERS */
  ahb = (struct ambapp_pnp_ahb *) (ioarea | AMBA_CONF_AREA);
  for (i = 0; i < maxloops; i++, ahb++) {
    memfunc(&ahb_buf, ahb, sizeof(struct ambapp_pnp_ahb), abus);
    if (ahb_buf.id == 0)
      continue;

    /* An AHB device present here */
    dev = ambapp_alloc_dev_struct(DEV_AHB_MST);
    if (!dev)
      return -1;

    ambapp_ahb_dev_init(ioarea, abus->mmaps, &ahb_buf, dev, ahbidx);

    if (*root == NULL)
      *root = dev;

    if (prev != parent)
      prev->next = dev;
    dev->prev = prev;
    prev = dev;
  }

  /* AHB SLAVES */
  ahb = (struct ambapp_pnp_ahb *)
    (ioarea | AMBA_CONF_AREA | AMBA_AHB_SLAVE_CONF_AREA);
  for (i = 0; i < maxloops; i++, ahb++) {
    memfunc(&ahb_buf, ahb, sizeof(struct ambapp_pnp_ahb), abus);
    if (ahb_buf.id == 0)
      continue;

    /* An AHB device present here */
    dev = ambapp_alloc_dev_struct(DEV_AHB_SLV);
    if (!dev)
      return -1;

    ambapp_ahb_dev_init(ioarea, abus->mmaps, &ahb_buf, dev, ahbidx);

    if (*root == NULL)
      *root = dev;

    if (prev != parent)
      prev->next = dev;
    dev->prev = prev;
    prev = dev;

    ahb_info = DEV_TO_AHB(dev);

    /* Is it a AHB/AHB Bridge ? */
    if (((dev->device == GAISLER_AHB2AHB) &&
        (dev->vendor == VENDOR_GAISLER) && (ahb_info->ver > 0)) ||
        ((dev->device == GAISLER_L2CACHE) &&
        (dev->vendor == VENDOR_GAISLER)) ||
        ((dev->device == GAISLER_GRIOMMU) &&
        (dev->vendor == VENDOR_GAISLER))) {
      /* AHB/AHB Bridge Found, recurse down the
       * Bridge
       */
      if (ahb_info->custom[1] != 0) {
        bridge_adr = ambapp_addr_from(abus->mmaps,
              ahb_info->custom[1]);
        /* Scan next bus if not already scanned */
        if (ambapp_scan2(abus, bridge_adr, memfunc, dev,
            &dev->children))
          return -1;
      }
    } else if ((dev->device == GAISLER_APBMST) &&
               (dev->vendor == VENDOR_GAISLER)) {
      /* AHB/APB Bridge Found, add the APB devices to this
       * AHB Slave's children
       */
      prevapb = dev;
      apbbase = ahb_info->start[0];

      /* APB SLAVES */
      apb = (struct ambapp_pnp_apb *)
        (apbbase | AMBA_CONF_AREA);
      for (j=0; j<AMBA_APB_SLAVES; j++, apb++) {
        memfunc(&apb_buf, apb, sizeof(*apb), abus);
        if (apb_buf.id == 0)
          continue;

        apbdev = ambapp_alloc_dev_struct(DEV_APB_SLV);
        if (!apbdev)
          return -1;

        ambapp_apb_dev_init(apbbase, abus->mmaps,
                            &apb_buf, apbdev, ahbidx);

        if (prevapb != dev)
          prevapb->next = apbdev;
        else
          dev->children = apbdev;
        apbdev->prev = prevapb;
        prevapb = apbdev;
      }
    }
  }

  /* Remember first AHB MST/SLV device on bus and Parent Bridge */
  abus->ahbs[ahbidx].dev = *root;
  abus->ahbs[ahbidx].bridge = parent;

  return 0;
}

/* Build AMBA Plug & Play device graph */
int ambapp_scan(
  struct ambapp_bus *abus,
  unsigned int ioarea,
  ambapp_memcpy_t memfunc,
  struct ambapp_mmap *mmaps
  )
{
  memset(abus, 0, sizeof(*abus));
  abus->mmaps = mmaps;

  /* Default to memcpy() */
  if (!memfunc)
    memfunc = (ambapp_memcpy_t)memcpy;

  return ambapp_scan2(abus, ioarea, memfunc, NULL, &abus->root);
}

/* Match search options againt device */
static int ambapp_dev_match_options(struct ambapp_dev *dev, unsigned int options, int vendor, int device)
{
  if ((((options & (OPTIONS_ALL_DEVS)) == OPTIONS_ALL_DEVS) || /* TYPE */
      ((options & OPTIONS_AHB_MSTS) && (dev->dev_type == DEV_AHB_MST)) ||
      ((options & OPTIONS_AHB_SLVS) && (dev->dev_type == DEV_AHB_SLV)) ||
      ((options & OPTIONS_APB_SLVS) && (dev->dev_type == DEV_APB_SLV))) &&
      ((vendor == -1) || (vendor == dev->vendor)) && /* VENDOR/DEV ID */
      ((device == -1) || (device == dev->device)) &&
      (((options & OPTIONS_ALL) == OPTIONS_ALL) || /* Allocated State */
      ((options & OPTIONS_FREE) && DEV_IS_FREE(dev)) ||
      ((options & OPTIONS_ALLOCATED) && DEV_IS_ALLOCATED(dev)))) {
    return 1;
  }
  return 0;
}

/* If device is an APB bridge all devices on the APB bridge is processed */
static int ambapp_for_each_apb(
  struct ambapp_dev *dev,
  unsigned int options,
  int vendor,
  int device,
  ambapp_func_t func,
  void *arg)
{
  int index, ret;
  struct ambapp_dev *apbslv;

  ret = 0;
  if (dev->children && (dev->children->dev_type == DEV_APB_SLV)) {
    /* Found a APB Bridge */
    index = 0;
    apbslv = dev->children;
    while (apbslv) {
      if (ambapp_dev_match_options(apbslv, options,
                                   vendor, device) == 1) {
        ret = func(apbslv, index, arg);
        if (ret != 0)
          break; /* Signalled stopped */
      }
      index++;
      apbslv = apbslv->next;
    }
  }

  return ret;
}


/**** AHB Slaves ****/

int amba_find_ahbslv(amba_confarea_type * amba_conf , int vendor , int device ,
                     amba_ahb_device * dev)
{
    /* APB device configuration word */
    unsigned int conf;

    /* APB slave mem bar */
    unsigned int iobar;

    /* iterator index through the APB slave devices */
    int i;


    /* iterate through the APB slave devices */
    for(i = 0; i < amba_conf->ahbslv.devnr; i++)
    {
        /* get the configuration area */
        conf = amba_get_confword(amba_conf->ahbslv , i , 0);

        /* check the device vendor */
        if(( amba_vendor(conf) == vendor ) && ( amba_device(conf) == device ))
        {
            /* get the io mem bar */
            iobar = amba_ahb_get_membar(amba_conf->ahbslv , i, 0);

            /* get the device start address */
            dev->start[0] = amba_iobar_start(*(amba_conf->ahbslv.addr[i]) , iobar);

            /* get the devide interrupt number */
            dev->irq = amba_irq(conf);

            /* found the device */
            return 1;
        }
    }
  }

    /* did not find the device */
    return 0;
}

int ambapp_for_each(
  struct ambapp_bus *abus,
  unsigned int options,
  int vendor,
  int device,
  ambapp_func_t func,
  void *arg)
{
  return ambapp_for_each_dev(abus->root, options, vendor, device, func, arg);
}
