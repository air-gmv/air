/**
 *  @file
 *  ambapp.h
 *
 *  @brief AMBA Plag & Play Bus Driver Macros for LEON2
 *
 *  Macros used for AMBA Plug & Play bus scanning
 *
 *  Project: RTEMS - Real-Time Executive for Multiprocessor Systems. Partial Modifications by RTEMS Improvement Project (Edisoft S.A.)
 *
 *  COPYRIGHT (c) 2007.
 *  Gaisler Research
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Version | Date        | Name         | Change history
 *  179     | 17/09/2008  | hsilva       | original version
 *  556     | 17/11/2008  | mcoutinho    | IPR 66
 *  3973    | 23/07/2009  | mcoutinho    | IPR 608
 *  5273    | 01/11/2009  | mcoutinho    | IPR 843
 *  7673    | 06/05/2010  | mcoutinho    | IPR 2277
 *  9619    | 23/02/2011  | mcoutinho    | IPR 451
 *  $Rev: 9872 $ | $Date: 2011-03-18 17:01:41 +0000 (Fri, 18 Mar 2011) $| $Author: aconstantino $ | SPR 2819
 *
 **/

/**
 *  @addtogroup SHARED_BETWEEN_SPARC_BSPS Shared between SPARC BSPs
 *  @{
 */

#ifndef __AMBAPP_H__
#define __AMBAPP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define AMBA_CONF_AREA 0xff000
#define AMBA_AHB_SLAVE_CONF_AREA (1 << 11)

#define AMBA_AHB_CONF_WORDS 8
#define AMBA_APB_CONF_WORDS 2
#define AMBA_AHB_MASTERS 16
#define AMBA_AHB_SLAVES 16
#define AMBA_APB_SLAVES 16
#define AMBA_APBUARTS 8

   /* Vendor codes */

   /**
    * @brief determine the GAISLOR vendor number for AMBA bus
    **/
#define VENDOR_GAISLER   1
#define VENDOR_PENDER    2
#define VENDOR_ESA       4
#define VENDOR_OPENCORES 8

   /* Gaisler Research device id's */
#define GAISLER_LEON3     0x03
#define GAISLER_LEON3DSU  0x04
#define GAISLER_ETHAHB    0x05
#define GAISLER_APBMST    0x06
#define GAISLER_AHBUART   0x07
#define GAISLER_SRCTRL    0x08
#define GAISLER_SDCTRL    0x09
#define GAISLER_APBUART   0x0C

   /**
    * @brief determine the GAISLOR vendor IRQ map for AMBA bus
    **/
#define GAISLER_IRQMP     0x0D
#define GAISLER_AHBRAM    0x0E

   /**
    * @brief determine the GAISLOR vendor General Purpose Timer
    **/
#define GAISLER_GPTIMER   0x11

#define GAISLER_PCITRG    0x12
#define GAISLER_PCISBRG   0x13
#define GAISLER_PCIFBRG   0x14
#define GAISLER_PCITRACE  0x15
#define GAISLER_DMACTRL   0x16
#define GAISLER_OCCAN     0x19
#define GAISLER_PIOPORT   0x1A
#define GAISLER_ETHMAC    0x1D
#define GAISLER_SPACEWIRE 0x1f
#define GAISLER_AHB2AHB   0x20
#define GAISLER_I2CMST    0x28
#define GAISLER_GRSPW2    0x29
#define GAISLER_GRCAN     0x34
#define GAISLER_GRHCAN    0x3d
#define GAISLER_GRFIFO    0x35
#define GAISLER_GRADCDAC  0x36
#define GAISLER_GRPULSE   0x37
#define GAISLER_GRTIMER   0x38
#define GAISLER_FTAHBRAM  0x50
#define GAISLER_FTMCTRL   0x54
#define GAISLER_BRM       0x72
#define GAISLER_GRSPW2_DMA 0x8a
#define GAISLER_GRSPW_ROUTER 0x08b

   /* European Space Agency device id's */
#define ESA_LEON2        0x2
#define ESA_MCTRL        0xF
#define ESA_SPW2         0x12

   /* Opencores device id's */
#define OPENCORES_PCIBR  0x4
#define OPENCORES_ETHMAC 0x5

   /*
    *
    * Macros for manipulating Configuration registers
    *
    */
#define amba_get_confword(tab, index, word) (*((tab).addr[(index)]+(word)))

#define amba_vendor(x) (((x) >> 24) & 0xff)

#define amba_device(x) (((x) >> 12) & 0xfff)

#define amba_ahb_get_membar(tab, index, nr) (*((tab).addr[(index)]+4+(nr)))

#define amba_ahb_get_custom(tab, index, nr) (*((tab).addr[(index)]+1+(nr)))

#define amba_apb_get_membar(tab, index) (*((tab).addr[(index)]+1))

#define amba_membar_start(mbar) (((mbar) & 0xfff00000) & (((mbar) & 0xfff0) << 16))

