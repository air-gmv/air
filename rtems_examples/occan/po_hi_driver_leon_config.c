/*
 * This is GMV solution based on PolyORB-HI-C distribution, a minimal
 * middleware written for generated code from AADL models.
 * You should use it with the Ocarina toolsuite.
 *
 * This file is almost a "copy paste" of 
 * Configuration file for LEON2, GRLIB-LEON2 and LEON3/LEON4 systems 
 * 
 *
 * Initializes,
 *   - Driver manager
 *   - Networking if ENABLE_NETWORK is set
 * 
 * 
 */


#include <stdlib.h>
#include <stdio.h>

#ifdef LEON2_RTEMS_POSIX
#include <drvmgr/leon2_amba_bus.h>
#endif 

//GMV leon3/NGMP only configuration TBD

#include <drvmgr/ambapp_bus_grlib.h>
#include <drvmgr/ambapp_bus.h>
#include <ambapp_ids.h>

/* B1553RT driver configuration (optional) */
struct drvmgr_key grlib_drv_res_b1553rt0[] =
{
#ifdef SET_B1553RT_FREQ
    {"coreFreq", DRVMGR_KT_INT, {(unsigned int)SET_B1553RT_FREQ}},
#endif
    DRVMGR_KEY_EMPTY
};

/* GRPCI driver configuration (optional) */
struct drvmgr_key grlib_drv_res_grpci0[] =
{
    {"byteTwisting", DRVMGR_KT_INT, {(unsigned int)1}},
    DRVMGR_KEY_EMPTY
};

/* GRPCI2 driver configuration (optional) */
struct drvmgr_key grlib_drv_res_grpci2_0[] =
{
#if 0
    {"INTA#", DRVMGR_KT_INT, {(unsigned int)3}},
    {"INTB#", DRVMGR_KT_INT, {(unsigned int)3}},
    {"INTC#", DRVMGR_KT_INT, {(unsigned int)3}},
    {"INTD#", DRVMGR_KT_INT, {(unsigned int)3}},
#endif
    DRVMGR_KEY_EMPTY
};

/* GRGPIO0 driver configuration (optional) */
struct drvmgr_key grlib_drv_res_grgpio0[] =
{
#if 0
    {"nBits", DRVMGR_KT_INT, {(unsigned int)24}},
#endif
    {"int1", DRVMGR_KT_INT,  {(unsigned int)1}},
    {"ptr2", DRVMGR_KT_POINTER,  {(unsigned int)0x23334445}},
    {"str3", DRVMGR_KT_STRING,  {(unsigned int)"STRING_ValUe"}},
    DRVMGR_KEY_EMPTY
};

/* GRGPIO1 driver configuration (optional) */
struct drvmgr_key grlib_drv_res_grgpio1[] =
{
    {"nBits", DRVMGR_KT_INT, {(unsigned int)8}},
    DRVMGR_KEY_EMPTY
};

/* GRGPIO1 driver configuration (optional) */
struct drvmgr_key grlib_drv_res_spictrl0[] =
{
#ifdef SPICTRL_SLV_SEL_FUNC
    {"slvSelFunc", DRVMGR_KT_POINTER, {(unsigned int)SPICTRL_SLV_SEL_FUNC}},
#endif
    DRVMGR_KEY_EMPTY
};

/* If RTEMS_DRVMGR_STARTUP is defined we override the "weak defaults" that
 * is defined by the LEON3 BSP.
 */
struct drvmgr_bus_res grlib_drv_resources =
{
    .next = NULL,
    .resource = {
    {DRIVER_AMBAPP_GAISLER_GRPCI2_ID, 0, &grlib_drv_res_grpci2_0[0]},
#ifdef LITTLE_ENDIAN_PCI_SYSTEM
    /* this configuration option enables PCI byte-twisting which is
     * required for little-endian PCI systems such as for UT699 and
     * the GR-LEON4-ITX board.
     */
    {DRIVER_AMBAPP_GAISLER_GRPCI_ID, 0, &grlib_drv_res_grpci0[0]},
#endif
/*
    {DRIVER_AMBAPP_GAISLER_B1553RT_ID, 0, &grlib_drv_res_b1553rt0[0]},
    {DRIVER_AMBAPP_GAISLER_SPICTRL_ID, 0, &grlib_drv_res_spictrl0[0]},
*/
    {DRIVER_AMBAPP_GAISLER_GRGPIO_ID, 0, &grlib_drv_res_grgpio0[0]},
    DRVMGR_RES_EMPTY
    }
};

