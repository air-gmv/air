/*
 * Network configuration
 * 
 ************************************************************
 * EDIT THIS FILE TO REFLECT YOUR NETWORK CONFIGURATION     *
 * BEFORE RUNNING ANY RTEMS PROGRAMS WHICH USE THE NETWORK! * 
 ************************************************************
 *
 *
 * The GRETH and SMC driver add them self to the interface list below,
 * they lookup MAC and IP addresses automatically from the 
 * interface_configs[] array below.
 */

#include <bsp/network_interface_add.h>

/* #define RTEMS_USE_BOOTP */

/* #include <bsp.h> */


/*
 * Network configuration
 */



struct rtems_bsdnet_config rtems_bsdnet_config = {
//        &loopback_config,       /* Network interface */
  NULL,
   NULL, // not using rtems_bsdnet_do_bootp,
   150,                  /* default network task priority */
   128*1024,             /* mbuf capacity */
   256*1024,             /* mbuf cluster capacity */

   
 //  "rtemshost",         /* Host name */
 //  "localnet",           /* Domain name */
 //  "192.168.0.1",    /* Gateway */
 //  "192.168.0.1",           /* Log host */
 //  {"192.168.0.1"}, /* Name server(s) */
 //  {"192.168.0.1"}, /* NTP server(s) */
   
//   2,                    /* sb_efficiency */
//   9216,                 /* UDP TX */
//   40 * 1024,            /* UDP RX */
//   128 * 1024,           /* TCP TX */
//   128 * 1024            /* TCP RX */
};

/* Table used by network interfaces that register themselves using the
 * network_interface_add routine. From this table the IP address, netmask 
 * and Ethernet MAC address of an interface is taken.
 *
 * The network_interface_add routine puts the interface into the
 * rtems_bsnet_config.ifconfig list.
 *
 * Set IP Address and Netmask to NULL to select BOOTP.
 */
struct ethernet_config interface_configs[] =
{
	{ "10.0.0.40", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x7A}},
//	{ "192.168.0.30", "255.255.255.0", {0x46, 0x9C, 0x1B, 0x45, 0x9A, 0x23}},
//	{ "192.168.0.46", "255.255.255.0", {0xD9, 0x17, 0x1E, 0xC3, 0x9D, 0x10}}
//	{ "192.168.3.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x7C}},
//	{ "192.168.4.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x7D}},
//	{ "192.168.5.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x7E}},
//	{ "192.168.6.67", "255.255.255.0", {0x00, 0x80, 0x7F, 0x22, 0x61, 0x7F}},
//	{NULL, NULL, {0,0,0,0,0,0}}
};
#define INTERFACE_CONFIG_CNT (sizeof(interface_configs)/sizeof(struct ethernet_config) - 1)