#define amba_iobar_start(base, iobar) ((base) | ((((iobar) & 0xfff00000)>>12) & (((iobar) & 0xfff0)<<4)) )

#define amba_irq(conf) ((conf) & 0x1f)

#define amba_ver(conf) (((conf)>>5) & 0x1f)

#define amba_membar_type(mbar) ((mbar) & 0xf)

#define AMBA_TYPE_APBIO 0x1
#define AMBA_TYPE_MEM   0x2
#define AMBA_TYPE_AHBIO 0x3

#define AMBA_TYPE_AHBIO_ADDR(addr,base_ioarea) ((unsigned int)(base_ioarea) | ((addr) >> 12))

   /*
    *  Types and structure used for AMBA Plug & Play bus scanning
    *
    */
   typedef struct amba_device_table
   {
      volatile int devnr; /* numbrer of devices on AHB or APB bus */
      unsigned int *addr[AMBA_AHB_MASTERS]; /* addresses to the devices configuration tables */
   } amba_device_table;

   typedef struct
   {
      volatile int devnr;
      unsigned int *addr[AMBA_APB_SLAVES]; /* addresses to the devices configuration tables */
      unsigned int apbmst[AMBA_APB_SLAVES]; /* pointer to AHB slave (which is a APB master) */
   } amba_apb_dev;

   struct amba_mmap
   {
      unsigned int cpu_adr;
      unsigned int size;
      unsigned int remote_amba_adr;
   };

   typedef struct _amba_confarea_type amba_confarea_type;

   struct _amba_confarea_type
   {
      amba_confarea_type *next; /* next bus in chain */
      int notroot; /* is root of a bus (mother AHB has 64 masters/slaves rest 16) */
      unsigned int ioarea;
      struct amba_mmap *mmaps;
      amba_device_table ahbmst;
      amba_device_table ahbslv;
      amba_apb_dev apbslv;
   };

   typedef struct
   {
      unsigned int start , irq , bus_id;
   } amba_apb_device;

   typedef struct
   {
      unsigned int start[4] , irq , ver;
   } amba_ahb_device;


   /**
    *  @brief Translate an address based on the mapping information
    *
    *  @param[in] mmaps the mapping information
    *  @param[in] address the address to translate
    *
    *  @return returns the translated address
    */
   unsigned int amba_addr_from(struct amba_mmap *mmaps , unsigned int address);

   /**
    *  @brief scan AMBA information
    *
    *  Scans AMBA Plug&Play Information and convers that information
    *  to a more readable format in RAM.
    *  Will scan for - AHB Masters
    *  - AHB Slaves
    *  - APB Slaves (if a AHB/APB bridge is found)
    *  @param[in] amba_conf AMBA P&P device info is placed here.
    *  @param[in] ioarea address of AMBA Plug&Play information,
    *  on LEON3 systems default is 0xfff00000
    *  @param[out] mmaps Memory mmap specific to this amba bus,
    *  if NULL no translation will be made (default).
    *  A array of maps, ending with a entry with size=0.
    */
   void amba_scan(amba_confarea_type * amba_conf , unsigned int ioarea ,
                  struct amba_mmap *mmaps);

   /***** APB SLAVES *****/

   /**
    *  @brief get first APB slave device of the specified vendor&device id
    *
    *  @param[in] amba_conf AMBA P&P device info is placed here.
    *  @param[in] vendor vendor of the amba device
    *  @param[in] device device to search for
    *  @param[out] dev places in this address the information of the device
    *  (if any found)
    *
    *  @return returns 1 if the device was found and 0 otherwise
    */
   int amba_find_apbslv(amba_confarea_type * amba_conf , int vendor , int device ,
                        amba_apb_device * dev);


   /******** AMBA DEVICES *******/

   /**
    * ESA MEMORY CONTROLLER
    */
   typedef struct
   {
      unsigned int mcfg1;
      unsigned int mcfg2;
      unsigned int mcfg3;
   } ambapp_regmap_mctrl;

   /**
    * APB UART
    */
   typedef struct
   {
      volatile unsigned int data;
      volatile unsigned int status;
      volatile unsigned int ctrl;
      volatile unsigned int scaler;
   } ambapp_apb_uart;

   typedef struct
   {
      volatile unsigned int ilevel;
      volatile unsigned int ipend;
      volatile unsigned int iforce;
      volatile unsigned int iclear;
      volatile unsigned int mpstat;
      volatile unsigned int notused01;
      volatile unsigned int notused02;
      volatile unsigned int notused03;
      volatile unsigned int notused10;
      volatile unsigned int notused11;
      volatile unsigned int notused12;
      volatile unsigned int notused13;
      volatile unsigned int notused20;
      volatile unsigned int notused21;
      volatile unsigned int notused22;
      volatile unsigned int notused23;
      volatile unsigned int mask[16];
      volatile unsigned int force[16];
   } LEON3_IrqCtrl_Regs_Map;

   /*****************************/

#ifdef __cplusplus
}
#endif

#endif

/**  
 *  @}
 */