#ifndef RTEMS_DRVMGR_STARTUP
struct grlib_config grlib_bus_config = 
{
    &ambapp_plb,        /* AMBAPP bus setup */
    &grlib_drv_resources,    /* Driver configuration */
};
#endif

/* LEON3/NGMP specific system init */
void system_init2(void)
{
#ifndef RTEMS_DRVMGR_STARTUP
    /* Register GRLIB root bus */
    ambapp_grlib_root_register(&grlib_bus_config);
#endif
}



//////
//////
////// RTEMS_PCI_CONFIG_LIB
// #ifdef RTEMS_PCI_CONFIG_LIB does we shall have our own defines TBD
#include <drvmgr/pci_bus.h>

// #ifdef CONFIGURE_DRIVER_PCI_GR_RASTA_IO // shall have our own defines TBD
/* GR-RASTA-IO[0] PCI Peripheral driver configuration example */
struct drvmgr_key rastaio_drv_res_0[] =
{
    /* Note: this is just made up, the GR-RASTA-IO driver does not have
     *       any configuration options at this point.
     */
    {"noDMA", DRVMGR_KT_INT, {(unsigned int)1}},
    DRVMGR_KEY_EMPTY
};

/* GR-RASTA-IO[1] PCI Peripheral driver configuration example */
struct drvmgr_key rastaio_drv_res_1[] =
{
    {"noDMA", DRVMGR_KT_INT, {(unsigned int)0}},
    DRVMGR_KEY_EMPTY
};
// #endif CONFIGURE_DRIVER_PCI_GR_RASTA_IO TBD

// #ifdef CONFIGURE_DRIVER_PCI_GR_LEON4_N2X TBD

/* Driver resources configuration for the PCI bus. Add optional configuration
 * options (driver resources) in the resource array below.
 *
 * Each PCI device instance may be configured.
 */
struct drvmgr_bus_res pcibus_drv_resources = {
    .next = NULL,
    .resource = {
        DRVMGR_RES_EMPTY,
//#ifdef CONFIGURE_DRIVER_PCI_GR_RASTA_IO TBD
        {DRIVER_PCI_GAISLER_RASTAIO_ID, 0, &rastaio_drv_res_0[0]},
        {DRIVER_PCI_GAISLER_RASTAIO_ID, 1, &rastaio_drv_res_1[0]},
//#endif
//#ifdef CONFIGURE_DRIVER_PCI_GR_LEON4_N2X TBD
//        {DRIVER_PCI_GAISLER_LEON4_N2X_ID, 0, &leon4n2x_drv_res_0[0]},
//#endif
    },
};

//#endif //RTEMS_PCI_CONFIG_LIB TBD



//////
//////
////// gr rasta io file
//#ifdef CONFIGURE_DRIVER_PCI_GR_RASTA_IO TBD

#include <bsp/gr_rasta_io.h>
#include <drvmgr/ambapp_bus.h>

/* GR-RASTA-IO boards configuration example. Note that this is 
 * optional, we only override defaults. If default are ok, nothing
 * is need to be done.
 */

/*** CONFIGURES MEMORY CONTROLLER ***/
struct drvmgr_key rastaioN_mctrl0_res[] =
{
#ifdef RASTA_IO_SRAM
    /* SRAM */
#ifdef RASTA_IO_30MHZ
/* Configuration for a GR-RASTA-IO 30MHz board */
    {"mcfg1", DRVMGR_KT_INT, {(unsigned int) 0x000002ff}},
    {"mcfg2", DRVMGR_KT_INT, {(unsigned int) 0x00001260}}, /* 0x00001265 */
    {"mcfg3", DRVMGR_KT_INT, {(unsigned int) 0x000e8000}},
#else
/* Configuration for a GR-RASTA-IO 50MHz board */
    {"mcfg1", DRVMGR_KT_INT, {(unsigned int) 0x0003c2ff}},
    {"mcfg2", DRVMGR_KT_INT, {(unsigned int) 0x00001265}},
    {"mcfg3", DRVMGR_KT_INT, {(unsigned int) 0x00184000}},
#endif
#else
    /* SDRAM */
    {"mcfg1", DRVMGR_KT_INT, {(unsigned int) 0x000002ff}},
    {"mcfg2", DRVMGR_KT_INT, {(unsigned int) 0x82206000}},
    {"mcfg3", DRVMGR_KT_INT, {(unsigned int) 0x000e8000}},
#endif
    DRVMGR_KEY_EMPTY
};

/* Configures Timer prescaler ***/
struct drvmgr_key rastaioN_gptimer0_res[] =
{
/* Set prescaler so that a 10MHz clock is generated */
#ifdef RASTA_IO_30MHZ
    /* 30MHz system needs dividing with 3 */
    {"prescaler", DRVMGR_KT_INT, {(unsigned int) 30/10-1}},
#else
    /* 50MHz system needs dividing with 5 */
    {"prescaler", DRVMGR_KT_INT, {(unsigned int) 50/10-1}},
#endif
    DRVMGR_KEY_EMPTY
};

/* Configuration for a GR-RASTA-IO 1553BRM driver */
struct drvmgr_key rastaio0_b1553brm_res[] = 
{
#ifdef RASTAIO_B1553RBM_REMOTE_ADDR
    /* Use address 0x40000000 on the RASTA-IO board. This will be
     * translated into a CPU address, probably 0xA0000000 on a AT697
     * PCI board.
     */
    {"dmaBaseAdr", DRVMGR_KT_POINTER, {(unsigned int) 0x40000001}}, /* 1 to indicate remote address */
#endif
    DRVMGR_KEY_EMPTY
};

/*** Driver resources for GR-RASTA-IO 0 AMBA PnP bus ***/
struct drvmgr_bus_res gr_rasta_io0_res =
{
    .next = NULL,
    .resource = {
        {DRIVER_AMBAPP_ESA_MCTRL_ID, 0, &rastaioN_mctrl0_res[0]},
        {DRIVER_AMBAPP_GAISLER_GPTIMER_ID, 0, &rastaioN_gptimer0_res[0]},
        {DRIVER_AMBAPP_GAISLER_B1553BRM_ID, 0, &rastaio0_b1553brm_res[0]},
        DRVMGR_RES_EMPTY
    },
};

/*** Driver resources for GR-RASTA-IO 1 AMBA PnP bus ***/
struct drvmgr_bus_res gr_rasta_io1_res =
{
    .next = NULL,
    .resource = {
        {DRIVER_AMBAPP_ESA_MCTRL_ID, 0, &rastaioN_mctrl0_res[0]},
        DRVMGR_RES_EMPTY
    },
};

// #ifdef CONFIGURE_DRIVER_PCI_GR_RASTA_IO TBD

/* Tell GR-RASTA-IO driver about the bus resources.
 * Resources for one GR-RASTA-IO board are available.
 * AMBAPP->PCI->GR-RASTA-IO->AMBAPP bus resources
 *
 * The resources will be used by the drivers for the 
 * cores found on the GR-RASTA-IO->AMBAPP bus.
 *
 * The "weak defaults" are overriden here.
 */
struct drvmgr_bus_res *gr_rasta_io_resources[] = 
{
    &gr_rasta_io0_res,        /* GR-RASTA-IO board 1 resources */
    &gr_rasta_io1_res,        /* GR-RASTA-IO board 2 resources */
    NULL,                /* End of table */
};

void system_init_rastaio(void)
{
    /* Nothing need to be done, the "weak defaults" are overriden by defining
     * gr_rasta_io_resources[]
     */
}
// #endif CONFIGURE_DRIVER_PCI_GR_RASTA_IO TBD

